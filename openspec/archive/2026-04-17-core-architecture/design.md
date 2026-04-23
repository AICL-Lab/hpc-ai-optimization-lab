# RFC 0001: HPC-AI-Optimization-Lab Architecture

> **Status**: Implemented
> **Last Updated**: 2026-04-17
> **Type**: Architecture Design Document
> **Related Spec**: [Product Spec](../product/hpc-ai-optimization-lab.md)

## 1. System Architecture

### 1.1 High-Level Architecture

```
+-----------------------------------------------------------------------------+
|                        Application Layer                                     |
|  +--------------+  +--------------+  +----------------------------------+  |
|  |  Examples    |  |  Benchmarks  |  |  Python Scripts                  |  |
|  +--------------+  +--------------+  +----------------------------------+  |
+-----------------------------------------------------------------------------+
                                    |
                                    v
+-----------------------------------------------------------------------------+
|                        Python Binding Layer                                  |
|  +----------------------------------------------------------------------+  |
|  |  hpc_ai_opt (Nanobind Module)                                         |  |
|  |  +-- elementwise (relu, sigmoid, transpose)                           |  |
|  |  +-- reduction (softmax, layer_norm, rms_norm)                        |  |
|  |  +-- gemm (matmul)                                                    |  |
|  +----------------------------------------------------------------------+  |
+-----------------------------------------------------------------------------+
                                    |
                                    v
+-----------------------------------------------------------------------------+
|                        Kernel Layer                                          |
|  +----------+ +----------+ +----------+ +----------+                        |
|  | 01_EW    | | 02_Red   | | 03_GEMM  | | 04_Conv  |                        |
|  | ReLU     | | Softmax  | | Step 1-7 | | Implicit |                        |
|  | Sigmoid  | | LayerNorm| | TensorCore| | Winograd |                        |
|  | Transpose| | RMSNorm  | |          | |          |                        |
|  +----------+ +----------+ +----------+ +----------+                        |
|  +----------+ +----------+ +----------+                                     |
|  | 05_Att   | | 06_Quant | | 07_Cuda13|                                     |
|  | FlashAttn| | INT8/FP8 | | TMA/Clust|                                     |
|  | RoPE     | | Dequant  | | FP8 GEMM |                                     |
|  | TopK     | |          | |          |                                     |
|  +----------+ +----------+ +----------+                                     |
+-----------------------------------------------------------------------------+
                                    |
                                    v
+-----------------------------------------------------------------------------+
|                        Common Infrastructure                                 |
|  +----------+ +----------+ +----------+ +----------+                        |
|  | Tensor<T>| | CudaTimer| | CudaCheck| | Types    |                        |
|  | RAII     | | Events   | | Macros   | | Concepts |                        |
|  +----------+ +----------+ +----------+ +----------+                        |
|  +----------+ +----------+                                                    |
|  | Reduce   | | Launch   |                                                    |
|  | Primitives| | Utilities|                                                    |
|  +----------+ +----------+                                                    |
+-----------------------------------------------------------------------------+
                                    |
                                    v
+-----------------------------------------------------------------------------+
|                        Build & Runtime                                       |
|  +----------+ +----------+ +----------+ +----------+                        |
|  | CMake    | | FetchContent| | CUDA   | | Docker   |                        |
|  | 3.24+    | | Dependencies| | 12.4+  | | Dev Env  |                        |
|  +----------+ +----------+ +----------+ +----------+                        |
+-----------------------------------------------------------------------------+
```

### 1.2 Dependency Graph

```
                    +--------------+
                    |  hpc_common  |
                    |  (INTERFACE) |
                    +------+-------+
                           |
         +-----------------+-----------------+
         |                 |                 |
         v                 v                 v
+---------------+  +---------------+  +---------------+
|hpc_element    |  |hpc_reduction  |  |  hpc_gemm     |
+---------------+  +---------------+  +---------------+
         |                 |                 |
         +-----------------+-----------------+
                           |
         +-----------------+-----------------+
         |                 |                 |
         v                 v                 v
+---------------+  +---------------+  +---------------+
|hpc_conv       |  |hpc_attention  |  |hpc_quant      |
+---------------+  +---------------+  +---------------+
                           |
                           v
                    +---------------+
                    | hpc_cuda13    |
                    +---------------+
```

---

## 2. Core Design Patterns

### 2.1 Module Interface Pattern

Each kernel module follows a unified interface design:

