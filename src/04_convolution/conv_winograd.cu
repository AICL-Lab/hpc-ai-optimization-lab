#include "conv_winograd.cuh"
#include "conv_implicit_gemm.cuh"
#include "../common/cuda_check.cuh"
#include <stdexcept>

namespace hpc::convolution {

// Experimental wrapper: until Winograd transforms are implemented, this path
// intentionally falls back to the validated implicit GEMM implementation.
template <>
void conv2d_winograd<float>(const float* input, const float* weight, float* output,
                            int batch, int in_channels, int out_channels,
                            int height, int width, cudaStream_t stream) {
    if (input == nullptr || weight == nullptr || output == nullptr) {
        throw std::invalid_argument("conv2d_winograd expects non-null input, weight, and output pointers");
    }
    if (batch <= 0 || in_channels <= 0 || out_channels <= 0 || height <= 0 || width <= 0) {
        throw std::invalid_argument("conv2d_winograd expects positive batch/channel/spatial dimensions");
    }

    ConvParams params{batch, in_channels, out_channels, height, width,
                      3, 3, 1, 1, 1, 1, 1, 1};
    conv2d_implicit_gemm<float>(input, weight, output, params, stream);
}

} // namespace hpc::convolution
