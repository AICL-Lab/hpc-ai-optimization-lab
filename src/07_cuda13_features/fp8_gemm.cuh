#pragma once

#include <cuda_runtime.h>
#include <cuda_fp8.h>
#include "../common/types.cuh"

namespace hpc::cuda13 {

enum class FP8Format {
    e4m3,
    e5m2
};

struct FP8GEMMConfig {
    int tile_m = 16;
    int tile_n = 16;
    int tile_k = 16;
    FP8Format format_a = FP8Format::e4m3;
    FP8Format format_b = FP8Format::e4m3;
    float scale_a = 1.0f;
    float scale_b = 1.0f;
    bool use_fp8 = true;
};

void fp8_gemm(const __half* A, const __half* B, __half* C,
              int M, int N, int K,
              const FP8GEMMConfig& config,
              cudaStream_t stream = nullptr);

void fp8_gemm_fallback(const __half* A, const __half* B, __half* C,
                       int M, int N, int K,
                       const FP8GEMMConfig& config,
                       cudaStream_t stream = nullptr);

} // namespace hpc::cuda13
