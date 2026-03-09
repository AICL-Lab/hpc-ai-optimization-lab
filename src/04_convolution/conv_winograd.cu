#include "conv_winograd.cuh"
#include "conv_implicit_gemm.cuh"
#include "../common/cuda_check.cuh"

namespace hpc::convolution {

// Winograd F(2x2, 3x3) transformation matrices
// TODO: Implement full Winograd convolution

template <>
void conv2d_winograd<float>(const float* input, const float* weight, float* output,
                            int batch, int in_channels, int out_channels,
                            int height, int width, cudaStream_t stream) {
    // Placeholder - full implementation requires Winograd transforms
    // Fall back to implicit GEMM with 3x3 kernel, stride=1, pad=1
    ConvParams params{batch, in_channels, out_channels, height, width,
                      3, 3, 1, 1, 1, 1, 1, 1};
    conv2d_implicit_gemm<float>(input, weight, output, params, stream);
}

} // namespace hpc::convolution
