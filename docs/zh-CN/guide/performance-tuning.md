# 性能调优指南

本指南提供了在不同 GPU 架构上优化内核性能的建议。

---

## 目录

- [架构特定调优](#架构特定调优)
- [内存优化](#内存优化)
- [内核配置](#内核配置)
- [分析方法](#分析方法)

---

## 架构特定调优

### GPU 架构概览

| 架构 | 计算能力 | 关键特性 |
|------|---------|---------|
| Volta | 7.0 | 首代 Tensor Core, 128KB 共享内存 |
| Turing | 7.5 | 增强版 Tensor Core, RT 核心 |
| Ampere | 8.0 | 第三代 Tensor Core, 160KB 共享内存 |
| Hopper | 9.0 | 第四代 Tensor Core, TMA, 集群 |

### 推荐的 Block 大小

```cpp
// Volta/Turing (SM 7.0-7.5)
constexpr int BLOCK_SIZE_V100 = 128;
constexpr int BLOCK_SIZE_T4 = 128;

// Ampere (SM 8.0)
constexpr int BLOCK_SIZE_A100 = 256;  // 更高的占用率
constexpr int BLOCK_SIZE_A10 = 128;

// Hopper (SM 9.0)
constexpr int BLOCK_SIZE_H100 = 256;
```

### 共享内存配置

| GPU | 最大共享内存 | GEMM 推荐 |
|-----|-------------|----------|
| V100 | 96 KB | 每 block 32 KB |
| A100 | 160 KB | 每 block 64 KB |
| H100 | 228 KB | 每 block 128 KB |

```cpp
// 根据架构配置共享内存
__global__ void kernel_with_dynamic_smem(...) {
    extern __shared__ float shared_data[];
    // ...
}

// 使用架构特定大小启动
size_t smem_size = 32 * 1024;  // V100 使用 32 KB
#if __CUDA_ARCH__ >= 800
    smem_size = 64 * 1024;  // A100+ 使用 64 KB
#endif

kernel_with_dynamic_smem<<<grid, block, smem_size, stream>>>(...);
```

---

## 内存优化

### 合并内存访问

```cpp
// ❌ 差：跨步访问模式
for (int i = 0; i < N; i += stride) {
    data[i] = ...;
}

// ✅ 好：合并访问
for (int i = threadIdx.x; i < N; i += blockDim.x) {
    data[i] = ...;
}
```

### 共享内存 Bank 冲突

```cpp
// ❌ 差：32元素跨度的 Bank 冲突
__shared__ float tile[32][32];
float val = tile[threadIdx.x][0];  // 所有线程访问同一个 bank

// ✅ 好：填充避免 Bank 冲突
__shared__ float tile[32][33];  // 额外一列用于填充
float val = tile[threadIdx.x][0];  // 无 Bank 冲突
```

### 向量化加载

```cpp
// ❌ 差：标量加载
float val = data[idx];

// ✅ 好：向量化加载
float4 val4 = reinterpret_cast<float4*>(data)[idx / 4];
```

---

## 内核配置

### GEMM 分块大小

| 矩阵大小 | 推荐分块 | 优化级别 |
|---------|---------|---------|
| 小型 (<512) | 16x16 | Naive 或 SharedMemTiling |
| 中型 (512-2048) | 32x32 | DoubleBuffer |
| 大型 (>2048) | 64x64 | RegisterTiling 或 TensorCore |

```cpp
// 根据问题规模选择优化
template <typename T>
void optimized_gemm(const T* A, const T* B, T* C, int M, int N, int K) {
    if (M >= 2048 && N >= 2048 && K >= 2048) {
        // 大型矩阵使用 Tensor Core
        gemm<T, GemmOpt::TensorCoreWMMA>(A, B, C, M, N, K);
    } else if (M >= 512 || N >= 512) {
        // 中型矩阵使用寄存器分块
        gemm<T, GemmOpt::RegisterTiling>(A, B, C, M, N, K);
    } else {
        // 小型矩阵使用共享内存分块
        gemm<T, GemmOpt::SharedMemTiling>(A, B, C, M, N, K);
    }
}
```

### Softmax Block 大小

```cpp
// 对于不同的序列长度
int block_size = 128;  // seq_len <= 1024 默认值
if (seq_len > 4096) {
    block_size = 512;  // 长序列使用更大的 block
}
```

### FlashAttention 配置

```cpp
// 不同 GPU 推荐的 block 大小
struct FlashAttnTuning {
    int block_m;  // Query block 大小
    int block_n;  // KV block 大小
    
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

## 分析方法

### 使用 Nsight Compute

```bash
# 详细内核分析
ncu --set full -o report ./your_program

# 关注内存吞吐量
ncu --metrics gpu__mem_throughput ./your_program

# 检查 Tensor Core 利用率
ncu --metrics sm__pipe_tensor_op_hmma_cycles_active ./your_program
```

### 需要监控的关键指标

| 指标 | 描述 | 目标值 |
|------|------|--------|
| `sm__cycles_active` | SM 活跃周期 | >90% |
| `gpu__mem_throughput` | 内存带宽利用率 | >80% |
| `l1tex__t_requests_pipe_lsu_mem_global_op_ld` | 全局加载 | 最小化 |
| `sm__pipe_fma_cycles_active` | FP32 计算利用率 | >70% |

### 性能检查清单

- [ ] 内存访问是合并的
- [ ] 避免了共享内存 Bank 冲突
- [ ] 寄存器使用不限制占用率
- [ ] Grid 大小饱和所有 SM
- [ ] 在适用时使用 Tensor Core
- [ ] 对内存受限内核使用双缓冲
- [ ] 利用内核融合机会

---

## 基准测试结果

### GEMM 性能 (A100 40GB)

| 大小 | Naive | SharedMem | DoubleBuffer | RegisterTiling | TensorCore |
|------|-------|-----------|--------------|----------------|------------|
| 512³ | 0.8 TF | 2.1 TF | 3.2 TF | 5.8 TF | 42 TF |
| 1024³ | 0.6 TF | 2.4 TF | 3.8 TF | 6.2 TF | 58 TF |
| 4096³ | 0.5 TF | 2.2 TF | 3.5 TF | 6.0 TF | 62 TF |

### Softmax 性能 (A100 40GB)

| Batch×SeqLen | Naive | WarpShuffle | OnlineSoftmax |
|--------------|-------|-------------|---------------|
| 32×1024 | 180 GB/s | 420 GB/s | 580 GB/s |
| 128×1024 | 220 GB/s | 480 GB/s | 650 GB/s |
| 32×4096 | 150 GB/s | 350 GB/s | 520 GB/s |

---

## 优化优先级

1. **算法选择** (10x-100x 加速)
   - 选择正确的优化级别
   - 尽可能使用 Tensor Core

2. **内存合并** (2x-10x 加速)
   - 确保连续访问模式
   - 使用共享内存进行数据复用

3. **共享内存优化** (1.5x-3x 加速)
   - 避免 Bank 冲突
   - 适当填充数组

4. **占用率调优** (1.2x-2x 加速)
   - 平衡寄存器使用
   - 调整 block 大小

5. **软件流水线** (1.1x-1.5x 加速)
   - 隐藏内存延迟
   - 重叠计算和内存访问

---

更多详情，请参阅：
- [GEMM 优化指南](gemm.md)
- [内存优化指南](memory.md)
- [故障排除指南](troubleshooting.md)
