# CUDA 13 & Hopper Architecture Features

This document provides an overview of CUDA / Hopper related concepts and background knowledge.
Note: The `src/07_cuda13_features/` directory in this repository currently consists mainly of experimental examples and compatibility fallbacks. Thus, the hardware capabilities described in this document should not be directly assumed to be fully implemented in the current code.

## 1. Hopper Architecture Overview

### Key Improvements

| Feature | Ampere (A100) | Hopper (H100) | Improvement |
|---------|---------------|---------------|-------------|
| FP16 Tensor Core | 312 TFLOPS | 989 TFLOPS | 3.2× |
| FP8 Tensor Core | - | 1979 TFLOPS | New |
| HBM Bandwidth | 2 TB/s | 3.35 TB/s | 1.7× |
| L2 Cache | 40 MB | 50 MB | 1.25× |
| Shared Memory | 164 KB/SM | 228 KB/SM | 1.4× |

### New Features List

1. **TMA (Tensor Memory Accelerator / 张量内存加速器)**: Asynchronous data transfer
2. **Thread Block Clusters / 线程块集群**: Inter-block cooperation
3. **Distributed Shared Memory / 分布式共享内存**: Cross-block shared memory access
4. **FP8 / 8位浮点数 (Data Types)**: e4m3 and e5m2 formats
5. **Asynchronous Transaction Barrier / 异步事务屏障**: Asynchronous synchronization primitives

---

## 2. TMA (Tensor Memory Accelerator / 张量内存加速器)

### What is TMA?

TMA is a hardware unit introduced in the Hopper Architecture / Hopper架构, specifically designed for efficient multi-dimensional data transfer.

**Advantages**:
- Frees up SM compute resources
- Automatically handles boundary conditions
- Supports multi-dimensional Tensor layouts

### TMA Usage Example

```cpp
#include <cuda/barrier>
#include <cuda/pipeline>

// Create TMA descriptor
__host__ void create_tma_descriptor(
    CUtensorMap* desc,
    void* global_addr,
    uint64_t dims[5],
    uint64_t strides[5],
    uint32_t box_dims[5]) {
    
    cuTensorMapEncodeTiled(
        desc,
        CU_TENSOR_MAP_DATA_TYPE_FLOAT32,
        4,  // number of dimensions
        global_addr,
        dims,
        strides,
        box_dims,
        CU_TENSOR_MAP_INTERLEAVE_NONE,
        CU_TENSOR_MAP_SWIZZLE_NONE,
        CU_TENSOR_MAP_L2_PROMOTION_NONE,
        CU_TENSOR_MAP_FLOAT_OOB_FILL_NONE
    );
}

// Using TMA in a kernel
__global__ void tma_copy_kernel(
    const __grid_constant__ CUtensorMap tensor_map,
    float* smem,
    int x, int y) {
    
    // Asynchronous load to Shared Memory
    if (threadIdx.x == 0) {
        cuda::memcpy_async(
            smem,
            tensor_map,
            cuda::aligned_size_t<16>(sizeof(float) * TILE_SIZE * TILE_SIZE),
            cuda::pipeline<cuda::thread_scope_block>{}
        );
    }
    
    // Wait for completion
    __syncthreads();
}
```

### TMA vs Traditional Load

```
Traditional Load:
1. Each thread computes the address
2. Each thread initiates a load request
3. Coalesced access optimization
4. Threads wait for data

TMA Load:
1. A single thread initiates a TMA request
2. TMA hardware handles the transfer automatically
3. Other threads can continue computation
4. Synchronization through barrier
```

---

## 3. Thread Block Clusters / 线程块集群

### What is a Cluster?

A Cluster is a collection of multiple Thread Blocks that can cooperatively access each other's Shared Memory.

```
Traditional Model:
Block 0 ←→ Shared Memory 0
Block 1 ←→ Shared Memory 1
Block 2 ←→ Shared Memory 2
(No direct communication between Blocks)

Cluster Model:
┌─────────────────────────────────────┐
│           Cluster                   │
│  Block 0 ←→ Block 1 ←→ Block 2     │
│    ↕          ↕          ↕         │
│  SMEM 0 ←→ SMEM 1 ←→ SMEM 2       │
│     (Distributed Shared Memory)     │
└─────────────────────────────────────┘
```

### Cluster Configuration

