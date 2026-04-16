#pragma once

#include <cuda_runtime.h>
#include <cuda/pipeline>
#include "../common/types.cuh"

namespace hpc::cuda13 {

struct TMAConfig {
    int cluster_width = 1;
    int cluster_height = 1;
    int pipeline_depth = 2;
    bool use_tma = true;
};

template <typename T, int NUM_CHANNELS = 8>
void tma_copy_2d(const T* src, T* dst,
                 int rows, int cols,
                 const TMAConfig& config,
                 cudaStream_t stream = nullptr);

template <typename T>
void tma_copy_2d_fallback(const T* src, T* dst,
                          int rows, int cols,
                          cudaStream_t stream = nullptr);

} // namespace hpc::cuda13
