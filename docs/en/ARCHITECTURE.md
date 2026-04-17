# Architecture Overview

This document describes the architecture, design patterns, and module organization of HPC-AI-Optimization-Lab.

---

## Design Philosophy

### Core Principles

1. **Progressive Optimization**: Each module demonstrates optimization techniques from naive to expert-level, allowing learners to understand the "why" behind each optimization.

2. **RAII Resource Management**: All GPU resources (memory, streams, events) are managed through RAII wrappers to prevent leaks and ensure exception safety.

3. **Modern C++**: Leverages C++20 features including concepts, `requires` clauses, and `std::span` where appropriate.

4. **Test-Driven Development**: All kernels include both unit tests (GoogleTest) and property-based tests (RapidCheck).

5. **Zero-Cost Abstractions**: Template-based design allows compile-time optimization selection with no runtime overhead.

---

## Project Architecture

```
hpc-ai-optimization-lab/
├── src/                    # Kernel implementations
│   ├── common/             # Shared utilities (header-only)
│   ├── elementwise/     # Module 1: Pointwise operations
│   ├── reduction/       # Module 2: Reduction operations
│   ├── gemm/            # Module 3: Matrix multiplication
│   ├── convolution/     # Module 4: Convolution operations
│   ├── attention/       # Module 5: Attention mechanisms
│   ├── quantization/    # Module 6: Quantization utilities
│   └── cuda13/ # Module 7: Experimental features
├── tests/                  # Test suites
├── examples/               # Runnable examples
├── python/                 # Python bindings
└── docs/                   # Documentation
```

---

## Common Library (`src/common/`)

The common library provides foundational utilities used across all modules.

### Core Components

| File | Purpose | Key Types/Functions |
|------|---------|---------------------|
| `cuda_check.cuh` | Error handling | `CUDA_CHECK`, `CUDA_CHECK_LAST` |
| `types.cuh` | Type traits | `CudaNumeric` concept, `accumulator_t` |
| `tensor.cuh` | GPU memory wrapper | `Tensor<T>` RAII class |
| `timer.cuh` | Performance timing | `CudaTimer` class |
| `reduce.cuh` | Reduction primitives | `warp_reduce_sum`, `block_reduce_sum` |
| `launch.cuh` | Kernel launch helpers | `LaunchConfig`, grid size calculations |
| `opt_level.cuh` | Optimization selection | `OptLevel` enum |

### Tensor Class Design

```cpp
template <CudaNumeric T>
class Tensor {
public:
    explicit Tensor(size_t size);
    ~Tensor();
    
    // Move semantics (no copies)
    Tensor(Tensor&&) noexcept;
    Tensor& operator=(Tensor&&) noexcept;
    
    // Accessors
    T* data() noexcept;
    size_t size() const noexcept;
    
    // Host-device transfers
    void copy_from_host(const T* data);
    void copy_to_host(T* data) const;
    std::vector<T> to_host() const;
    
    // Async variants
    void copy_from_host_async(const T* data, cudaStream_t stream);
    
    // Operations
    void zero();
    
private:
    size_t size_;
    T* data_;
};
```

### Type System

```cpp
// Concept for CUDA-compatible types
template <typename T>
concept CudaNumeric = std::is_arithmetic_v<T> ||
                      std::is_same_v<T, __half> ||
                      std::is_same_v<T, __nv_bfloat16>;

// Accumulator type selection
template <typename T>
using accumulator_t = typename AccumulatorType<T>::type;
// float -> float, __half -> float, __nv_bfloat16 -> float
```

---

## Module Structure Pattern

Each kernel module follows a consistent structure:

### File Organization

```
src/XX_modulename/
├── CMakeLists.txt      # Build configuration
├── kernel_name.cuh     # Public interface (header)
└── kernel_name.cu      # Implementation (template specializations)
```

### Header Interface Pattern

```cpp
#pragma once

#include <cuda_runtime.h>
#include <concepts>

namespace hpc::module {

// Optimization levels
enum class OptLevel {
    Naive,
    Intermediate,
    Advanced
};

// Template declaration with constraints
template <typename T, OptLevel Level = OptLevel::Advanced>
    requires std::is_same_v<T, float> || std::is_same_v<T, __half>
void kernel_name(const T* input, T* output, size_t n, 
                 cudaStream_t stream = nullptr);

} // namespace hpc::module
```