```cpp
// Compile-time configuration
__global__ __cluster_dims__(2, 2, 1)
void cluster_kernel(...) {
    // Cluster size: 2×2×1 = 4 Blocks
}

// Runtime configuration
cudaLaunchConfig_t config;
config.gridDim = {num_blocks_x, num_blocks_y, 1};
config.blockDim = {256, 1, 1};

cudaLaunchAttribute attrs[1];
attrs[0].id = cudaLaunchAttributeClusterDimension;
attrs[0].val.clusterDim = {2, 2, 1};

config.attrs = attrs;
config.numAttrs = 1;

cudaLaunchKernelEx(&config, cluster_kernel, args...);
```

### Distributed Shared Memory / 分布式共享内存

```cpp
__global__ __cluster_dims__(4, 1, 1)
void distributed_smem_kernel(float* output) {
    __shared__ float smem[256];
    
    // Get Cluster information
    cluster_group cluster = this_cluster();
    int cluster_rank = cluster.block_rank();
    int cluster_size = cluster.num_blocks();
    
    // Initialize local Shared Memory
    smem[threadIdx.x] = cluster_rank * 1000 + threadIdx.x;
    cluster.sync();
    
    // Access Shared Memory from other Blocks
    int target_rank = (cluster_rank + 1) % cluster_size;
    float* remote_smem = cluster.map_shared_rank(smem, target_rank);
    
    float val = remote_smem[threadIdx.x];  // Read remote SMEM
    
    output[blockIdx.x * blockDim.x + threadIdx.x] = val;
}
```

### Cluster Reduction

```cpp
__global__ __cluster_dims__(4, 1, 1)
void cluster_reduce_kernel(const float* input, float* output, int n) {
    __shared__ float smem[256];
    
    cluster_group cluster = this_cluster();
    
    // Each Block computes its local sum
    float local_sum = 0.0f;
    for (int i = blockIdx.x * blockDim.x + threadIdx.x; i < n; i += gridDim.x * blockDim.x) {
        local_sum += input[i];
    }
    
    // Intra-block reduction
    local_sum = block_reduce_sum(local_sum);
    
    if (threadIdx.x == 0) {
        smem[0] = local_sum;
    }
    cluster.sync();
    
    // Inter-cluster reduction (using Distributed Shared Memory)
    if (threadIdx.x == 0 && cluster.block_rank() == 0) {
        float cluster_sum = 0.0f;
        for (int i = 0; i < cluster.num_blocks(); ++i) {
            float* remote_smem = cluster.map_shared_rank(smem, i);
            cluster_sum += remote_smem[0];
        }
        output[blockIdx.x / cluster.num_blocks()] = cluster_sum;
    }
}
```

---

## 4. FP8 / 8位浮点数 (Data Types)

### FP8 Formats

| Format | Exponent Bits | Mantissa Bits | Range | Precision | Purpose |
|--------|---------------|---------------|-------|-----------|---------|
| E4M3 | 4 | 3 | ±240 | Higher | Weights, Activations |
| E5M2 | 5 | 2 | ±57344 | Lower | Gradients |

### FP8 GEMM

```cpp
#include <cuda_fp8.h>

__global__ void fp8_gemm_kernel(
    const __nv_fp8_e4m3* A,
    const __nv_fp8_e4m3* B,
    float* C,
    int M, int N, int K) {
    
    // Matrix multiplication using Tensor Cores with FP8
    // Hopper Tensor Cores have native FP8 support
    
    // Declare Fragments
    wmma::fragment<wmma::matrix_a, 16, 16, 32, __nv_fp8_e4m3, wmma::row_major> a_frag;
    wmma::fragment<wmma::matrix_b, 16, 16, 32, __nv_fp8_e4m3, wmma::col_major> b_frag;
    wmma::fragment<wmma::accumulator, 16, 16, 32, float> c_frag;
    
    wmma::fill_fragment(c_frag, 0.0f);
    
    for (int k = 0; k < K; k += 32) {
        wmma::load_matrix_sync(a_frag, A + row * K + k, K);
        wmma::load_matrix_sync(b_frag, B + k * N + col, N);
        wmma::mma_sync(c_frag, a_frag, b_frag, c_frag);
    }
    
    wmma::store_matrix_sync(C + row * N + col, c_frag, N, wmma::mem_row_major);
}
```

### FP8 Quantization / Dequantization

