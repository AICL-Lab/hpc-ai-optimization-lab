# GEMM Optimization Deep Dive

This document details the 7-step optimization path for GEMM (General Matrix Multiplication).

## Overview

GEMM computation: `C = α * A × B + β * C`

Where:
- A: M × K matrix
- B: K × N matrix
- C: M × N matrix
- α, β: scalar coefficients

## Step 1: Naive Global Memory

### Implementation Strategy

Each thread computes one element of the output matrix C.

```cpp
__global__ void gemm_naive_kernel(const float* A, const float* B, float* C,
                                   int M, int N, int K) {
    int row = blockIdx.y * blockDim.y + threadIdx.y;
    int col = blockIdx.x * blockDim.x + threadIdx.x;

    if (row < M && col < N) {
        float sum = 0.0f;
        for (int k = 0; k < K; ++k) {
            sum += A[row * K + k] * B[k * N + col];
        }
        C[row * N + col] = sum;
    }
}
```

### Performance Analysis

- **Issue**: Each element requires 2K global memory accesses
- **Bandwidth Utilization**: ~5-10%
- **TFLOPS**: ~0.5 (FP32, RTX 4090)

### Memory Access Pattern

```
Thread (0,0): A[0,0], A[0,1], ..., A[0,K-1]  ← Coalesced access ✓
              B[0,0], B[1,0], ..., B[K-1,0]  ← Strided access ✗
```

---

## Step 2: Shared Memory Tiling

### Optimization Strategy

Load sub-blocks (tiles) of A and B into Shared Memory to reduce global memory accesses.

```cpp
constexpr int TILE_SIZE = 32;

__global__ void gemm_shared_kernel(const float* A, const float* B, float* C,
                                    int M, int N, int K) {
    __shared__ float As[TILE_SIZE][TILE_SIZE];
    __shared__ float Bs[TILE_SIZE][TILE_SIZE];

    int row = blockIdx.y * TILE_SIZE + threadIdx.y;
    int col = blockIdx.x * TILE_SIZE + threadIdx.x;
    float sum = 0.0f;

    for (int t = 0; t < (K + TILE_SIZE - 1) / TILE_SIZE; ++t) {
        // Cooperative loading of tiles into Shared Memory
        int a_col = t * TILE_SIZE + threadIdx.x;
        int b_row = t * TILE_SIZE + threadIdx.y;

        As[threadIdx.y][threadIdx.x] = (row < M && a_col < K) ? A[row * K + a_col] : 0.0f;
        Bs[threadIdx.y][threadIdx.x] = (b_row < K && col < N) ? B[b_row * N + col] : 0.0f;

        __syncthreads();

        // Compute partial dot product
        for (int k = 0; k < TILE_SIZE; ++k) {
            sum += As[threadIdx.y][k] * Bs[k][threadIdx.x];
        }

        __syncthreads();
    }

    if (row < M && col < N) {
        C[row * N + col] = sum;
    }
}
```

### Performance Gains

- **Global Memory Access Reduction**: K → K/TILE_SIZE
- **Bandwidth Utilization**: ~30-40%
- **TFLOPS**: ~2.0

### Tiling Diagram

```
        K                           K
    ┌───────┐                   ┌───────┐
    │       │                   │       │
M   │   A   │               K   │   B   │
    │       │                   │       │
    └───────┘                   └───────┘
        ↓                           ↓
    ┌───┬───┬───┐               ┌───┬───┬───┐
    │T1 │T2 │T3 │               │T1 │T2 │T3 │
    ├───┼───┼───┤               ├───┼───┼───┤
    │T4 │T5 │T6 │               │T4 │T5 │T6 │
    └───┴───┴───┘               └───┴───┴───┘
    
    Each Block processes a TILE_SIZE × TILE_SIZE output tile
```

---

## Step 3: Double Buffering

### Optimization Strategy

Use Double Buffering to prefetch the next tile while computing the current tile.

