#pragma once

#include <cuda_runtime.h>

namespace hpc::convolution {

struct ConvParams;
struct WinogradConfig {
    int tile_size = 4;
    bool use_winograd = true;
};

void conv2d_winograd(const float* input, const float* weight, float* output,
                     const ConvParams& params, const WinogradConfig& config = {},
                     cudaStream_t stream = nullptr);

void conv2d_winograd_fallback(const float* input, const float* weight, float* output,
                              const ConvParams& params, cudaStream_t stream = nullptr);

}  // namespace hpc::convolution