```cpp
// kernel_name.cuh - Public Interface
#pragma once
#include <cuda_runtime.h>
#include <concepts>

namespace hpc::module {

// Optimization level enumeration
enum class OptLevel {
    Naive,        // Baseline implementation
    Intermediate, // Basic optimizations
    Advanced      // Production-ready
};

// Template declaration with constraints
template <typename T, OptLevel Level = OptLevel::Advanced>
    requires std::is_same_v<T, float> || std::is_same_v<T, __half>
void kernel_name(const T* input, T* output, size_t n,
                 cudaStream_t stream = nullptr);

} // namespace hpc::module
```

### 2.2 Implementation Pattern

```cpp
// kernel_name.cu - Implementation
#include "kernel_name.cuh"
#include "../common/cuda_check.cuh"

namespace hpc::module {

namespace {  // Private implementations

template <typename T>
__global__ void kernel_naive(const T* __restrict__ input,
                             T* __restrict__ output, size_t n) {
    size_t idx = blockIdx.x * blockDim.x + threadIdx.x;
    if (idx < n) {
        output[idx] = compute(input[idx]);
    }
}

template <typename T>
__global__ void kernel_optimized(const T* __restrict__ input,
                                  T* __restrict__ output, size_t n) {
    // Optimized implementation with vectorization, etc.
}

} // anonymous namespace

// Explicit template specializations
template <>
void kernel_name<float, OptLevel::Naive>(const float* input, float* output,
                                          size_t n, cudaStream_t stream) {
    int block = 256;
    int grid = (n + block - 1) / block;
    kernel_naive<float><<<grid, block, 0, stream>>>(input, output, n);
    CUDA_CHECK_LAST();
}

template <>
void kernel_name<float, OptLevel::Advanced>(const float* input, float* output,
                                             size_t n, cudaStream_t stream) {
    // Optimized launch configuration
    kernel_optimized<float><<<grid, block, 0, stream>>>(input, output, n);
    CUDA_CHECK_LAST();
}

} // namespace hpc::module
```

### 2.3 RAII Resource Management

```cpp
template <CudaNumeric T>
class Tensor {
public:
    explicit Tensor(size_t size) : size_(size), data_(nullptr) {
        CUDA_CHECK(cudaMalloc(&data_, size * sizeof(T)));
    }

    ~Tensor() {
        if (data_) {
            cudaFree(data_);
        }
    }

    // Move semantics
    Tensor(Tensor&& other) noexcept
        : size_(other.size_), data_(other.data_) {
        other.data_ = nullptr;
        other.size_ = 0;
    }

    Tensor& operator=(Tensor&& other) noexcept {
        if (this != &other) {
            if (data_) cudaFree(data_);
            data_ = other.data_;
            size_ = other.size_;
            other.data_ = nullptr;
            other.size_ = 0;
        }
        return *this;
    }

    // Delete copy
    Tensor(const Tensor&) = delete;
    Tensor& operator=(const Tensor&) = delete;

    // Accessors
    T* data() noexcept { return data_; }
    const T* data() const noexcept { return data_; }
    size_t size() const noexcept { return size_; }

    // Host transfers
    void copy_from_host(const T* host_data);
    std::vector<T> to_host() const;

private:
    size_t size_;
    T* data_;
};
```

---

## 3. Memory Hierarchy Strategy

### 3.1 Memory Types and Usage

| Memory Type | Latency | Bandwidth | Capacity | Use Case |
|-------------|---------|-----------|----------|----------|
| Registers | 1 cycle | N/A | ~256KB/SM | Thread-local accumulators |
| Shared Memory | ~5 cycles | ~20 TB/s | ~100KB/SM | Block-level data reuse |
| L1 Cache | ~30 cycles | ~10 TB/s | ~128KB/SM | Auto-managed |
| L2 Cache | ~200 cycles | ~3 TB/s | ~6MB | Auto-managed |
| HBM | ~500 cycles | ~1-3 TB/s | 16-80GB | Large datasets |

### 3.2 Optimization Strategy by Memory Type

```
+-------------------------------------------------------------+
|                      Optimization Priority                   |
+-------------------------------------------------------------+
|  1. Register Tiling    ->  Minimize data movement            |
|  2. Shared Memory      ->  Maximize data reuse               |
|  3. Coalesced Access   ->  Maximize memory bandwidth         |
|  4. Vectorization      ->  Reduce instruction count          |
|  5. Cache Optimization ->  L1/L2 hit rate                    |
+-------------------------------------------------------------+
```

### 3.3 Shared Memory Bank Conflict Avoidance

