// HPC-AI-Optimization-Lab Python Bindings
// Thin nanobind wrappers over selected CUDA kernels.

#include <cuda_runtime.h>

#include <nanobind/nanobind.h>
#include <nanobind/ndarray.h>

#include "elementwise/relu.cuh"
#include "elementwise/sigmoid.cuh"
#include "elementwise/transpose.cuh"
#include "gemm/gemm.cuh"
#include "reduction/layernorm.cuh"
#include "reduction/rmsnorm.cuh"
#include "reduction/softmax.cuh"

#include "validation.hpp"

namespace nb = nanobind;

namespace {

using hpc::bindings::GemmOperands;
using hpc::bindings::NormOperands;
using hpc::bindings::SoftmaxOperands;
using hpc::bindings::validate_elementwise_io;
using hpc::bindings::validate_transpose_io;

}  // namespace

void relu_wrapper(nb::ndarray<float, nb::device::cuda>& input,
                  nb::ndarray<float, nb::device::cuda>& output) {
    const size_t n = validate_elementwise_io(input, output);
    hpc::elementwise::relu<float, hpc::elementwise::OptLevel::GridStride>(
        input.data(), output.data(), n, nullptr);
}

void sigmoid_wrapper(nb::ndarray<float, nb::device::cuda>& input,
                     nb::ndarray<float, nb::device::cuda>& output) {
    const size_t n = validate_elementwise_io(input, output);
    hpc::elementwise::sigmoid<float, hpc::elementwise::OptLevel::GridStride>(
        input.data(), output.data(), n, nullptr);
}

void transpose_wrapper(nb::ndarray<float, nb::device::cuda>& input,
                       nb::ndarray<float, nb::device::cuda>& output, int rows, int cols) {
    validate_transpose_io(input, output, rows, cols);
    hpc::elementwise::transpose<float, hpc::elementwise::TransposeOpt::SharedMemPadded>(
        input.data(), output.data(), rows, cols, nullptr);
}

void softmax_wrapper(nb::ndarray<float, nb::device::cuda>& input,
                     nb::ndarray<float, nb::device::cuda>& output, int batch, int seq_len) {
    SoftmaxOperands op(batch, seq_len);
    op.validate_tensors(input, output);
    hpc::reduction::softmax<float, hpc::reduction::SoftmaxOpt::OnlineSoftmax>(
        input.data(), output.data(), batch, seq_len, nullptr);
}

void layer_norm_wrapper(nb::ndarray<float, nb::device::cuda>& input,
                        nb::ndarray<float, nb::device::cuda>& gamma,
                        nb::ndarray<float, nb::device::cuda>& beta,
                        nb::ndarray<float, nb::device::cuda>& output, int batch, int hidden_size,
                        float eps) {
    NormOperands op(batch, hidden_size, eps);
    op.validate_io_with_beta(input, output, gamma, beta);
    hpc::reduction::layer_norm<float>(input.data(), gamma.data(), beta.data(), output.data(), batch,
                                      hidden_size, eps, nullptr);
}

void rms_norm_wrapper(nb::ndarray<float, nb::device::cuda>& input,
                      nb::ndarray<float, nb::device::cuda>& gamma,
                      nb::ndarray<float, nb::device::cuda>& output, int batch, int hidden_size,
                      float eps) {
    NormOperands op(batch, hidden_size, eps);
    op.validate_io(input, output, gamma);
    hpc::reduction::rms_norm<float>(input.data(), gamma.data(), output.data(), batch, hidden_size,
                                    eps, nullptr);
}

void matmul_wrapper(nb::ndarray<float, nb::device::cuda>& A, nb::ndarray<float, nb::device::cuda>& B,
                    nb::ndarray<float, nb::device::cuda>& C, int M, int N, int K, float alpha,
                    float beta) {
    GemmOperands op(M, N, K, alpha, beta);
    op.validate_tensors(A, B, C);
    hpc::gemm::gemm<float, hpc::gemm::GemmOpt::Auto>(
        A.data(), B.data(), C.data(), M, N, K, alpha, beta, nullptr);
}

void matmul_cutlass_wrapper(nb::ndarray<float, nb::device::cuda>& A,
                            nb::ndarray<float, nb::device::cuda>& B,
                            nb::ndarray<float, nb::device::cuda>& C, int M, int N, int K,
                            float alpha, float beta) {
    GemmOperands op(M, N, K, alpha, beta);
    op.validate_tensors(A, B, C);
    hpc::gemm::gemm_cutlass<float>(A.data(), B.data(), C.data(), M, N, K, alpha, beta, nullptr);
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
    gemm.def("matmul_cutlass", &matmul_cutlass_wrapper,
             "CUTLASS-backed matrix multiplication baseline");
}
