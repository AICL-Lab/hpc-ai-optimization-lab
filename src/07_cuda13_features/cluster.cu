#include "cluster.cuh"
#include "../common/cuda_check.cuh"
#include <stdexcept>

namespace hpc::cuda13 {

// Experimental fallback for a future thread-block-cluster implementation.
// Today this uses a portable block reduction and does not rely on SM90-only features.

template <typename T>
__global__ void cluster_reduce_kernel(const T* __restrict__ input,
                                       T* __restrict__ output,
                                       size_t n) {
    // Simple reduction without cluster features for compatibility
    extern __shared__ float smem[];
    
    int tid = threadIdx.x;
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    
    smem[tid] = (idx < n) ? static_cast<float>(input[idx]) : 0.0f;
    __syncthreads();
    
    // Block-level reduction
    for (int s = blockDim.x / 2; s > 0; s >>= 1) {
        if (tid < s) {
            smem[tid] += smem[tid + s];
        }
        __syncthreads();
    }
    
    if (tid == 0) {
        atomicAdd(output, static_cast<T>(smem[0]));
    }
}

template <>
void cluster_reduce<float>(const float* input, float* output, size_t n,
                           const ClusterConfig& config, cudaStream_t stream) {
    if (input == nullptr || output == nullptr) {
        throw std::invalid_argument("cluster_reduce expects non-null input and output pointers");
    }
    if (n == 0) {
        throw std::invalid_argument("cluster_reduce expects n > 0");
    }
    if (config.block_dims.x == 0) {
        throw std::invalid_argument("cluster_reduce expects config.block_dims.x > 0");
    }

    int block_size = config.block_dims.x;
    int grid_size = (n + block_size - 1) / block_size;
    size_t smem_size = block_size * sizeof(float);

    CUDA_CHECK(cudaMemsetAsync(output, 0, sizeof(float), stream));

    cluster_reduce_kernel<float><<<grid_size, block_size, smem_size, stream>>>(
        input, output, n);
    CUDA_CHECK_LAST();
}

} // namespace hpc::cuda13
