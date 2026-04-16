#include <stdexcept>

#include <cooperative_groups/memcpy_async.h>

#include "../common/cuda_check.cuh"
#include "cluster.cuh"

namespace hpc::cuda13 {

namespace cg = cooperative_groups;

template <typename T>
__global__ void cluster_reduce_kernel(const T* __restrict__ input, T* __restrict__ output,
                                      size_t n) {
    extern __shared__ float smem[];

    cg::cluster_group cluster = cg::this_cluster();
    int cluster_rank = cluster.rank();
    int cluster_size = cluster.size();

    int tid = threadIdx.x;
    int idx = blockIdx.x * blockDim.x + threadIdx.x;

    float val = (idx < n) ? static_cast<float>(input[idx]) : 0.0f;
    smem[tid] = val;

    cluster.sync();

    if (cluster.use_cluster()) {
        for (int s = cluster_size / 2; s > 0; s >>= 1) {
            int peer_rank = (cluster_rank ^ s);
            if (cluster_rank < s) {
                smem[tid] = smem[tid] + smem[tid + s * blockDim.x];
            }
            cluster.sync();
        }

        if (cluster_rank == 0) {
            float block_sum = 0.0f;
            for (int i = 0; i < cluster_size; ++i) {
                block_sum += smem[i * blockDim.x];
            }
            atomicAdd(output, static_cast<T>(block_sum));
        }
    } else {
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
}

template <typename T>
__global__ void cluster_reduce_fallback_kernel(const T* __restrict__ input, T* __restrict__ output,
                                               size_t n) {
    extern __shared__ float smem[];

    int tid = threadIdx.x;
    int idx = blockIdx.x * blockDim.x + threadIdx.x;

    smem[tid] = (idx < n) ? static_cast<float>(input[idx]) : 0.0f;
    __syncthreads();

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
void cluster_reduce<float>(const float* input, float* output, size_t n, const ClusterConfig& config,
                           cudaStream_t stream) {
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

    if (config.use_cluster && is_hopper_architecture()) {
        cluster_reduce_kernel<float>
            <<<grid_size, block_size, smem_size, stream>>>(input, output, n);
    } else {
        cluster_reduce_fallback_kernel<float>
            <<<grid_size, block_size, smem_size, stream>>>(input, output, n);
    }
    CUDA_CHECK_LAST();
}

template <>
void cluster_reduce_fallback<float>(const float* input, float* output, size_t n,
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

    cluster_reduce_fallback_kernel<float>
        <<<grid_size, block_size, smem_size, stream>>>(input, output, n);
    CUDA_CHECK_LAST();
}

}  // namespace hpc::cuda13
