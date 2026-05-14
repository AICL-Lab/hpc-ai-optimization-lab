#pragma once

#include <nanobind/nanobind.h>
#include <nanobind/ndarray.h>

#include <cmath>
#include <stdexcept>
#include <string>

namespace nb = nanobind;

namespace hpc::bindings {

// ---------------------------------------------------------------------------
// Low-level tensor validation primitives
// ---------------------------------------------------------------------------

template <typename T>
size_t validate_tensor(const nb::ndarray<T, nb::device::cuda>& tensor, const char* name,
                       bool require_contiguous = true) {
    const size_t size = tensor.size();
    if (size == 0) {
        throw std::invalid_argument(std::string(name) + " must not be empty");
    }
    if (require_contiguous) {
        size_t expected_stride = 1;
        for (size_t axis = tensor.ndim(); axis > 0; --axis) {
            const size_t index = axis - 1;
            if (tensor.stride(index) != static_cast<int64_t>(expected_stride)) {
                throw std::invalid_argument(std::string(name) + " must be contiguous in memory");
            }
            expected_stride *= tensor.shape(index);
        }
    }
    return size;
}

template <typename T>
void validate_ndim(const nb::ndarray<T, nb::device::cuda>& tensor, size_t expected_ndim,
                   const char* name) {
    if (tensor.ndim() != expected_ndim) {
        throw std::invalid_argument(std::string(name) + " must be a " +
                                    std::to_string(expected_ndim) + "D CUDA ndarray");
    }
}

template <typename T>
void validate_size(const nb::ndarray<T, nb::device::cuda>& tensor, size_t expected_size,
                   const char* name) {
    if (tensor.size() != expected_size) {
        throw std::invalid_argument(std::string(name) + " has unexpected size: expected " +
                                    std::to_string(expected_size) + ", got " +
                                    std::to_string(tensor.size()));
    }
}

template <typename T>
void validate_matrix(const nb::ndarray<T, nb::device::cuda>& tensor, int rows, int cols,
                       const char* name) {
    validate_ndim(tensor, 2, name);
    if (tensor.shape(0) != static_cast<size_t>(rows) ||
        tensor.shape(1) != static_cast<size_t>(cols)) {
        throw std::invalid_argument(std::string(name) + " has unexpected shape");
    }
    validate_size(tensor, static_cast<size_t>(rows) * static_cast<size_t>(cols), name);
}

template <typename T>
void validate_vector(const nb::ndarray<T, nb::device::cuda>& tensor, int size, const char* name) {
    validate_ndim(tensor, 1, name);
    validate_size(tensor, static_cast<size_t>(size), name);
}

// ---------------------------------------------------------------------------
// Scalar checks
// ---------------------------------------------------------------------------

inline void validate_positive(int value, const char* name) {
    if (value <= 0) {
        throw std::invalid_argument(std::string(name) + " must be positive");
    }
}

inline size_t validate_positive_product(int lhs, int rhs, const char* lhs_name,
                                          const char* rhs_name) {
    if (lhs <= 0 || rhs <= 0) {
        throw std::invalid_argument(std::string(lhs_name) + " and " + rhs_name +
                                    " must be positive");
    }
    return static_cast<size_t>(lhs) * static_cast<size_t>(rhs);
}

inline void validate_finite(float value, const char* name) {
    if (!std::isfinite(value)) {
        throw std::invalid_argument(std::string(name) + " must be finite");
    }
}

inline void validate_finite_positive(float value, const char* name) {
    if (!std::isfinite(value) || value <= 0.0f) {
        throw std::invalid_argument(std::string(name) + " must be finite and positive");
    }
}

// ---------------------------------------------------------------------------
// High-level operation-family validation helpers
// Each helper is a deep module: a small interface hiding the full
// validation surface for that operation family.
// ---------------------------------------------------------------------------

// Elementwise: input/output pair of identical size.
template <typename T>
size_t validate_elementwise_io(const nb::ndarray<T, nb::device::cuda>& input,
                               const nb::ndarray<T, nb::device::cuda>& output,
                               const char* input_name = "input",
                               const char* output_name = "output") {
    size_t n = validate_tensor(input, input_name);
    validate_tensor(output, output_name);
    validate_size(output, n, output_name);
    return n;
}

// Transpose: input(rows x cols) -> output(cols x rows).
template <typename T>
void validate_transpose_io(const nb::ndarray<T, nb::device::cuda>& input,
                           const nb::ndarray<T, nb::device::cuda>& output, int rows, int cols,
                           const char* input_name = "input",
                           const char* output_name = "output") {
    const size_t expected = validate_positive_product(rows, cols, "rows", "cols");
    validate_matrix(input, rows, cols, input_name);
    validate_matrix(output, cols, rows, output_name);
    validate_size(input, expected, input_name);
    validate_size(output, expected, output_name);
}

// GEMM operand bundle.
// Validates scalar dimensions and, on demand, the tensor shapes.
struct GemmOperands {
    int M;
    int N;
    int K;
    float alpha;
    float beta;