```cpp
// Bad: Bank conflicts with 32-bit access
__shared__ float tile[32][32];  // Conflict when accessing tile[x][y]

// Good: Padding to avoid bank conflicts
__shared__ float tile[32][33];  // Extra column prevents conflicts

// Best: Use wider types when possible
__shared__ float4 tile[8][32];  // 128-bit access, fewer banks touched
```

---

## 4. GEMM Optimization Deep Dive

### 4.1 Optimization Journey

| Step | Technique | Key Code Pattern | Performance Gain |
|------|-----------|------------------|------------------|
| 1 | Naive | `C[row*N+col] += A[row*K+k] * B[k*N+col]` | Baseline |
| 2 | Shared Mem | `__shared__ float As[TILE][TILE]` | 4x |
| 3 | Double Buffer | Two shared memory buffers | 1.75x |
| 4 | Register Tiling | Accumulate in registers | 1.7x |
| 5 | Tensor Core WMMA | `wmma::mma_sync()` | 8x |
| 6 | MMA PTX | Inline PTX instructions | 1.2x |
| 7 | Software Pipeline | Multi-stage overlap | 1.15x |

### 4.2 Tiling Strategy

```
GEMM: C(M,N) = A(M,K) x B(K,N)

Tiling dimensions:
  BLOCK_M = 128  // Threads per block in M dimension
  BLOCK_N = 128  // Threads per block in N dimension
  BLOCK_K = 32   // K dimension per iteration

Warp tiling:
  WARP_M = 64    // Elements per warp in M
  WARP_N = 64    // Elements per warp in N

Thread tiling:
  THREAD_M = 8   // Elements per thread in M
  THREAD_N = 8   // Elements per thread in N
```

### 4.3 Tensor Core Usage

```cpp
#include <mma.h>
using namespace nvcuda::wmma;

// Tensor Core GEMM tile
fragment<matrix_a, 16, 16, 16, half, row_major> a_frag;
fragment<matrix_b, 16, 16, 16, half, col_major> b_frag;
fragment<accumulator, 16, 16, 16, float> c_frag;

// Load tiles
load_matrix_sync(a_frag, A + row * K + k, K);
load_matrix_sync(b_frag, B + k * N + col, N);
fill_fragment(c_frag, 0.0f);

// Matrix multiply-accumulate
mma_sync(c_frag, a_frag, b_frag, c_frag);

// Store result
store_matrix_sync(C + row * N + col, c_frag, N, mem_row_major);
```

---

## 5. Performance Analysis Framework

### 5.1 Metrics

| Metric | Formula | Target |
|--------|---------|--------|
| TFLOPS | `2 * M * N * K / (time_ms * 1e9)` | GPU peak * 70% |
| Bandwidth | `bytes_transferred / (time_ms * 1e6)` | HBM bandwidth * 80% |
| Occupancy | `active_warps / max_warps` | 50%+ (memory-bound) |
| Efficiency | `achieved / theoretical` | 70%+ |

### 5.2 Roofline Model

```
AI (Arithmetic Intensity) = FLOPS / Bytes

Memory-bound region:  AI < AI_peak
  Performance = Bandwidth * AI

Compute-bound region: AI > AI_peak
  Performance = Peak TFLOPS

AI_peak = Peak TFLOPS / Peak Bandwidth
```

### 5.3 Profiling Commands

```bash
# Nsight Compute - Detailed kernel analysis
ncu --set full -o profile ./your_app

# Roofline analysis
ncu --set roofline -o roofline ./your_app

# Nsight Systems - Timeline analysis
nsys profile -o timeline ./your_app
nsys-ui timeline.nsys-rep

# Memory throughput analysis
ncu --metrics gpu__dram_throughput.avg.pct_of_peak ./your_app
```

---

## 6. Python Binding Design

### 6.1 Zero-Copy Architecture

```
+--------------+      +--------------+
| PyTorch      |      | CUDA Memory  |
| Tensor       |<---->| (GPU)        |
| (CUDA)       |      |              |
+--------------+      +--------------+
       |                     |
       | data_ptr()          |
       v                     v
+----------------------------------+
| Nanobind Binding                 |
| nb::tensor<T, nb::device::cuda>  |
| (Zero-copy, pointer only)        |
+----------------------------------+
       |
       | Direct pointer pass
       v
+----------------------------------+
| CUDA Kernel                      |
| hpc::module::kernel(ptr, ...)    |
+----------------------------------+
```

### 6.2 Binding Implementation

