// HPC-AI-Optimization-Lab Python Bindings
// Thin nanobind wrappers over selected CUDA kernels.

#include <nanobind/nanobind.h>
#include <nanobind/tensor.h>

#include <cmath>
#include <stdexcept>
#include <string>

#include <cuda_runtime.h>

#include "01_elementwise/relu.cuh"
#include "01_elementwise/sigmoid.cuh"
#include "01_elementwise/transpose.cuh"
#include "02_reduction/layernorm.cuh"
#include "02_reduction/rmsnorm.cuh"
#include "02_reduction/softmax.cuh"
#include "03_gemm/gemm.cuh"

namespace nb = nanobind;

namespace {

template <typename T>
size_t require_non_empty(const nb::tensor<T, nb::device::cuda>& tensor, const char* name) {
    const size_t size = tensor.size();
    if (size == 0) {
        throw std::invalid_argument(std::string(name) + " must not be empty");
    }
    return size;
}

inline void require_size(size_t actual, size_t expected, const char* name) {
    if (actual != expected) {
        throw std::invalid_argument(
            std::string(name) + " has unexpected size: expected " +
            std::to_string(expected) + ", got " + std::to_string(actual));
    }
}

inline size_t require_positive_product(int lhs, int rhs, const char* lhs_name, const char* rhs_name) {
    if (lhs <= 0 || rhs <= 0) {
        throw std::invalid_argument(std::string(lhs_name) + " and " + rhs_name + " must be positive");
    }
    return static_cast<size_t>(lhs) * static_cast<size_t>(rhs);
}

inline void require_finite_positive(float value, const char* name) {
    if (!std::isfinite(value) || value <= 0.0f) {
        throw std::invalid_argument(std::string(name) + " must be finite and positive");
    }
}

inline void require_finite(float value, const char* name) {
    if (!std::isfinite(value)) {
        throw std::invalid_argument(std::string(name) + " must be finite");
    }
}

} // namespace

void relu_wrapper(nb::tensor<float, nb::device::cuda>& input,
                  nb::tensor<float, nb::device::cuda>& output) {
    const size_t n = require_non_empty(input, "input");
    require_size(output.size(), n, "output");
    hpc::elementwise::relu<float, hpc::elementwise::OptLevel::GridStride>(
        input.data(), output.data(), n, nullptr);
}

void sigmoid_wrapper(nb::tensor<float, nb::device::cuda>& input,
                     nb::tensor<float, nb::device::cuda>& output) {
    const size_t n = require_non_empty(input, "input");
    require_size(output.size(), n, "output");
    hpc::elementwise::sigmoid<float, hpc::elementwise::OptLevel::GridStride>(
        input.data(), output.data(), n, nullptr);
}

void transpose_wrapper(nb::tensor<float, nb::device::cuda>& input,
                       nb::tensor<float, nb::device::cuda>& output,
                       int rows, int cols) {
    const size_t expected = require_positive_product(rows, cols, "rows", "cols");
    require_size(input.size(), expected, "input");
    require_size(output.size(), expected, "output");
    hpc::elementwise::transpose<float, hpc::elementwise::TransposeOpt::SharedMemPadded>(
        input.data(), output.data(), rows, cols, nullptr);
}

void softmax_wrapper(nb::tensor<float, nb::device::cuda>& input,
                     nb::tensor<float, nb::device::cuda>& output,
                     int batch, int seq_len) {
    const size_t expected = require_positive_product(batch, seq_len, "batch", "seq_len");
    require_size(input.size(), expected, "input");
    require_size(output.size(), expected, "output");
    hpc::reduction::softmax<float, hpc::reduction::SoftmaxOpt::OnlineSoftmax>(
        input.data(), output.data(), batch, seq_len, nullptr);
}

void layer_norm_wrapper(nb::tensor<float, nb::device::cuda>& input,
                        nb::tensor<float, nb::device::cuda>& gamma,
                        nb::tensor<float, nb::device::cuda>& beta,
                        nb::tensor<float, nb::device::cuda>& output,
                        int batch, int hidden_size, float eps) {
    const size_t expected = require_positive_product(batch, hidden_size, "batch", "hidden_size");
    require_finite_positive(eps, "eps");
    require_size(input.size(), expected, "input");
    require_size(output.size(), expected, "output");
    require_size(gamma.size(), static_cast<size_t>(hidden_size), "gamma");
    require_size(beta.size(), static_cast<size_t>(hidden_size), "beta");
    hpc::reduction::layer_norm<float>(
        input.data(), gamma.data(), beta.data(), output.data(),
        batch, hidden_size, eps, nullptr);
}

void rms_norm_wrapper(nb::tensor<float, nb::device::cuda>& input,
                      nb::tensor<float, nb::device::cuda>& gamma,
                      nb::tensor<float, nb::device::cuda>& output,
                      int batch, int hidden_size, float eps) {
    const size_t expected = require_positive_product(batch, hidden_size, "batch", "hidden_size");
    require_finite_positive(eps, "eps");
    require_size(input.size(), expected, "input");
    require_size(output.size(), expected, "output");
    require_size(gamma.size(), static_cast<size_t>(hidden_size), "gamma");
    hpc::reduction::rms_norm<float>(
        input.data(), gamma.data(), output.data(),
        batch, hidden_size, eps, nullptr);
}

void matmul_wrapper(nb::tensor<float, nb::device::cuda>& A,
                    nb::tensor<float, nb::device::cuda>& B,
                    nb::tensor<float, nb::device::cuda>& C,
                    int M, int N, int K,
                    float alpha, float beta) {
    const size_t a_expected = require_positive_product(M, K, "M", "K");
    const size_t b_expected = require_positive_product(K, N, "K", "N");
    const size_t c_expected = require_positive_product(M, N, "M", "N");
    require_finite(alpha, "alpha");
    require_finite(beta, "beta");
    require_size(A.size(), a_expected, "A");
    require_size(B.size(), b_expected, "B");
    require_size(C.size(), c_expected, "C");
    hpc::gemm::gemm<float, hpc::gemm::GemmOpt::SharedMemTiling>(
        A.data(), B.data(), C.data(), M, N, K, alpha, beta, nullptr);
}

NB_MODULE(hpc_ai_opt, m) {
    m.doc() = "HPC-AI-Optimization-Lab CUDA Kernels";

    auto elementwise = m.def_submodule("elementwise", "Elementwise operations");
    elementwise.def("relu", &relu_wrapper, "ReLU activation");
    elementwise.def("sigmoid", &sigmoid_wrapper, "Sigmoid activation");
    elementwise.def("transpose", &transpose_wrapper, "Matrix transpose");

    auto reduction = m.def_submodule("reduction", "Reduction operations");
    reduction.def("softmax", &softmax_wrapper, "Softmax");
    reduction.def("layer_norm", &layer_norm_wrapper, "Layer normalization");
    reduction.def("rms_norm", &rms_norm_wrapper, "RMS normalization");

    auto gemm = m.def_submodule("gemm", "Matrix multiplication");
    gemm.def("matmul", &matmul_wrapper, "Matrix multiplication");
}
