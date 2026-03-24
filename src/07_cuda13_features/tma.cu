#include "tma.cuh"
#include "../common/cuda_check.cuh"
#include <stdexcept>

namespace hpc::cuda13 {

// Experimental fallback for the future TMA path.
// This currently performs a regular kernel copy so behavior is portable and testable.

template <typename T>
__global__ void async_copy_kernel(const T* __restrict__ src,
                                   T* __restrict__ dst,
                                   int rows, int cols) {
    // Using cuda::memcpy_async for demonstration
    int row = blockIdx.y;
    int col_start = blockIdx.x * blockDim.x + threadIdx.x;
    
    if (row < rows && col_start < cols) {
        dst[row * cols + col_start] = src[row * cols + col_start];
    }
}

template <>
void tma_copy_2d<float>(const float* src, float* dst,
                        int rows, int cols, cudaStream_t stream) {
    if (src == nullptr || dst == nullptr) {
        throw std::invalid_argument("tma_copy_2d expects non-null src and dst pointers");
    }
    if (rows <= 0 || cols <= 0) {
        throw std::invalid_argument("tma_copy_2d expects positive rows and cols");
    }

    dim3 block(256);
    dim3 grid((cols + block.x - 1) / block.x, rows);
    async_copy_kernel<float><<<grid, block, 0, stream>>>(src, dst, rows, cols);
    CUDA_CHECK_LAST();
}

} // namespace hpc::cuda13
