# Memory Access Optimization Guide

This document provides a detailed introduction to CUDA memory access optimization techniques, including Coalesced Access, Vectorized Load/Store, and Shared Memory usage.

## 1. Coalesced Access

### What is Coalesced Access?

When a Warp (32 threads) accesses consecutive memory addresses, the GPU can merge these accesses into one or a few memory transactions.

### Good Access Pattern

```cpp
// ✓ Coalesced access: adjacent threads access adjacent addresses
__global__ void good_access(float* data, int n) {
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    if (idx < n) {
        data[idx] = data[idx] * 2.0f;  // Thread 0 accesses data[0], Thread 1 accesses data[1], ...
    }
}
```

### Bad Access Pattern

```cpp
// ✗ Non-coalesced access: strided access
__global__ void bad_access(float* data, int n, int stride) {
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    if (idx * stride < n) {
        data[idx * stride] = data[idx * stride] * 2.0f;  // Thread 0 accesses data[0], Thread 1 accesses data[stride], ...
    }
}
```

### Memory Transaction Comparison

```
Coalesced access (stride=1):
Thread 0  → data[0]   ┐
Thread 1  → data[1]   │
Thread 2  → data[2]   ├─ 1 transaction of 128B
...                   │
Thread 31 → data[31]  ┘

Non-coalesced access (stride=32):
Thread 0  → data[0]    → 1 transaction of 32B
Thread 1  → data[32]   → 1 transaction of 32B
Thread 2  → data[64]   → 1 transaction of 32B
...
Thread 31 → data[992]  → 1 transaction of 32B
                       = 32 transactions!
```

---

## 2. Vectorized Load/Store

### Why Use Vectorization?

- Reduce instruction count
- Improve memory bandwidth utilization
- Better instruction-level parallelism

### float4 Vectorization Example

```cpp
// Scalar version
__global__ void relu_scalar(const float* input, float* output, size_t n) {
    size_t idx = blockIdx.x * blockDim.x + threadIdx.x;
    if (idx < n) {
        output[idx] = fmaxf(0.0f, input[idx]);
    }
}

// Vectorized version (float4)
__global__ void relu_vectorized(const float* input, float* output, size_t n) {
    size_t idx = blockIdx.x * blockDim.x + threadIdx.x;
    size_t vec_idx = idx * 4;
    
    if (vec_idx + 3 < n) {
        // Load 4 floats at once
        float4 in = reinterpret_cast<const float4*>(input)[idx];
        
        // Process
        float4 out;
        out.x = fmaxf(0.0f, in.x);
        out.y = fmaxf(0.0f, in.y);
        out.z = fmaxf(0.0f, in.z);
        out.w = fmaxf(0.0f, in.w);
        
        // Store 4 floats at once
        reinterpret_cast<float4*>(output)[idx] = out;
    }
}
```

### Performance Comparison

| Version | Instruction Count | Bandwidth Utilization |
|---------|-------------------|-----------------------|
| Scalar | 4× | ~60% |
| float4 | 1× | ~90% |

### Alignment Requirements

```cpp
// float4 requires 16-byte alignment
float* data;
cudaMalloc(&data, n * sizeof(float));  // Default 256-byte alignment, meets requirement

// Check alignment
assert(reinterpret_cast<uintptr_t>(data) % 16 == 0);
```

---

## 3. Grid Stride Loop

### Why Use Grid Stride Loop?

- Handle arbitrarily sized inputs
- Better load balancing
- Reduce kernel launch overhead

### Implementation

```cpp
__global__ void relu_grid_stride(const float* input, float* output, size_t n) {
    size_t idx = blockIdx.x * blockDim.x + threadIdx.x;
    size_t stride = blockDim.x * gridDim.x;
    
    // Each thread processes multiple elements
    for (size_t i = idx; i < n; i += stride) {
        output[i] = fmaxf(0.0f, input[i]);
    }
}

// Launch configuration
int block_size = 256;
int num_blocks = min((n + block_size - 1) / block_size, 1024);  // Limit block count
relu_grid_stride<<<num_blocks, block_size>>>(input, output, n);
```

### Work Distribution Illustration

```
n = 10000, block_size = 256, num_blocks = 4

Thread 0:    processes 0, 1024, 2048, 3072, 4096, 5120, 6144, 7168, 8192, 9216
Thread 1:    processes 1, 1025, 2049, 3073, 4097, 5121, 6145, 7169, 8193, 9217
...
Thread 1023: processes 1023, 2047, 3071, 4095, 5119, 6143, 7167, 8191, 9215, ...
```

---

## 4. Shared Memory Optimization

### Bank Conflict

Shared Memory is divided into 32 Banks, each with a width of 4 bytes.

```
Bank 0:  addr 0,  32,  64,  96, ...
Bank 1:  addr 4,  36,  68, 100, ...
Bank 2:  addr 8,  40,  72, 104, ...
...
Bank 31: addr 124, 156, 188, 220, ...
```