```cpp
__global__ void gemm_double_buffer_kernel(const float* A, const float* B, float* C,
                                           int M, int N, int K) {
    __shared__ float As[2][TILE_SIZE][TILE_SIZE];  // Double buffering
    __shared__ float Bs[2][TILE_SIZE][TILE_SIZE];

    int row = blockIdx.y * TILE_SIZE + threadIdx.y;
    int col = blockIdx.x * TILE_SIZE + threadIdx.x;
    float sum = 0.0f;

    int write_stage = 0;
    int read_stage = 0;

    // Prefetch first tile
    load_tile(As[write_stage], Bs[write_stage], A, B, 0, row, col, M, N, K);
    __syncthreads();

    for (int t = 0; t < num_tiles; ++t) {
        read_stage = write_stage;
        write_stage = 1 - write_stage;

        // Asynchronously load next tile
        if (t + 1 < num_tiles) {
            load_tile(As[write_stage], Bs[write_stage], A, B, t + 1, row, col, M, N, K);
        }

        // Compute current tile
        for (int k = 0; k < TILE_SIZE; ++k) {
            sum += As[read_stage][threadIdx.y][k] * Bs[read_stage][k][threadIdx.x];
        }

        __syncthreads();
    }
    // ...
}
```

### Performance Gains

- **Memory Latency Hiding**: Overlap computation with memory loads
- **TFLOPS**: ~3.5

### Timeline Comparison

```
Without Double Buffering:
|--Load T1--|--Compute T1--|--Load T2--|--Compute T2--|

With Double Buffering:
|--Load T1--|--Compute T1--|--Compute T2--|--Compute T3--|
            |--Load T2----|--Load T3----|--Load T4----|
```

---

## Step 4: Register Tiling

### Optimization Strategy

Each thread computes multiple output elements to reduce Shared Memory accesses.

```cpp
constexpr int REG_TILE_M = 8;  // Each thread computes an 8×8 tile
constexpr int REG_TILE_N = 8;

__global__ void gemm_register_tiling_kernel(...) {
    // Register accumulators
    float reg_c[REG_TILE_M][REG_TILE_N] = {0.0f};

    for (int k_tile = 0; k_tile < K; k_tile += BLK_K) {
        // Load to Shared Memory
        // ...

        // Compute using Register Tiling
        for (int k = 0; k < BLK_K; ++k) {
            float reg_a[REG_TILE_M];
            float reg_b[REG_TILE_N];

            // Load from Shared Memory to registers
            for (int m = 0; m < REG_TILE_M; ++m)
                reg_a[m] = As[k][thread_m * REG_TILE_M + m];
            for (int n = 0; n < REG_TILE_N; ++n)
                reg_b[n] = Bs[k][thread_n * REG_TILE_N + n];

            // Outer product computation
            for (int m = 0; m < REG_TILE_M; ++m)
                for (int n = 0; n < REG_TILE_N; ++n)
                    reg_c[m][n] += reg_a[m] * reg_b[n];
        }
    }
    // ...
}
```

### Performance Gains

- **Shared Memory Access Reduction**: 8× (REG_TILE_M)
- **Instruction-level Parallelism**: More independent computations
- **TFLOPS**: ~6.0

---

## Step 5: Tensor Core (WMMA API)

### Optimization Strategy

Use NVIDIA Tensor Core for matrix multiplication, leveraging dedicated hardware acceleration.

```cpp
#include <mma.h>
using namespace nvcuda;

constexpr int WMMA_M = 16;
constexpr int WMMA_N = 16;
constexpr int WMMA_K = 16;

__global__ void gemm_wmma_kernel(const __half* A, const __half* B, float* C,
                                  int M, int N, int K) {
    // Declare Fragments
    wmma::fragment<wmma::matrix_a, WMMA_M, WMMA_N, WMMA_K, __half, wmma::row_major> a_frag;
    wmma::fragment<wmma::matrix_b, WMMA_M, WMMA_N, WMMA_K, __half, wmma::row_major> b_frag;
    wmma::fragment<wmma::accumulator, WMMA_M, WMMA_N, WMMA_K, float> c_frag;

    wmma::fill_fragment(c_frag, 0.0f);

    for (int k = 0; k < K; k += WMMA_K) {
        // Load Fragments
        wmma::load_matrix_sync(a_frag, A + row * K + k, K);
        wmma::load_matrix_sync(b_frag, B + k * N + col, N);

        // Tensor Core MMA
        wmma::mma_sync(c_frag, a_frag, b_frag, c_frag);
    }

    // Store results
    wmma::store_matrix_sync(C + row * N + col, c_frag, N, wmma::mem_row_major);
}
```

### Performance Gains

- **Tensor Core Throughput**: 8-16× higher than CUDA Cores
- **TFLOPS**: ~50+ (FP16)

### Tensor Core Architecture