    GemmOperands(int m, int n, int k, float a, float b) : M(m), N(n), K(k), alpha(a), beta(b) {
        validate_positive_product(M, K, "M", "K");
        validate_positive_product(K, N, "K", "N");
        validate_positive_product(M, N, "M", "N");
        validate_finite(alpha, "alpha");
        validate_finite(beta, "beta");
    }

    template <typename T>
    void validate_tensors(const nb::ndarray<T, nb::device::cuda>& A,
                          const nb::ndarray<T, nb::device::cuda>& B,
                          const nb::ndarray<T, nb::device::cuda>& C,
                          const char* a_name = "A",
                          const char* b_name = "B",
                          const char* c_name = "C") const {
        validate_matrix(A, M, K, a_name);
        validate_matrix(B, K, N, b_name);
        validate_matrix(C, M, N, c_name);
    }
};

// Softmax operand bundle.
struct SoftmaxOperands {
    int batch;
    int seq_len;

    SoftmaxOperands(int b, int s) : batch(b), seq_len(s) {
        validate_positive_product(batch, seq_len, "batch", "seq_len");
    }

    template <typename T>
    void validate_tensors(const nb::ndarray<T, nb::device::cuda>& input,
                          const nb::ndarray<T, nb::device::cuda>& output,
                          const char* input_name = "input",
                          const char* output_name = "output") const {
        validate_matrix(input, batch, seq_len, input_name);
        validate_matrix(output, batch, seq_len, output_name);
    }
};

// LayerNorm / RMSNorm operand bundle.
struct NormOperands {
    int batch;
    int hidden_size;
    float eps;

    NormOperands(int b, int h, float e) : batch(b), hidden_size(h), eps(e) {
        validate_positive_product(batch, hidden_size, "batch", "hidden_size");
        validate_finite_positive(eps, "eps");
    }

    template <typename T>
    void validate_io(const nb::ndarray<T, nb::device::cuda>& input,
                     const nb::ndarray<T, nb::device::cuda>& output,
                     const nb::ndarray<T, nb::device::cuda>& gamma,
                     const char* input_name = "input",
                     const char* output_name = "output",
                     const char* gamma_name = "gamma") const {
        validate_matrix(input, batch, hidden_size, input_name);
        validate_matrix(output, batch, hidden_size, output_name);
        validate_vector(gamma, hidden_size, gamma_name);
    }

    template <typename T>
    void validate_io_with_beta(const nb::ndarray<T, nb::device::cuda>& input,
                               const nb::ndarray<T, nb::device::cuda>& output,
                               const nb::ndarray<T, nb::device::cuda>& gamma,
                               const nb::ndarray<T, nb::device::cuda>& beta,
                               const char* input_name = "input",
                               const char* output_name = "output",
                               const char* gamma_name = "gamma",
                               const char* beta_name = "beta") const {
        validate_io(input, output, gamma, input_name, output_name, gamma_name);
        validate_vector(beta, hidden_size, beta_name);
    }
};

}  // namespace hpc::bindings
