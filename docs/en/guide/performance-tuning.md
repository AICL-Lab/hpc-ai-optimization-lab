# Performance Tuning Guide | 性能调优指南

This guide provides recommendations for optimizing kernel performance on different GPU architectures.

---

## Table of Contents

- [Architecture-Specific Tuning](#architecture-specific-tuning)
- [Memory Optimization](#memory-optimization)
- [Kernel Configuration](#kernel-configuration)
- [Profiling Methods](#profiling-methods)

---

## Architecture-Specific Tuning

### GPU Architecture Overview

| Architecture | Compute Capability | Key Features |
|--------------|-------------------|--------------|
| Volta | 7.0 | First Tensor Core, 128KB shared memory |
| Turing | 7.5 | Enhanced Tensor Core, RT cores |
| Ampere | 8.0 | 3rd gen Tensor Core, 160KB shared memory |
| Hopper | 9.0 | 4th gen Tensor Core, TMA, Clusters |

### Recommended Block Sizes

```cpp
// Volta/Turing (SM 7.0-7.5)
constexpr int BLOCK_SIZE_V100 = 128;
constexpr int BLOCK_SIZE_T4 = 128;

// Ampere (SM 8.0)
constexpr int BLOCK_SIZE_A100 = 256;  // Higher occupancy
constexpr int BLOCK_SIZE_A10 = 128;

// Hopper (SM 9.0)
constexpr int BLOCK_SIZE_H100 = 256;
```

### Shared Memory Configuration

| GPU | Max Shared Memory | Recommended for GEMM |
|-----|------------------|---------------------|
| V100 | 96 KB | 32 KB per block |
| A100 | 160 KB | 64 KB per block |
| H100 | 228 KB | 128 KB per block |

```cpp
// Configure shared memory based on architecture
__global__ void kernel_with_dynamic_smem(...) {
    extern __shared__ float shared_data[];
    // ...
}

// Launch with architecture-specific size
size_t smem_size = 32 * 1024;  // 32 KB for V100
#if __CUDA_ARCH__ >= 800
    smem_size = 64 * 1024;  // 64 KB for A100+
#endif

kernel_with_dynamic_smem<<<grid, block, smem_size, stream>>>(...);
```

---

## Memory Optimization

### Coalesced Memory Access

```cpp
// ❌ Poor: Strided access pattern
for (int i = 0; i < N; i += stride) {
    data[i] = ...;
}

// ✅ Good: Coalesced access
for (int i = threadIdx.x; i < N; i += blockDim.x) {
    data[i] = ...;
}
```

### Shared Memory Bank Conflicts

```cpp
// ❌ Poor: Bank conflicts with 32-element stride
__shared__ float tile[32][32];
float val = tile[threadIdx.x][0];  // All threads access same bank

// ✅ Good: Padding to avoid bank conflicts
__shared__ float tile[32][33];  // Extra column for padding
float val = tile[threadIdx.x][0];  // No bank conflicts
```

### Vectorized Loads

```cpp
// ❌ Poor: Scalar loads
float val = data[idx];

// ✅ Good: Vectorized loads
float4 val4 = reinterpret_cast<float4*>(data)[idx / 4];
```

---

## Kernel Configuration

### GEMM Tile Sizes

| Matrix Size | Recommended Tile | Optimization Level |
|-------------|-----------------|-------------------|
| Small (<512) | 16x16 | Naive or SharedMemTiling |
| Medium (512-2048) | 32x32 | DoubleBuffer |
| Large (>2048) | 64x64 | RegisterTiling or TensorCore |

```cpp
// Select optimization based on problem size
template <typename T>
void optimized_gemm(const T* A, const T* B, T* C, int M, int N, int K) {
    if (M >= 2048 && N >= 2048 && K >= 2048) {
        // Use Tensor Core for large matrices
        gemm<T, GemmOpt::TensorCoreWMMA>(A, B, C, M, N, K);
    } else if (M >= 512 || N >= 512) {
        // Use register tiling for medium matrices
        gemm<T, GemmOpt::RegisterTiling>(A, B, C, M, N, K);
    } else {
        // Use shared memory tiling for small matrices
        gemm<T, GemmOpt::SharedMemTiling>(A, B, C, M, N, K);
    }
}
```

### Softmax Block Size

```cpp
// For different sequence lengths
int block_size = 128;  // Default for seq_len <= 1024
if (seq_len > 4096) {
    block_size = 512;  // Larger blocks for long sequences
}
```

### FlashAttention Configuration

```cpp
// Recommended block sizes for different GPUs
struct FlashAttnTuning {
    int block_m;  // Query block size
    int block_n;  // KV block size
    
    static FlashAttnTuning for_gpu(int compute_capability) {
        if (compute_capability >= 90) {
            return {128, 64};   // H100
        } else if (compute_capability >= 80) {
            return {64, 64};    // A100
        } else {
            return {32, 32};    // V100/T4
        }
    }
};
```

---

## Profiling Methods

### Using Nsight Compute

```bash
# Detailed kernel analysis
ncu --set full -o report ./your_program

# Focus on memory throughput
ncu --metrics gpu__mem_throughput ./your_program

# Check Tensor Core utilization
ncu --metrics sm__pipe_tensor_op_hmma_cycles_active ./your_program
```

### Key Metrics to Monitor

| Metric | Description | Target |
|--------|-------------|--------|
| `sm__cycles_active` | SM active cycles | >90% |
| `gpu__mem_throughput` | Memory bandwidth utilization | >80% |
| `l1tex__t_requests_pipe_lsu_mem_global_op_ld` | Global loads | Minimize |
| `sm__pipe_fma_cycles_active` | FP32 compute utilization | >70% |

### Performance Checklist

- [ ] Memory access is coalesced
- [ ] Shared memory bank conflicts avoided
- [ ] Register usage doesn't limit occupancy
- [ ] Grid size saturates all SMs
- [ ] Tensor Core used where applicable
- [ ] Double buffering for memory-bound kernels
- [ ] Kernel fusion opportunities exploited

---

## Benchmark Results

### GEMM Performance (A100 40GB)

| Size | Naive | SharedMem | DoubleBuffer | RegisterTiling | TensorCore |
|------|-------|-----------|--------------|----------------|------------|
| 512³ | 0.8 TF | 2.1 TF | 3.2 TF | 5.8 TF | 42 TF |
| 1024³ | 0.6 TF | 2.4 TF | 3.8 TF | 6.2 TF | 58 TF |
| 4096³ | 0.5 TF | 2.2 TF | 3.5 TF | 6.0 TF | 62 TF |

### Softmax Performance (A100 40GB)

| Batch×SeqLen | Naive | WarpShuffle | OnlineSoftmax |
|--------------|-------|-------------|---------------|
| 32×1024 | 180 GB/s | 420 GB/s | 580 GB/s |
| 128×1024 | 220 GB/s | 480 GB/s | 650 GB/s |
| 32×4096 | 150 GB/s | 350 GB/s | 520 GB/s |

---

## Optimization Priority

1. **Algorithm Selection** (10x-100x speedup)
   - Choose right optimization level
   - Use Tensor Core when possible

2. **Memory Coalescing** (2x-10x speedup)
   - Ensure contiguous access patterns
   - Use shared memory for data reuse

3. **Shared Memory Optimization** (1.5x-3x speedup)
   - Avoid bank conflicts
   - Pad arrays appropriately

4. **Occupancy Tuning** (1.2x-2x speedup)
   - Balance register usage
   - Adjust block size

5. **Software Pipelining** (1.1x-1.5x speedup)
   - Hide memory latency
   - Overlap compute and memory

---

For more details, see:
- [GEMM Optimization Guide](gemm.md)
- [Memory Optimization Guide](memory.md)
- [Troubleshooting Guide](troubleshooting.md)