```cpp
// FP32 → FP8 quantization
__device__ __nv_fp8_e4m3 quantize_fp8(float val, float scale) {
    return __nv_fp8_e4m3(val / scale);
}

// FP8 → FP32 dequantization
__device__ float dequantize_fp8(__nv_fp8_e4m3 val, float scale) {
    return float(val) * scale;
}

// Dynamic scaling
__global__ void compute_scale_kernel(const float* input, float* scale, int n) {
    // Find the maximum absolute value
    float max_abs = 0.0f;
    for (int i = threadIdx.x; i < n; i += blockDim.x) {
        max_abs = fmaxf(max_abs, fabsf(input[i]));
    }
    max_abs = block_reduce_max(max_abs);
    
    if (threadIdx.x == 0) {
        // Max value for E4M3 is approximately 240
        *scale = max_abs / 240.0f;
    }
}
```

---

## 5. Asynchronous Transaction Barrier / 异步事务屏障

### What is a Transaction Barrier?

A Transaction Barrier is a new type of synchronization primitive that can wait for a specific number of asynchronous operations to complete.

```cpp
#include <cuda/barrier>

__global__ void async_barrier_kernel(float* data) {
    __shared__ cuda::barrier<cuda::thread_scope_block> barrier;
    __shared__ float smem[256];
    
    if (threadIdx.x == 0) {
        init(&barrier, blockDim.x);
    }
    __syncthreads();
    
    // Asynchronous load
    cuda::memcpy_async(
        smem + threadIdx.x,
        data + blockIdx.x * blockDim.x + threadIdx.x,
        sizeof(float),
        barrier
    );
    
    // Wait for all asynchronous operations to complete
    barrier.arrive_and_wait();
    
    // Now it is safe to use smem
    float val = smem[threadIdx.x];
}
```

### Multi-stage Pipeline

```cpp
__global__ void pipelined_kernel(const float* input, float* output, int n) {
    constexpr int STAGES = 3;
    __shared__ float smem[STAGES][256];
    __shared__ cuda::barrier<cuda::thread_scope_block> barriers[STAGES];
    
    // Initialize barriers
    if (threadIdx.x == 0) {
        for (int i = 0; i < STAGES; ++i) {
            init(&barriers[i], blockDim.x);
        }
    }
    __syncthreads();
    
    // Prologue: Fill the pipeline
    for (int stage = 0; stage < STAGES - 1; ++stage) {
        int idx = stage * blockDim.x + threadIdx.x;
        if (idx < n) {
            cuda::memcpy_async(smem[stage] + threadIdx.x, input + idx, sizeof(float), barriers[stage]);
        }
    }
    
    // Main loop
    for (int i = 0; i < n / blockDim.x; ++i) {
        int compute_stage = i % STAGES;
        int load_stage = (i + STAGES - 1) % STAGES;
        
        // Wait for current stage data to be ready
        barriers[compute_stage].arrive_and_wait();
        
        // Compute
        float val = smem[compute_stage][threadIdx.x] * 2.0f;
        
        // Asynchronously load next batch of data
        int next_idx = (i + STAGES - 1) * blockDim.x + threadIdx.x;
        if (next_idx < n) {
            cuda::memcpy_async(smem[load_stage] + threadIdx.x, input + next_idx, sizeof(float), barriers[load_stage]);
        }
        
        // Write back the result
        output[i * blockDim.x + threadIdx.x] = val;
    }
}
```

---

## 6. Performance Optimization Recommendations

### When to use TMA

- Large contiguous data transfers
- Multi-dimensional Tensor access
- Need to free SM compute resources

### When to use Thread Block Clusters / 线程块集群

- Inter-block communication is required
- Reduction operations span multiple blocks
- Larger Shared Memory is needed

### When to use FP8 / 8位浮点数

- Inference scenarios
- Model accuracy allows
- Maximum throughput is required

---

## 7. Compatibility Considerations

```cpp
// Check for Hopper feature support
int device;
cudaGetDevice(&device);

cudaDeviceProp prop;
cudaGetDeviceProperties(&prop, device);

// Compute Capability 9.0+ is required for Hopper features
if (prop.major >= 9) {
    // TMA, Cluster, and FP8 can be used
}

// Check for Cluster support
int cluster_support;
cudaDeviceGetAttribute(&cluster_support, cudaDevAttrClusterLaunch, device);
```

---

## 8. References

- [CUDA C++ Programming Guide - Hopper](https://docs.nvidia.com/cuda/cuda-c-programming-guide/index.html#hopper-architecture)
- [NVIDIA H100 Whitepaper](https://resources.nvidia.com/en-us-tensor-core)
- [CUTLASS 3.0 - Hopper Support](https://github.com/NVIDIA/cutlass/blob/main/media/docs/hopper.md)
