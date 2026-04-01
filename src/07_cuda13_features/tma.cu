#include "tma.cuh"
#include "../common/cuda_check.cuh"
#include <stdexcept>
#include <cooperative_groups.h>

namespace hpc::cuda13 {

bool is_hopper_architecture() {
    int device = 0;
    cudaDeviceProp prop;
    CUDA_CHECK(cudaGetDeviceProperties(&prop, device));
    return prop.major >= 9;
}

namespace cg = cooperative_groups;

template <typename T, int NUM_CHANNELS>
__global__ void tma_copy_kernel(const T* __restrict__ src,
                                 T* __restrict__ dst,
                                 int rows, int cols) {
    extern __shared__ char smem[];
    using TMA_LOAD = cuda::pipeline::async_load_factory<NUM_CHANNELS, T>;
    using TMA_STORE = cuda::pipeline::async_store_factory<NUM_CHANNELS, T>;
    
    __shared__ TMA_LOAD tma_load;
    __shared__ TMA_STORE tma_store;
    
    cg::thread_block tile = cg::this_thread_block();
    cuda::pipeline::prologue consume = cuda::pipeline::make_prologue();
    
    int row = blockIdx.y;
    int col = blockIdx.x * blockDim.x + threadIdx.x;
    
    if (row < rows && col < cols) {
        cuda::memcpy_async(dst + row * cols + col, 
                          src + row * cols + col,
                          sizeof(T) * NUM_CHANNELS, consume);
    }
    
    cuda::pipeline::commit consume;
    consume.wait();
}

template <typename T>
__global__ void async_copy_kernel(const T* __restrict__ src,
                                   T* __restrict__ dst,
                                   int rows, int cols) {
    int row = blockIdx.y;
    int col_start = blockIdx.x * blockDim.x + threadIdx.x;
    
    if (row < rows && col_start < cols) {
        dst[row * cols + col_start] = src[row * cols + col_start];
    }
}

template <>
void tma_copy_2d<float, 8>(const float* src, float* dst,
                           int rows, int cols,
                           const TMAConfig& config,
                           cudaStream_t stream) {
    if (src == nullptr || dst == nullptr) {
        throw std::invalid_argument("tma_copy_2d expects non-null src and dst pointers");
    }
    if (rows <= 0 || cols <= 0) {
        throw std::invalid_argument("tma_copy_2d expects positive rows and cols");
    }

    if (config.use_tma && is_hopper_architecture()) {
        constexpr int NUM_CHANNELS = 8;
        dim3 block(128);
        dim3 grid((cols + NUM_CHANNELS - 1) / NUM_CHANNELS, rows);
        size_t smem_size = sizeof(float) * NUM_CHANNELS * 2;
        
        tma_copy_kernel<float, NUM_CHANNELS><<<grid, block, smem_size, stream>>>(
            src, dst, rows, cols);
    } else {
        tma_copy_2d_fallback(src, dst, rows, cols, stream);
    }
    CUDA_CHECK_LAST();
}

template <>
void tma_copy_2d<float>(const float* src, float* dst,
                        int rows, int cols,
                        const TMAConfig& config,
                        cudaStream_t stream) {
    tma_copy_2d<float, 8>(src, dst, rows, cols, config, stream);
}

template <>
void tma_copy_2d_fallback<float>(const float* src, float* dst,
                                  int rows, int cols,
                                  cudaStream_t stream) {
    dim3 block(256);
    dim3 grid((cols + block.x - 1) / block.x, rows);
    async_copy_kernel<float><<<grid, block, 0, stream>>>(src, dst, rows, cols);
    CUDA_CHECK_LAST();
}

} // namespace hpc::cuda13
