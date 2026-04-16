# HPC-AI-Optimization-Lab Examples

This directory contains runnable examples demonstrating how to use the library.

---

## Available Examples

### CUDA Examples

| Example | Description | Key Concepts |
|---------|-------------|--------------|
| `01_elementwise/relu_example.cu` | ReLU activation benchmark | Memory bandwidth, vectorization |
| `03_gemm/gemm_benchmark.cu` | Complete GEMM optimization comparison | All 7 optimization steps, cuBLAS comparison |

### Python Examples

| Example | Description |
|---------|-------------|
| `python/basic_usage.py` | Basic Python binding usage with PyTorch tensors |

---

## Building Examples

```bash
# Configure with examples enabled
cmake -S . -B build -DBUILD_EXAMPLES=ON

# Build all examples
cmake --build build

# Build specific examples
cmake --build build --target relu_example gemm_benchmark
```

---

## Running Examples

### CUDA Examples

```bash
# ReLU example - demonstrates memory bandwidth optimization
./build/examples/relu_example

# Expected output:
# === ReLU Example ===
# Array size: 16777216 elements (64.00 MB)
# 
# Kernel configurations:
#   Naive:     grid=65536, block=256
#   Vectorized: grid=16384, block=256
# 
# Verifying results...
# Results MATCH ✓
# 
# === Performance Results ===
# Naive ReLU:     0.1234 ms
# Optimized ReLU: 0.0876 ms
# Speedup:        1.41x
# 
# Memory Bandwidth:
#   Naive:     1038.45 GB/s
#   Optimized: 1462.84 GB/s

# GEMM benchmark - compares all optimization levels
./build/examples/gemm_benchmark

# Expected output:
# Device: NVIDIA GeForce RTX 4090
# Compute Capability: 8.9
# Peak FP32 TFLOPS: 82.58
# 
# ========================================
# GEMM Benchmark: M=1024, N=1024, K=1024
# ========================================
# 
# Implementation              Time (ms)      TFLOPS    vs cuBLAS
# ---------------            ---------      ------    ---------
# cuBLAS                          0.042       50.3       100.0%
# Step 1: Naive                   2.145        1.0         2.0%
# Step 2: SharedMem Tiling        0.534        4.0         7.9%
# Step 3: Double Buffer           0.312        6.9        13.5%
# Step 4: Register Tiling         0.187       11.5        22.8%
# Step 5: Software Pipeline       0.089       24.1        47.9%
```

---

## Example Details

### ReLU Example (`01_elementwise/relu_example.cu`)

Demonstrates:
- Naive vs vectorized kernel performance
- Grid stride loop implementation
- Memory bandwidth calculation
- Result verification

```cpp
// Key code patterns shown:

// 1. Vectorized load/store with float4
float4 in = reinterpret_cast<const float4*>(input)[idx];
float4 out;
out.x = fmaxf(0.0f, in.x);
out.y = fmaxf(0.0f, in.y);
out.z = fmaxf(0.0f, in.z);
out.w = fmaxf(0.0f, in.w);
reinterpret_cast<float4*>(output)[idx] = out;

// 2. Grid stride loop
for (size_t i = idx; i < n; i += blockDim.x * gridDim.x) {
    output[i] = fmaxf(0.0f, input[i]);
}

// 3. Bandwidth calculation
float bandwidth = (2.0f * bytes) / (time_ms * 1e6);  // GB/s
```

### GEMM Benchmark (`03_gemm/gemm_benchmark.cu`)

Demonstrates:
- All 7 GEMM optimization levels
- Comparison with cuBLAS baseline
- TFLOPS calculation
- Roofline analysis

```cpp
// Key optimization concepts shown:

// Step 2: Shared Memory Tiling
__shared__ float As[TILE_SIZE][TILE_SIZE];
__shared__ float Bs[TILE_SIZE][TILE_SIZE];

// Cooperative loading
As[threadIdx.y][threadIdx.x] = A[row * K + a_col];
Bs[threadIdx.y][threadIdx.x] = B[b_row * N + col];

// Step 5: Tensor Core WMMA
wmma::fragment<wmma::matrix_a, 16, 16, 16, half, wmma::row_major> a_frag;
wmma::load_matrix_sync(a_frag, A + row * K + k, K);
wmma::mma_sync(c_frag, a_frag, b_frag, c_frag);
```

### Python Example (`python/basic_usage.py`)

```python
import torch
import hpc_ai_opt

# Create input tensors on GPU
x = torch.randn(1024, 1024, device="cuda", dtype=torch.float32)
y = torch.empty_like(x)

# Apply ReLU using optimized CUDA kernel
hpc_ai_opt.elementwise.relu(x, y)

# Verify result
expected = torch.relu(x)
assert torch.allclose(y, expected)

print(f"Input range: [{x.min():.4f}, {x.max():.4f}]")
print(f"Output range: [{y.min():.4f}, {y.max():.4f}]")
```

---

## Extending Examples

When adding new examples, follow this structure:

```
examples/
├── module_name/
│   └── example_name.cu
└── CMakeLists.txt  # Add target
```

### Example Template

```cpp
/**
 * @file example_name.cu
 * @brief Brief description of what this example demonstrates
 * 
 * This example shows:
 * - Optimization technique 1
 * - Optimization technique 2
 */

#include <cuda_runtime.h>
#include <cstdio>
#include <vector>

#include "module/kernel.cuh"
#include "common/cuda_check.cuh"
#include "common/tensor.cuh"
#include "common/timer.cuh"

int main() {
    printf("=== Example Name ===\n");
    
    // Configuration
    const size_t N = 1024 * 1024;
    
    // Setup
    std::vector<float> h_input(N, 1.0f);
    hpc::Tensor<float> d_input(N);
    hpc::Tensor<float> d_output(N);
    d_input.copy_from_host(h_input);
    
    // Benchmark
    hpc::CudaTimer timer;
    timer.start();
    kernel(d_input.data(), d_output.data(), N);
    timer.stop();
    
    printf("Time: %.4f ms\n", timer.elapsed_ms());
    
    // Verify
    auto result = d_output.to_host();
    // Add verification logic
    
    return 0;
}
```

---

## Performance Analysis

### Using Nsight Compute

```bash
# Profile a specific kernel
ncu --set full -o profile ./build/examples/relu_example

# Analyze memory throughput
ncu --metrics gpu__dram_throughput.avg.pct_of_peak, \
              l1tex__t_sectors_pipe_lsu_mem_global_op_ld.sum \
    ./build/examples/relu_example

# Roofline analysis
ncu --set roofline -o roofline ./build/examples/gemm_benchmark
```

### Using Nsight Systems

```bash
# Timeline analysis
nsys profile -o timeline ./build/examples/gemm_benchmark
nsys-ui timeline.nsys-rep
```

---

## Notes

- All examples require a CUDA-capable GPU
- Examples use RAII (`hpc::Tensor`) for memory management
- Performance numbers vary by GPU architecture
- Run multiple iterations for stable measurements