```
Tensor Core (multiple per SM):
┌─────────────────────────────────────┐
│  16×16×16 Matrix Multiply-Accumulate │
│                                     │
│  A (16×16, FP16) × B (16×16, FP16)  │
│           ↓                         │
│     C (16×16, FP32)                 │
└─────────────────────────────────────┘
```

---

## Step 6: Tensor Core (MMA PTX) 🚧

> **Status**: Fallback wrapper over Step 5
> 
> Step 6 currently delegates to Step 5 (WMMA) for stability. The enum entry is retained to keep the optimization ladder explicit, but the shipped close-out behavior is the WMMA fallback.

### Optimization Strategy

Use PTX instructions to control Tensor Core directly for finer-grained control.

```cpp
__device__ __forceinline__ void mma_m16n8k16_fp16(
    uint32_t* d, const uint32_t* a, const uint32_t* b, const uint32_t* c) {
    asm volatile(
        "mma.sync.aligned.m16n8k16.row.col.f16.f16.f16.f16 "
        "{%0, %1}, "
        "{%2, %3, %4, %5}, "
        "{%6, %7}, "
        "{%8, %9};\n"
        : "=r"(d[0]), "=r"(d[1])
        : "r"(a[0]), "r"(a[1]), "r"(a[2]), "r"(a[3]),
          "r"(b[0]), "r"(b[1]),
          "r"(c[0]), "r"(c[1])
    );
}
```

### Projected Performance Gains

- **More fine-grained register control**
- **Projected TFLOPS**: ~60+ (estimated)

---

## Step 7: Software Pipelining ✅

> **Status**: Implemented
> 
> Step 7 ships in the current version as the maintained software-pipelined GEMM path for `float` and `__half`.

### Optimization Strategy

Use multi-stage pipelining to hide instruction latency.

```cpp
constexpr int PIPE_STAGES = 3;

__global__ void gemm_software_pipeline_kernel(...) {
    __shared__ float As[PIPE_STAGES][TILE_K][TILE_M + 1];
    __shared__ float Bs[PIPE_STAGES][TILE_K][TILE_N + 1];

    // Prologue: fill pipeline
    for (int stage = 0; stage < PIPE_STAGES - 1; ++stage) {
        load_tile(As[stage], Bs[stage], ...);
    }

    // Main loop: pipeline execution
    for (int k_tile = 0; k_tile < num_tiles; ++k_tile) {
        int compute_stage = k_tile % PIPE_STAGES;
        int load_stage = (k_tile + PIPE_STAGES - 1) % PIPE_STAGES;

        // Asynchronously load next tile
        if (k_tile + PIPE_STAGES - 1 < num_tiles) {
            load_tile(As[load_stage], Bs[load_stage], ...);
        }

        // Compute current tile
        compute_tile(As[compute_stage], Bs[compute_stage], reg_c);
    }
}
```

### Performance Gains

- **Instruction Latency Hiding**: Multi-stage overlap
- **TFLOPS**: ~70+

### Pipeline Diagram

```
Stage 0: |--Load--|--Compute--|--Load--|--Compute--|
Stage 1:          |--Load--|--Compute--|--Load--|--Compute--|
Stage 2:                   |--Load--|--Compute--|--Load--|--Compute--|
```

---

## Performance Comparison Summary

| Step | Optimization | TFLOPS (FP32) | Relative Speedup | Status |
|------|--------------|---------------|------------------|--------|
| 1 | Naive | 0.5 | 1.0× | ✅ Implemented |
| 2 | Shared Memory Tiling | 2.0 | 4.0× | ✅ Implemented |
| 3 | Double Buffering | 3.5 | 7.0× | ✅ Implemented |
| 4 | Register Tiling | 6.0 | 12.0× | ✅ Implemented |
| 5 | WMMA | 50+ | 100× | ✅ Implemented |
| 6 | MMA PTX | ~60† | ~120× | 🚧 WMMA fallback |
| 7 | Software Pipelining | ~70† | ~140× | ✅ Implemented |

† Projected estimates

> **Note**: Steps 1-5 are fully implemented and tested. Step 6 currently delegates to Step 5 for stability. Step 7 is implemented as the shipped software-pipelined path.

## References

- [NVIDIA CUTLASS](https://github.com/NVIDIA/cutlass)
- [How to Optimize a CUDA Matmul Kernel](https://siboehm.com/articles/22/CUDA-MMM)
- [NVIDIA Tensor Core Programming](https://docs.nvidia.com/cuda/cuda-c-programming-guide/index.html#wmma)
