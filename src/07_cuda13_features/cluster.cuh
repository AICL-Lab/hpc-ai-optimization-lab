#pragma once

#include <cuda_runtime.h>

namespace hpc::cuda13 {

struct ClusterConfig {
    dim3 cluster_dims;
    dim3 grid_dims;
    dim3 block_dims;
    bool use_cluster = true;
};

bool is_hopper_architecture();

template <typename T>
void cluster_reduce(const T* input, T* output, size_t n,
                    const ClusterConfig& config,
                    cudaStream_t stream = nullptr);

template <typename T>
void cluster_reduce_fallback(const T* input, T* output, size_t n,
                             const ClusterConfig& config,
                             cudaStream_t stream = nullptr);

} // namespace hpc::cuda13