### Bank Conflict Example

```cpp
// ✗ Bank Conflict: all threads access the same Bank
__shared__ float smem[32][32];
float val = smem[threadIdx.x][0];  // All threads access Bank 0

// ✓ No Bank Conflict: each thread accesses a different Bank
float val = smem[0][threadIdx.x];  // Thread i accesses Bank i
```

### Padding to Eliminate Bank Conflict

```cpp
// Bank Conflict in matrix transpose
__shared__ float tile[32][32];  // Column access causes Bank Conflict

// Add Padding
__shared__ float tile[32][32 + 1];  // +1 eliminates Bank Conflict

// Principle:
// Without Padding: tile[0][0], tile[1][0], tile[2][0], ... are all in Bank 0
// With Padding: tile[0][0] is in Bank 0
//               tile[1][0] is in Bank 1 (because each row has 1 extra element)
//               tile[2][0] is in Bank 2
//               ...
```

---

## 5. Matrix Transpose Optimization

### Naive Implementation (Non-coalesced Write)

```cpp
__global__ void transpose_naive(const float* input, float* output, int rows, int cols) {
    int row = blockIdx.y * blockDim.y + threadIdx.y;
    int col = blockIdx.x * blockDim.x + threadIdx.x;
    
    if (row < rows && col < cols) {
        // Read: Coalesced (row-major)
        // Write: Non-coalesced (column-major)
        output[col * rows + row] = input[row * cols + col];
    }
}
```

### Shared Memory Optimization

```cpp
constexpr int TILE_DIM = 32;

__global__ void transpose_shared(const float* input, float* output, int rows, int cols) {
    __shared__ float tile[TILE_DIM][TILE_DIM + 1];  // +1 to avoid Bank Conflict
    
    int x = blockIdx.x * TILE_DIM + threadIdx.x;
    int y = blockIdx.y * TILE_DIM + threadIdx.y;
    
    // Coalesced read to Shared Memory
    if (x < cols && y < rows) {
        tile[threadIdx.y][threadIdx.x] = input[y * cols + x];
    }
    
    __syncthreads();
    
    // Compute transposed coordinates
    x = blockIdx.y * TILE_DIM + threadIdx.x;
    y = blockIdx.x * TILE_DIM + threadIdx.y;
    
    // Coalesced write to Global Memory
    if (x < rows && y < cols) {
        output[y * rows + x] = tile[threadIdx.x][threadIdx.y];
    }
}
```

### Access Pattern Comparison

```
Naive:
Read: input[row * cols + col]     → Coalesced ✓
Write: output[col * rows + row]   → Non-coalesced ✗

Shared Memory:
Read: input[y * cols + x]         → Coalesced ✓
tile[ty][tx] = ...                → No Bank Conflict (Padding)
Write: output[y * rows + x]       → Coalesced ✓
... = tile[tx][ty]                → No Bank Conflict (Padding)
```

---

## 6. Performance Measurement

### Bandwidth Calculation

```cpp
// Theoretical bandwidth (RTX 4090: 1008 GB/s)
float theoretical_bandwidth = 1008.0f;  // GB/s

// Actual bandwidth
float data_size = n * sizeof(float) * 2;  // Read + Write
float time_ms = timer.elapsed();
float actual_bandwidth = data_size / (time_ms * 1e6);  // GB/s

// Bandwidth utilization
float efficiency = actual_bandwidth / theoretical_bandwidth * 100.0f;
printf("Bandwidth: %.2f GB/s (%.1f%%)\n", actual_bandwidth, efficiency);
```

### Nsight Compute Profiling

```bash
# Analyze memory access patterns
ncu --metrics l1tex__t_sectors_pipe_lsu_mem_global_op_ld.sum,\
              l1tex__t_sectors_pipe_lsu_mem_global_op_st.sum \
    ./your_kernel

# Analyze Shared Memory Bank Conflict
ncu --metrics l1tex__data_bank_conflicts_pipe_lsu_mem_shared_op_ld.sum,\
              l1tex__data_bank_conflicts_pipe_lsu_mem_shared_op_st.sum \
    ./your_kernel
```

---

## 7. Best Practices Summary

| Optimization Technique | Applicable Scenario | Expected Improvement |
|------------------------|---------------------|----------------------|
| Coalesced Access | All Kernels | 2-10× |
| Vectorization (float4) | Elementwise Operations | 1.5-2× |
| Grid Stride Loop | Large Data Volumes | 1.2-1.5× |
| Shared Memory | Data Reuse | 2-5× |
| Padding | Eliminating Bank Conflict | 1.2-1.5× |

## References

- [CUDA C++ Best Practices Guide](https://docs.nvidia.com/cuda/cuda-c-best-practices-guide/)
- [CUDA Memory Model](https://developer.nvidia.com/blog/cuda-refresher-cuda-programming-model/)
