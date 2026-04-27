# Best Practices

Follow these best practices for developing high-performance CUDA kernels.

## Code Organization

### Module Structure

Each kernel module should follow this pattern:

```
src/<module>/
├── CMakeLists.txt      # Build configuration
├── kernel.cu           # Implementation
└── kernel.cuh          # Public interface
```

### Header File Template

```cpp
// kernel.cuh
#pragma once

#include "common/opt_level.cuh"
#include <cuda_runtime.h>
#include <type_traits>

namespace hpc::module {

template <typename T, OptLevel Level = OptLevel::Advanced>
    requires std::is_same_v<T, float> || std::is_same_v<T, __half>
void kernel_name(const T* input, T* output, size_t n,
                 cudaStream_t stream = nullptr);

} // namespace hpc::module
```

### Implementation Template

```cpp
// kernel.cu
#include "<module>/kernel.cuh"
#include "common/cuda_check.cuh"
#include "common/launch.cuh"

namespace hpc::module {

template <typename T, OptLevel Level>
__global__ void kernel_kernel(const T* input, T* output, size_t n) {
    // Implementation
}

template <typename T, OptLevel Level>
void kernel_name(const T* input, T* output, size_t n,
                 cudaStream_t stream) {
    if (n == 0) return;
    
    dim3 block(256);
    dim3 grid((n + 255) / 256);
    
    kernel_kernel<T, Level><<<grid, block, 0, stream>>>(
        input, output, n);
    
    CUDA_CHECK(cudaGetLastError());
}

// Explicit instantiations
template void kernel_name<float, OptLevel::Basic>(...);
template void kernel_name<float, OptLevel::Advanced>(...);

} // namespace hpc::module
```

## Performance Guidelines

### 1. Memory Access Patterns

**DO:**
```cpp
// Coalesced access - consecutive threads access consecutive memory
int idx = blockIdx.x * blockDim.x + threadIdx.x;
output[idx] = input[idx];
```

**DON'T:**
```cpp
// Uncoalesced access - random access pattern
int idx = (threadIdx.x * 13) % 256;
output[idx] = input[idx];
```

### 2. Shared Memory Usage

**Bank Conflict Avoidance:**
```cpp
// Good: Padding to avoid bank conflicts
__shared__ float tile[32][33]; // 33 instead of 32

// Bad: Causes bank conflicts
__shared__ float tile[32][32];
```

**Optimal Shared Memory Layout:**
```cpp
// 2D tiling with shared memory
__shared__ float As[BLOCK_SIZE][BLOCK_SIZE];
__shared__ float Bs[BLOCK_SIZE][BLOCK_SIZE];

// Load with vectorized access
float4 val = reinterpret_cast<const float4*>(input)[idx];
```

### 3. Occupancy Guidelines

**Target Occupancy:**
- Minimum: 25% (6 warps/SM)
- Good: 50% (12 warps/SM)
- Excellent: 75%+ (18 warps/SM)

**Register Usage:**
```cpp
// Limit registers per thread
__launch_bounds__(256, 4) // max 256 threads, 4 blocks/SM
__global__ void kernel(...) {
    // Implementation
}
```

### 4. Tensor Core Usage (Volta+)

**FP16 GEMM with Tensor Cores:**
```cpp
#if __CUDA_ARCH__ >= 700
// Use WMMA intrinsics
#include <mma.h>
using namespace nvcuda;

wmma::fragment<wmma::matrix_a, 16, 16, 16, half, wmma::row_major> a_frag;
wmma::fragment<wmma::matrix_b, 16, 16, 16, half, wmma::col_major> b_frag;
wmma::fragment<wmma::accumulator, 16, 16, 16, float> c_frag;

wmma::load_matrix_sync(a_frag, a_ptr, ld_a);
wmma::load_matrix_sync(b_frag, b_ptr, ld_b);
wmma::mma_sync(c_frag, a_frag, b_frag, c_frag);
#endif
```

## Testing Best Practices

### Unit Test Structure

```cpp
TEST(ModuleName, KernelName_BasicCase) {
    // Arrange
    const size_t n = 1024;
    std::vector<float> h_input(n, 1.0f);
    std::vector<float> h_output(n);
    
    // Act
    hpc::module::kernel_name(h_input.data(), 
                             h_output.data(), 
                             n);
    
    // Assert
    for (size_t i = 0; i < n; ++i) {
        EXPECT_NEAR(h_output[i], expected[i], 1e-5);
    }
}
```

### Property-Based Testing

```cpp
rapidcheck::property(
    [](size_t n) {
        RC_PRE(n > 0 && n < 1000000);
        
        std::vector<float> input(n);
        std::generate(input.begin(), input.end(), std::rand);
        
        RC_ASSERT_NO_THROW({
            hpc::module::kernel_name(
                input.data(), output.data(), n);
        });
        
        // Verify output properties
        RC_ASSERT(std::all_of(output.begin(), output.end(),
                              [](float v) { return v >= 0; }));
    }
);
```

## Error Handling

### Use CUDA_CHECK Macro

```cpp
#include "common/cuda_check.cuh"

void safe_function() {
    float* d_ptr;
    CUDA_CHECK(cudaMalloc(&d_ptr, size));
    
    kernel<<<blocks, threads>>>(d_ptr);
    CUDA_CHECK(cudaGetLastError());
    CUDA_CHECK(cudaDeviceSynchronize());
    
    CUDA_CHECK(cudaFree(d_ptr));
}
```

### Never Ignore Errors

```cpp
// BAD: Ignoring errors
cudaFree(ptr);

// GOOD: Checking errors
CUDA_CHECK(cudaFree(ptr));
```

## Documentation

### Doxygen Comments

```cpp
/**
 * @brief Perform element-wise ReLU activation
 * 
 * @param input Input tensor (device pointer)
 * @param output Output tensor (device pointer)
 * @param n Number of elements
 * @param stream CUDA stream (optional)
 * 
 * @tparam T Data type (float or __half)
 * @tparam Level Optimization level
 * 
 * @note Output is computed in-place when input == output
 */
template <typename T, OptLevel Level>
void relu(const T* input, T* output, size_t n,
          cudaStream_t stream = nullptr);
```

## Continuous Integration

### Pre-commit Checklist

- [ ] Code formatted with `clang-format`
- [ ] Static analysis passed (`clang-tidy`)
- [ ] Unit tests pass (`ctest`)
- [ ] Property tests pass (`rapidcheck`)
- [ ] No compiler warnings
- [ ] Documentation updated

## Further Reading

- [CUDA C Best Practices Guide](https://docs.nvidia.com/cuda/cuda-c-best-practices-guide/)
- [Performance Tuning Guide](performance-tuning.md)
- [Architecture Guide](../api/architecture.md)
