#include <stdexcept>

#include <cooperative_groups.h>
#include <cooperative_groups/memcpy_async.h>

#include "../common/cuda_check.cuh"
#include "tma.cuh"

namespace hpc::cuda13 {

namespace cg = cooperative_groups;

namespace {

template <typename T, int NUM_CHANNELS>
__global__ void tma_copy_kernel(const T* __restrict__ src, T* __restrict__ dst, int rows, int cols);

template <typename T>
__global__ void async_copy_kernel(const T* __restrict__ src, T* __restrict__ dst, int rows,
                                  int cols);

struct TMACopyPlan {
    bool use_native_path;
    dim3 block;
    dim3 grid;
    size_t shared_memory_bytes;
};

void validate_tma_copy_args(const float* src, const float* dst, int rows, int cols,
                            const TMAConfig& config) {
    if (src == nullptr || dst == nullptr) {
        throw std::invalid_argument("tma_copy_2d expects non-null src and dst pointers");
    }
    if (rows <= 0 || cols <= 0) {
        throw std::invalid_argument("tma_copy_2d expects positive rows and cols");
    }
    if (config.cluster_width <= 0 || config.cluster_height <= 0 || config.pipeline_depth <= 0) {
        throw std::invalid_argument(
            "tma_copy_2d expects positive cluster_width, cluster_height, and pipeline_depth");
    }
}

TMACopyPlan build_tma_copy_plan(int rows, int cols, const TMAConfig& config) {
    constexpr int kNumChannels = 8;
    const bool use_native_path =
        config.use_tma && is_hopper_architecture() && config.cluster_width == 1 &&
        config.cluster_height == 1 && config.pipeline_depth == 2;
    return {
        use_native_path,
        dim3(128),
        dim3((cols + kNumChannels - 1) / kNumChannels, rows),
        sizeof(float) * kNumChannels * 2,
    };
}

void launch_tma_copy(const float* src, float* dst, int rows, int cols, const TMACopyPlan& plan,
                     cudaStream_t stream) {
    tma_copy_kernel<float, 8>
        <<<plan.grid, plan.block, plan.shared_memory_bytes, stream>>>(src, dst, rows, cols);
}

void launch_tma_copy_fallback(const float* src, float* dst, int rows, int cols,
                              cudaStream_t stream) {
    dim3 block(256);
    dim3 grid((cols + block.x - 1) / block.x, rows);
    async_copy_kernel<float><<<grid, block, 0, stream>>>(src, dst, rows, cols);
}

}  // namespace

// Simplified async copy kernel using cuda::memcpy_async (available in CUDA 11+)
template <typename T, int NUM_CHANNELS>
__global__ void tma_copy_kernel(const T* __restrict__ src, T* __restrict__ dst, int rows,
                                int cols) {
    int row = blockIdx.y;
    int col_start = blockIdx.x * blockDim.x + threadIdx.x;

    if (row < rows) {
        cg::thread_block block = cg::this_thread_block();

        // Use cooperative groups memcpy_async for efficient async copy
        for (int col = col_start; col < cols; col += blockDim.x * gridDim.x) {
            if (col + NUM_CHANNELS <= cols) {
                cg::memcpy_async(block, dst + row * cols + col, src + row * cols + col,
                                 sizeof(T) * NUM_CHANNELS);
            } else {
                // Handle remaining elements
                for (int i = col; i < cols; ++i) {
                    dst[row * cols + i] = src[row * cols + i];
                }
            }
        }
        cg::wait(block);
    }
}

template <typename T>
__global__ void async_copy_kernel(const T* __restrict__ src, T* __restrict__ dst, int rows,
                                  int cols) {
    int row = blockIdx.y;
    int col_start = blockIdx.x * blockDim.x + threadIdx.x;

    if (row < rows && col_start < cols) {
        dst[row * cols + col_start] = src[row * cols + col_start];
    }
}

template <>
void tma_copy_2d<float, 8>(const float* src, float* dst, int rows, int cols,
                           const TMAConfig& config, cudaStream_t stream) {
    validate_tma_copy_args(src, dst, rows, cols, config);
    const auto plan = build_tma_copy_plan(rows, cols, config);

    if (plan.use_native_path) {
        launch_tma_copy(src, dst, rows, cols, plan, stream);
    } else {
        launch_tma_copy_fallback(src, dst, rows, cols, stream);
    }
    CUDA_CHECK_LAST();
}

template <>
void tma_copy_2d<float>(const float* src, float* dst, int rows, int cols, const TMAConfig& config,
                        cudaStream_t stream) {
    tma_copy_2d<float, 8>(src, dst, rows, cols, config, stream);
}

template <>
void tma_copy_2d_fallback<float>(const float* src, float* dst, int rows, int cols,
                                 cudaStream_t stream) {
    validate_tma_copy_args(src, dst, rows, cols, TMAConfig{});
    launch_tma_copy_fallback(src, dst, rows, cols, stream);
    CUDA_CHECK_LAST();
}

}  // namespace hpc::cuda13