### Implementation Pattern

```cpp
#include "kernel_name.cuh"
#include "../common/cuda_check.cuh"

namespace hpc::module {

// Private kernel implementations
namespace {

template <typename T>
__global__ void kernel_naive(...) { /* ... */ }

template <typename T>
__global__ void kernel_optimized(...) { /* ... */ }

} // anonymous namespace

// Explicit template specializations
template <>
void kernel_name<float, OptLevel::Naive>(...) {
    kernel_naive<float><<<...>>>(...);
    CUDA_CHECK_LAST();
}

template <>
void kernel_name<float, OptLevel::Advanced>(...) {
    kernel_optimized<float><<<...>>>(...);
    CUDA_CHECK_LAST();
}

} // namespace hpc::module
```

---

## Module Details

### Module 01: Elementwise Operations

**Purpose**: Demonstrate memory access optimization techniques.

| Operation | Optimizations |
|-----------|---------------|
| ReLU | Naive, Vectorized, Grid Stride |
| Sigmoid | Naive, Vectorized, Grid Stride |
| Vector Add | Naive, Vectorized, Grid Stride |
| Transpose | Naive, Shared Memory, Padded |

**Key Learning**: Memory coalescing, vectorization, bank conflict avoidance.

### Module 02: Reduction Operations

**Purpose**: Demonstrate parallel reduction techniques.

| Operation | Optimizations |
|-----------|---------------|
| Softmax | Online algorithm (single-pass) |
| LayerNorm | Welford's algorithm |
| RMSNorm | Simplified normalization |

**Key Learning**: Warp shuffle, block reduction, numerical stability.

### Module 03: GEMM

**Purpose**: Comprehensive matrix multiplication optimization journey.

| Step | Technique | Key Concept |
|------|-----------|-------------|
| 1 | Naive | Baseline implementation |
| 2 | Shared Memory | Data reuse |
| 3 | Double Buffer | Latency hiding |
| 4 | Register Tiling | Reduced shared memory access |
| 5 | WMMA | Tensor Core API |
| 6 | MMA PTX | Low-level Tensor Core |
| 7 | Software Pipeline | Multi-stage overlap |

**Key Learning**: Complete optimization pipeline.

### Module 04: Convolution

**Purpose**: Image convolution implementations.

| Operation | Description |
|-----------|-------------|
| Implicit GEMM | Production-ready, validated |
| Winograd | Experimental, falls back to implicit GEMM |

### Module 05: Attention

**Purpose**: Transformer attention mechanisms.

| Operation | Description |
|-----------|-------------|
| FlashAttention | IO-aware attention with online softmax |
| RoPE | Rotary positional embedding |
| TopK | MoE routing |

### Module 06: Quantization

**Purpose**: Reduced precision operations.

| Operation | Description |
|-----------|-------------|
| INT8 Quantize | Per-row scaling |
| INT8 Dequantize | Restore from quantized |
| FP8 Scaling | Placeholder for Hopper |

### Module 07: CUDA 13 Features

**Purpose**: Experimental Hopper architecture features.

| Feature | Status |
|---------|--------|
| TMA | Async copy fallback |
| Clusters | Block reduction fallback |
| FP8 GEMM | Scaled FP16 demo |

---

## Build System

### CMake Structure

```cmake
# Root CMakeLists.txt
project(HPC-AI-Optimization-Lab)

# Common library (header-only, interface)
add_library(hpc_common INTERFACE)

# Module libraries
function(hpc_add_cuda_library target)
    add_library(${target} ${ARGN})
    target_link_libraries(${target} PUBLIC hpc_common)
    # CUDA-specific options...
endfunction()

# Each module
add_subdirectory(src/elementwise)  # Creates hpc_elementwise
add_subdirectory(src/reduction)    # Creates hpc_reduction
# ...
```

### Dependency Graph

```
hpc_common (interface)
    ├── fmt::fmt
    └── CUTLASS (headers only)
         │
         ▼
┌────────┴────────┬─────────────┬───────────────┐
│                 │             │               │
▼                 ▼             ▼               ▼
hpc_elementwise  hpc_reduction  hpc_gemm  hpc_attention
     │                 │             │           │
     └─────────────────┴─────────────┴───────────┘
                       │
                       ▼
                  hpc_cuda13
```

