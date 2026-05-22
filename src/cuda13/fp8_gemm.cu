#include <iostream>
#include <cmath>

#include <mma.h>

#include "../common/cuda_check.cuh"
#include "fp8_gemm.cuh"

namespace hpc::cuda13 {

using namespace nvcuda;

namespace {

__global__ void fp8_gemm_naive_kernel(const __half* __restrict__ A, const __half* __restrict__ B,
                                      __half* __restrict__ C, int M, int N, int K, float scale_a,
                                      float scale_b);

__global__ void fp8_gemm_kernel(const __half* __restrict__ A, const __half* __restrict__ B,
                                __half* __restrict__ C, int M, int N, int K, float scale_a,
                                float scale_b);

constexpr int kSupportedTileM = 128;
constexpr int kSupportedTileN = 128;
constexpr int kSupportedTileK = 64;

struct FP8GemmPlan {
    bool use_native_path;
    dim3 block;
    dim3 grid;
};

void validate_fp8_gemm_args(const __half* A, const __half* B, __half* C, int M, int N, int K,
                            const FP8GEMMConfig& config) {
    if (A == nullptr || B == nullptr || C == nullptr) {
        throw std::invalid_argument("fp8_gemm expects non-null A, B, C pointers");
    }
    if (M <= 0 || N <= 0 || K <= 0) {
        throw std::invalid_argument("fp8_gemm expects positive M, N, K");
    }
    if (!std::isfinite(config.scale_a) || !std::isfinite(config.scale_b) || config.scale_a <= 0.0f ||
        config.scale_b <= 0.0f) {
        throw std::invalid_argument("fp8_gemm expects finite positive scale_a and scale_b");
    }
    if (config.tile_m <= 0 || config.tile_n <= 0 || config.tile_k <= 0) {
        throw std::invalid_argument("fp8_gemm expects positive tile_m, tile_n, and tile_k");
    }
    if (config.use_fp8 && (config.tile_m != kSupportedTileM || config.tile_n != kSupportedTileN ||
                           config.tile_k != kSupportedTileK)) {
        throw std::invalid_argument(
            "fp8_gemm expects tile_m/tile_n/tile_k == 128/128/64 when use_fp8 is enabled");
    }
}

FP8GemmPlan build_fp8_gemm_plan(int M, int N, const FP8GEMMConfig& config) {
    return {
        config.use_fp8 && is_hopper_architecture(),
        dim3(256, 1, 1),
        dim3((M + kSupportedTileM - 1) / kSupportedTileM,
             (N + kSupportedTileN - 1) / kSupportedTileN, 1),
    };
}

void launch_fp8_gemm(const __half* A, const __half* B, __half* C, int M, int N, int K,
                     const FP8GemmPlan& plan, const FP8GEMMConfig& config, cudaStream_t stream) {
    fp8_gemm_kernel<<<plan.grid, plan.block, 0, stream>>>(A, B, C, M, N, K, config.scale_a,
                                                           config.scale_b);
}

void launch_fp8_gemm_fallback(const __half* A, const __half* B, __half* C, int M, int N, int K,
                              const FP8GEMMConfig& config, cudaStream_t stream) {
    dim3 block(16, 16);
    dim3 grid((N + block.x - 1) / block.x, (M + block.y - 1) / block.y);

    fp8_gemm_naive_kernel<<<grid, block, 0, stream>>>(A, B, C, M, N, K, config.scale_a,
                                                       config.scale_b);
}

}  // namespace

// Naive GEMM kernel for half precision (fallback)
__global__ void fp8_gemm_naive_kernel(const __half* __restrict__ A, const __half* __restrict__ B,
                                      __half* __restrict__ C, int M, int N, int K, float scale_a,
                                      float scale_b) {
    int row = blockIdx.y * blockDim.y + threadIdx.y;
    int col = blockIdx.x * blockDim.x + threadIdx.x;

    if (row < M && col < N) {
        float sum = 0.0f;
        for (int k = 0; k < K; ++k) {
            float a_val = __half2float(A[row * K + k]) * scale_a;
            float b_val = __half2float(B[k * N + col]) * scale_b;
            sum += a_val * b_val;
        }
        C[row * N + col] = __float2half(sum);
    }
}

__global__ void fp8_gemm_kernel(const __half* __restrict__ A, const __half* __restrict__ B,
                                __half* __restrict__ C, int M, int N, int K, float scale_a,
                                float scale_b) {
    (void)scale_a;  // Used for FP8 scaling in future implementation
    (void)scale_b;

    const int BM = 128;
    const int BN = 128;
    const int BK = 64;

    extern __shared__ __half smem[];
    __half* s_a = smem;
    __half* s_b = smem + BK * BM;

    const int warp_idx = threadIdx.x / 32;
    const int lane_idx = threadIdx.x % 32;

    wmma::fragment<wmma::matrix_a, 16, 16, 16, __half, wmma::row_major> a_frag[4];
    wmma::fragment<wmma::matrix_b, 16, 16, 16, __half, wmma::col_major> b_frag[4];
    wmma::fragment<wmma::accumulator, 16, 16, 16, __half, wmma::row_major> c_frag[4];

    for (int i = 0; i < 4; ++i) {
        wmma::fill_fragment(c_frag[i], (__half)0.0f);
    }

    for (int by = 0; by < K; by += BK) {
        for (int i = 0; i < 4; ++i) {
            int row = by + (blockIdx.x * BM / 16) * 4 + i;
            int col = blockIdx.y * BN + (warp_idx < 2 ? 0 : 32) + ((lane_idx % 8) * 4 + i % 4);

            if (row < M && col < K) {
                s_a[i] = A[row * K + col];
            } else {
                s_a[i] = (__half)0.0f;
            }

            row = by + (i < 2 ? 0 : 32) + (lane_idx / 8);
            col = blockIdx.y * BN + (blockIdx.x * BN / 16) * 4 + i;

            if (row < K && col < N) {
                s_b[i] = B[row * N + col];
            } else {
                s_b[i] = (__half)0.0f;
            }
        }
        __syncthreads();

        for (int i = 0; i < 4; ++i) {
            a_frag[i].fill((__half)0.0f);
            b_frag[i].fill((__half)0.0f);
        }

        wmma::load_matrix_sync(a_frag[0], s_a, 64);
        wmma::load_matrix_sync(b_frag[0], s_b, 64);

        for (int i = 0; i < 4; ++i) {
            wmma::mma_sync(c_frag[i], a_frag[i], b_frag[i], c_frag[i]);
        }

        __syncthreads();
    }

    for (int i = 0; i < 4; ++i) {
        wmma::store_matrix_sync(s_a + i * 256, c_frag[i], 64, wmma::row_major);
    }

    __syncthreads();

    for (int i = 0; i < 4; ++i) {
        int row = blockIdx.x * BM + (warp_idx < 2 ? 0 : 32) + ((lane_idx / 8) * 4 + i % 4);
        int col = blockIdx.y * BN + (i < 2 ? 0 : 32) + (lane_idx % 8) * 4 + i % 4;

        if (row < M && col < N) {
            C[row * N + col] = s_a[i * 256 + ((lane_idx / 8) * 4 + i % 4) * 16 + (lane_idx % 8)];
        }
    }
}

void fp8_gemm(const __half* A, const __half* B, __half* C, int M, int N, int K,
              const FP8GEMMConfig& config, cudaStream_t stream) {
    validate_fp8_gemm_args(A, B, C, M, N, K, config);
    const auto plan = build_fp8_gemm_plan(M, N, config);

    if (plan.use_native_path) {
        launch_fp8_gemm(A, B, C, M, N, K, plan, config, stream);
    } else {
        launch_fp8_gemm_fallback(A, B, C, M, N, K, config, stream);
    }
    CUDA_CHECK_LAST();
}

void fp8_gemm_fallback(const __half* A, const __half* B, __half* C, int M, int N, int K,
                       const FP8GEMMConfig& config, cudaStream_t stream) {
    validate_fp8_gemm_args(A, B, C, M, N, K, config);
    launch_fp8_gemm_fallback(A, B, C, M, N, K, config, stream);
    CUDA_CHECK_LAST();
}

}  // namespace hpc::cuda13