```cpp
void relu_wrapper(nb::tensor<float, nb::device::cuda>& input,
                  nb::tensor<float, nb::device::cuda>& output) {
    // Validation
    if (input.size() != output.size()) {
        throw std::invalid_argument("Size mismatch");
    }

    // Zero-copy direct pointer pass
    hpc::elementwise::relu<float, hpc::elementwise::OptLevel::GridStride>(
        input.data(), output.data(), input.size(), nullptr);
}

NB_MODULE(hpc_ai_opt, m) {
    m.doc() = "HPC-AI-Optimization-Lab CUDA Kernels";

    auto elementwise = m.def_submodule("elementwise");
    elementwise.def("relu", &relu_wrapper, "ReLU activation");
}
```

---

## 7. Build System Design

### 7.1 CMake Structure

```cmake
cmake_minimum_required(VERSION 3.24)
project(HPC-AI-Optimization-Lab LANGUAGES CXX CUDA)

# Standards
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CUDA_STANDARD 20)

# GPU Architecture Detection
include(FindCUDA/select_compute_arch)
CUDA_DETECT_INSTALLED_GPUS(INSTALLED_GPU_CCS)
set(CMAKE_CUDA_ARCHITECTURES ${INSTALLED_GPU_CCS})

# Dependencies (FetchContent)
include(FetchContent)
FetchContent_Declare(googletest ...)
FetchContent_Declare(nanobind ...)
FetchContent_Declare(fmt ...)
FetchContent_Declare(cutlass ...)
FetchContent_MakeAvailable(googletest nanobind fmt cutlass)

# Common library (interface)
add_library(hpc_common INTERFACE)

# Kernel modules
add_subdirectory(src/elementwise)  # hpc_elementwise
add_subdirectory(src/reduction)    # hpc_reduction
add_subdirectory(src/gemm)         # hpc_gemm
# ...

# Python bindings (optional)
if(BUILD_PYTHON_BINDINGS)
    add_subdirectory(python)
endif()

# Tests
enable_testing()
add_subdirectory(tests)
```

### 7.2 Module CMakeLists.txt Pattern

```cmake
# src/gemm/CMakeLists.txt
add_library(hpc_gemm
    gemm.cu
)

target_include_directories(hpc_gemm PUBLIC
    ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(hpc_gemm PUBLIC
    hpc_common
)
```

---

## 8. Error Handling Strategy

### 8.1 Error Categories

| Category | Handling | Example |
|----------|----------|---------|
| CUDA API | `CUDA_CHECK` macro | `cudaMalloc` failures |
| Kernel Launch | `CUDA_CHECK_LAST()` | Launch configuration errors |
| Input Validation | `std::invalid_argument` | Null pointers, size mismatches |
| Numerical | Tolerance checks | NaN, Inf detection |

### 8.2 Error Handling Code

```cpp
// CUDA API wrapper
#define CUDA_CHECK(call)                                                    \
    do {                                                                    \
        cudaError_t err = call;                                             \
        if (err != cudaSuccess) {                                           \
            throw std::runtime_error(                                       \
                fmt::format("CUDA error at {}:{}: {}",                      \
                           __FILE__, __LINE__, cudaGetErrorString(err)));   \
        }                                                                   \
    } while (0)

// Kernel launch wrapper
#define CUDA_CHECK_LAST() CUDA_CHECK(cudaGetLastError())

// Input validation
if (input == nullptr || output == nullptr) {
    throw std::invalid_argument("Null pointer argument");
}
if (M <= 0 || N <= 0 || K <= 0) {
    throw std::invalid_argument("Dimensions must be positive");
}
```

---

## 9. Extension Guide

### 9.1 Adding a New Kernel

1. **Create module directory**: `src/XX_newmodule/`

2. **Define header interface**: `kernel.cuh`
   ```cpp
   namespace hpc::newmodule {
   template<typename T>
   void kernel(...);
   }
   ```

3. **Implement kernel**: `kernel.cu`
   ```cpp
   template<>
   void kernel<float>(...) { /* specialization */ }
   ```

4. **Add build configuration**: `CMakeLists.txt`
   ```cmake
   add_library(hpc_newmodule kernel.cu)
   target_link_libraries(hpc_newmodule PUBLIC hpc_common)
   ```

5. **Write tests**: `tests/newmodule/test_kernel.cpp`

6. **Add Python binding** (optional): `python/bindings/newmodule.cpp`

7. **Update documentation**: API_REFERENCE.md, README.md
