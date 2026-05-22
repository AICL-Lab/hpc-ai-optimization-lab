#include <stdexcept>

#include <cooperative_groups/memcpy_async.h>

#include "../common/cuda_check.cuh"
#include "cluster.cuh"

namespace hpc::cuda13 {

namespace cg = cooperative_groups;

namespace {

template <typename T>
__global__ void cluster_reduce_kernel(const T* __restrict__ input, T* __restrict__ output,
                                      size_t n);

template <typename T>
__global__ void cluster_reduce_fallback_kernel(const T* __restrict__ input, T* __restrict__ output,
                                               size_t n);

struct ClusterLaunchPlan {
    bool use_cluster_kernel;
    int block_size;
    int grid_size;
    size_t shared_memory_bytes;
};

bool is_default_cluster_shape(const ClusterConfig& config) {
    return config.cluster_dims.x == 1 && config.cluster_dims.y == 1 && config.cluster_dims.z == 1 &&
           config.grid_dims.x == 1 && config.grid_dims.y == 1 && config.grid_dims.z == 1;
}

void validate_cluster_reduce_args(const float* input, float* output, size_t n,
                                  const ClusterConfig& config) {
    if (input == nullptr || output == nullptr) {
        throw std::invalid_argument("cluster_reduce expects non-null input and output pointers");
    }
    if (n == 0) {
        throw std::invalid_argument("cluster_reduce expects n > 0");
    }
    if (config.block_dims.x == 0) {
        throw std::invalid_argument("cluster_reduce expects config.block_dims.x > 0");
    }
    if (config.cluster_dims.x == 0 || config.grid_dims.x == 0) {
        throw std::invalid_argument(
            "cluster_reduce expects config.cluster_dims.x and config.grid_dims.x > 0");
    }
}

ClusterLaunchPlan build_cluster_launch_plan(size_t n, const ClusterConfig& config) {
    const int block_size = static_cast<int>(config.block_dims.x);
    return {
        config.use_cluster && is_hopper_architecture() && is_default_cluster_shape(config),
        block_size,
        static_cast<int>((n + block_size - 1) / block_size),
        static_cast<size_t>(block_size) * sizeof(float),
    };
}

void reset_cluster_output(float* output, cudaStream_t stream) {
    CUDA_CHECK(cudaMemsetAsync(output, 0, sizeof(float), stream));
}

void launch_cluster_reduce(const float* input, float* output, size_t n,
                           const ClusterLaunchPlan& plan, cudaStream_t stream) {
    cluster_reduce_kernel<float>
        <<<plan.grid_size, plan.block_size, plan.shared_memory_bytes, stream>>>(input, output, n);
}

void launch_cluster_reduce_fallback(const float* input, float* output, size_t n,
                                    const ClusterLaunchPlan& plan, cudaStream_t stream) {
    cluster_reduce_fallback_kernel<float>
        <<<plan.grid_size, plan.block_size, plan.shared_memory_bytes, stream>>>(input, output, n);
}

}  // namespace

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
    validate_cluster_reduce_args(input, output, n, config);
    const auto plan = build_cluster_launch_plan(n, config);
    reset_cluster_output(output, stream);

    if (plan.use_cluster_kernel) {
        launch_cluster_reduce(input, output, n, plan, stream);
    } else {
        launch_cluster_reduce_fallback(input, output, n, plan, stream);
    }
    CUDA_CHECK_LAST();
}

template <>
void cluster_reduce_fallback<float>(const float* input, float* output, size_t n,
                                    const ClusterConfig& config, cudaStream_t stream) {
    validate_cluster_reduce_args(input, output, n, config);
    const auto plan = build_cluster_launch_plan(n, config);
    reset_cluster_output(output, stream);
    launch_cluster_reduce_fallback(input, output, n, plan, stream);
    CUDA_CHECK_LAST();
}

}  // namespace hpc::cuda13