---

## Testing Architecture

### Test Organization

```
tests/
├── test_utils.hpp         # Shared test utilities
├── CMakeLists.txt         # Test configuration
├── elementwise/           # Module-specific tests
├── reduction/
├── gemm/
├── attention/
├── quantization/
├── convolution/
└── cuda13/
```

### Test Utilities

```cpp
namespace hpc::test {

// Floating-point comparison
template <typename T>
bool almost_equal(T a, T b, T rel_tol = 1e-5, T abs_tol = 1e-6);

// Vector comparison
template <typename T>
bool vectors_almost_equal(const std::vector<T>& a, const std::vector<T>& b);

// Random data generation
template <typename T>
std::vector<T> random_vector(size_t n, T min = -1.0, T max = 1.0);

} // namespace hpc::test
```

### Property-Based Testing

```cpp
// RapidCheck property test
RC_GTEST_PROP(GemmTest, Correctness, ()) {
    auto M = *rc::gen::inRange<int>(1, 64);
    // Auto-generates test cases, finds edge cases
}
```

---

## Python Bindings

### Architecture

```
python/
├── CMakeLists.txt      # Nanobind configuration
├── bindings/
│   └── bindings.cpp    # C++ -> Python bindings
└── benchmark/
    └── benchmark.py    # Performance scripts
```

### Binding Pattern

```cpp
// bindings.cpp
#include <nanobind/nanobind.h>
#include <nanobind/ndarray.h>

namespace nb = nanobind;

NB_MODULE(hpc_ai_opt, m) {
    // Elementwise submodule
    nb::module_ elementwise = m.def_submodule("elementwise");
    
    elementwise.def("relu", [](nb::ndarray<float> input,
                                nb::ndarray<float> output) {
        // Validate shapes, extract pointers, call CUDA kernel
    });
}
```

---

## Performance Considerations

### Memory Hierarchy Usage

| Memory Type | Latency | Bandwidth | Use Case |
|-------------|---------|-----------|----------|
| Registers | 1 cycle | N/A | Thread-local accumulators |
| Shared Memory | ~5 cycles | ~20 TB/s | Block-local data reuse |
| L2 Cache | ~200 cycles | ~3 TB/s | Auto-managed caching |
| HBM | ~500 cycles | ~1-3 TB/s | Large datasets |

### Occupancy Guidelines

| Kernel Type | Target Occupancy | Reason |
|-------------|------------------|--------|
| Memory-bound | 50%+ | Hide memory latency |
| Compute-bound | 25-50% | Saturate FPUs |
| Tensor Core | 12.5-25% | High register usage |

---

## Extending the Library

### Adding a New Kernel

1. **Create module directory**: `src/XX_newmodule/`

2. **Define header interface**: `kernel.cuh`
   ```cpp
   namespace hpc::newmodule {
   template<typename T>
   void kernel(...);
   }
   ```

3. **Implement kernels**: `kernel.cu`
   ```cpp
   template<>
   void kernel<float>(...) { /* specialization */ }
   ```

4. **Add build configuration**: `CMakeLists.txt`
   ```cmake
   hpc_add_cuda_library(hpc_newmodule kernel.cu)
   ```

5. **Write tests**: `tests/newmodule/test_kernel.cpp`

6. **Update root CMakeLists.txt**:
   ```cmake
   add_subdirectory(src/XX_newmodule)
   ```

---

## Best Practices

### Error Handling

```cpp
// Always check CUDA errors
CUDA_CHECK(cudaMalloc(&ptr, size));
kernel<<<grid, block>>>(args);
CUDA_CHECK_LAST();  // Check kernel launch
```

### Resource Management

```cpp
// Prefer RAII over manual management
hpc::Tensor<float> data(n);  // Automatic cleanup

// Avoid raw pointers for owned resources
// float* data;
// cudaMalloc(&data, ...);  // Easy to leak
```

### Stream Management

```cpp
// Use explicit streams for async operations
cudaStream_t stream;
cudaStreamCreate(&stream);

kernel<<<grid, block, 0, stream>>>(args);

cudaStreamSynchronize(stream);
cudaStreamDestroy(stream);
```
