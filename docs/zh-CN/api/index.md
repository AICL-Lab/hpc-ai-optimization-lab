# API 参考

本文档提供了 HPC-AI-Optimization-Lab 中所有公共接口的完整 API 文档。

---

## 目录

- [公共库 (Common Library)](#公共库)
- [模块 01: 逐元素运算 (Elementwise)](#模块-01-逐元素运算)
- [模块 02: 归约 (Reduction)](#模块-02-归约)
- [模块 03: GEMM](#模块-03-gemm)
- [模块 04: 卷积 (Convolution)](#模块-04-卷积)
- [模块 05: 注意力 (Attention)](#模块-05-注意力)
- [模块 06: 量化 (Quantization)](#模块-06-量化)
- [模块 07: CUDA 13 特性](#模块-07-cuda-13-特性)
- [Python API](#python-api)

---

## 公共库

### Tensor\<T\>

GPU 内存管理的 RAII 封装类。

**头文件**: `common/tensor.cuh`

```cpp
#include "common/tensor.cuh"

namespace hpc {

template <CudaNumeric T>
class Tensor {
public:
    // 构造函数
    explicit Tensor(size_t size);
    
    // 析构函数（自动清理）
    ~Tensor();
    
    // 移动语义（禁止拷贝）
    Tensor(Tensor&& other) noexcept;
    Tensor& operator=(Tensor&& other) noexcept;
    
    // 访问器
    T* data() noexcept;
    const T* data() const noexcept;
    size_t size() const noexcept;
    size_t bytes() const noexcept;
    bool empty() const noexcept;
    
    // 主机 ↔ 设备 数据传输
    void copy_from_host(const T* host_data);
    void copy_to_host(T* host_data) const;
    void copy_from_host(const std::vector<T>& host_vec);
    std::vector<T> to_host() const;
    
    // 异步传输
    void copy_from_host_async(const T* host_data, cudaStream_t stream);
    void copy_to_host_async(T* host_data, cudaStream_t stream) const;
    
    // 操作
    void zero();
};

} // namespace hpc
```

**示例**:

```cpp
#include "common/tensor.cuh"

int main() {
    // 创建包含 1024 个 float 的张量
    hpc::Tensor<float> data(1024);
    
    // 使用主机数据初始化
    std::vector<float> host_data(1024, 1.0f);
    data.copy_from_host(host_data);
    
    // 在内核中使用
    my_kernel<<<1, 256>>>(data.data(), data.size());
    cudaDeviceSynchronize();
    
    // 获取结果
    auto result = data.to_host();
    
    return 0;
}  // 自动清理
```

---

### CudaTimer

GPU 端性能计时器。

**头文件**: `common/timer.cuh`

```cpp
#include "common/timer.cuh"

namespace hpc {

class CudaTimer {
public:
    CudaTimer();
    ~CudaTimer();
    
    // 不可拷贝，可移动
    CudaTimer(CudaTimer&&) noexcept;
    CudaTimer& operator=(CudaTimer&&) noexcept;
    
    // 计时操作
    void start(cudaStream_t stream = nullptr);
    void stop(cudaStream_t stream = nullptr);
    
    // 结果获取
    float elapsed_ms() const;
};

} // namespace hpc
```

**示例**:

```cpp
hpc::CudaTimer timer;
hpc::Tensor<float> data(1024 * 1024);

timer.start();
data.zero();
cudaDeviceSynchronize();
timer.stop();

std::cout << "Time: " << timer.elapsed_ms() << " ms\n";
```

---

### CUDA_CHECK 宏

CUDA 操作的错误处理宏。

**头文件**: `common/cuda_check.cuh`

```cpp
#include "common/cuda_check.cuh"

// 检查返回值
CUDA_CHECK(cudaMalloc(&ptr, size));

// 检查上一次内核启动
kernel<<<grid, block>>>(args);
CUDA_CHECK_LAST();
```

---

## 模块 01: 逐元素运算

### ReLU

整流线性单元（Rectified Linear Unit）激活函数。

**头文件**: `elementwise/relu.cuh`

```cpp
#include "elementwise/relu.cuh"

namespace hpc::elementwise {

enum class OptLevel {
    Naive,       // 基础逐元素处理
    Vectorized,  // float4 加载/存储
    GridStride   // Grid stride 循环（推荐）
};

template <typename T, OptLevel Level = OptLevel::GridStride>
    requires std::is_same_v<T, float> || std::is_same_v<T, __half>
void relu(const T* input, T* output, size_t n, 
          cudaStream_t stream = nullptr);

} // namespace hpc::elementwise
```

**示例**:

```cpp
#include "elementwise/relu.cuh"
#include "common/tensor.cuh"

// 使用默认优化级别（GridStride）
hpc::Tensor<float> input(1024);
hpc::Tensor<float> output(1024);

hpc::elementwise::relu<float>(input.data(), output.data(), 1024);

// 使用特定优化级别
hpc::elementwise::relu<float, hpc::elementwise::OptLevel::Vectorized>(
    input.data(), output.data(), 1024);
```

---

### Sigmoid

Sigmoid 激活函数。

**头文件**: `elementwise/sigmoid.cuh`

```cpp
#include "elementwise/sigmoid.cuh"

namespace hpc::elementwise {

template <typename T, OptLevel Level = OptLevel::GridStride>
    requires std::is_same_v<T, float> || std::is_same_v<T, __half>
void sigmoid(const T* input, T* output, size_t n,
             cudaStream_t stream = nullptr);

} // namespace hpc::elementwise
```

---

### Vector Add

逐元素向量加法。

**头文件**: `elementwise/vector_add.cuh`

```cpp
#include "elementwise/vector_add.cuh"

namespace hpc::elementwise {

template <typename T, OptLevel Level = OptLevel::GridStride>
    requires std::is_same_v<T, float> || std::is_same_v<T, __half>
void vector_add(const T* a, const T* b, T* c, size_t n,
                cudaStream_t stream = nullptr);

} // namespace hpc::elementwise
```

---

### Transpose

矩阵转置，支持多种优化选项。

**头文件**: `elementwise/transpose.cuh`

```cpp
#include "elementwise/transpose.cuh"

namespace hpc::elementwise {

enum class TransposeOpt {
    Naive,           // 直接按行读、按列写
    SharedMemory,    // 使用共享内存
    SharedMemPadded  // 使用带填充的共享内存（推荐）
};

template <typename T, TransposeOpt Opt = TransposeOpt::SharedMemPadded>
    requires std::is_same_v<T, float> || std::is_same_v<T, __half>
void transpose(const T* input, T* output, int rows, int cols,
               cudaStream_t stream = nullptr);

} // namespace hpc::elementwise
```

---

## 模块 02: 归约

### Softmax

数值稳定的 Softmax，采用在线算法实现。

**头文件**: `reduction/softmax.cuh`

```cpp
#include "reduction/softmax.cuh"

namespace hpc::reduction {

enum class SoftmaxOpt {
    Naive,          // 两遍扫描，使用全局原子操作
    WarpShuffle,    // Warp 级归约
    OnlineSoftmax,  // 单遍扫描（推荐）
    Fused           // 融合 L2 缓存持久化
};

template <typename T, SoftmaxOpt Opt = SoftmaxOpt::OnlineSoftmax>
    requires std::is_same_v<T, float> || std::is_same_v<T, __half>
void softmax(const T* input, T* output, int batch, int seq_len,
             cudaStream_t stream = nullptr);

} // namespace hpc::reduction
```

**示例**:

```cpp
#include "reduction/softmax.cuh"

int batch = 32, seq_len = 128;
hpc::Tensor<float> input(batch * seq_len);
hpc::Tensor<float> output(batch * seq_len);

hpc::reduction::softmax<float>(
    input.data(), output.data(), batch, seq_len);
```

---

### LayerNorm

层归一化（Layer Normalization）。

**头文件**: `reduction/layernorm.cuh`

```cpp
#include "reduction/layernorm.cuh"

namespace hpc::reduction {

template <typename T>
    requires std::is_same_v<T, float> || std::is_same_v<T, __half>
void layer_norm(const T* input, const T* gamma, const T* beta,
                T* output, int batch, int hidden_size,
                float eps = 1e-5f, cudaStream_t stream = nullptr);

} // namespace hpc::reduction
```

---

### RMSNorm

均方根层归一化（Root Mean Square Layer Normalization）。

**头文件**: `reduction/rmsnorm.cuh`

```cpp
#include "reduction/rmsnorm.cuh"

namespace hpc::reduction {

template <typename T>
    requires std::is_same_v<T, float> || std::is_same_v<T, __half>
void rms_norm(const T* input, const T* gamma, T* output,
              int batch, int hidden_size,
              float eps = 1e-5f, cudaStream_t stream = nullptr);

} // namespace hpc::reduction
```

---

## 模块 03: GEMM

### 通用矩阵乘法 (General Matrix Multiply)

7 步优化进阶实现。

**头文件**: `gemm/gemm.cuh`

```cpp
#include "gemm/gemm.cuh"

namespace hpc::gemm {

enum class GemmOpt {
    Naive,            // 步骤 1: 仅使用全局内存
    SharedMemTiling,  // 步骤 2: 共享内存分块
    DoubleBuffer,     // 步骤 3: 双缓冲
    RegisterTiling,   // 步骤 4: 寄存器分块
    TensorCoreWMMA,   // 步骤 5: WMMA API ✅
    TensorCoreMMA,    // 步骤 6: MMA PTX（委托给步骤 5）🚧
    SoftwarePipeline  // 步骤 7: 软件流水线（计划中）🚧
};

template <typename T, GemmOpt Opt = GemmOpt::SharedMemTiling>
    requires std::is_same_v<T, float> || 
             std::is_same_v<T, __half> || 
             std::is_same_v<T, int8_t>
void gemm(const T* A, const T* B, T* C,
          int M, int N, int K,
          float alpha = 1.0f, float beta = 0.0f,
          cudaStream_t stream = nullptr);

// CUTLASS 基准测试（仅 float，实验性）
template <typename T>
void gemm_cutlass(const T* A, const T* B, T* C,
                  int M, int N, int K,
                  float alpha = 1.0f, float beta = 0.0f,
                  cudaStream_t stream = nullptr);

} // namespace hpc::gemm
```

**类型支持矩阵**:

| 类型 | 步骤 1-4 | 步骤 5 (WMMA) | 步骤 6 (MMA) | 步骤 7 (流水线) | CUTLASS |
|------|----------|---------------|--------------|-----------------|---------|
| `float` | ✅ | ✅ | ✅† | 🚧 | ✅ |
| `__half` | ✅ | ✅ | ✅† | 🚧 | 🚧 |
| `int8_t` | ✅‡ | ✅ | ✅† | 🚧 | 🚧 |

† 步骤 6 当前为稳定性考虑委托给步骤 5  
‡ INT8 GEMM 当前仅 SharedMemTiling 优化完全实现；其他优化级别委托给 SharedMemTiling

**示例**:

```cpp
#include "gemm/gemm.cuh"

int M = 1024, N = 1024, K = 1024;

hpc::Tensor<float> A(M * K);
hpc::Tensor<float> B(K * N);
hpc::Tensor<float> C(M * N);

// 基础共享内存分块
hpc::gemm::gemm<float, hpc::gemm::GemmOpt::SharedMemTiling>(
    A.data(), B.data(), C.data(), M, N, K);

// Tensor Core（需要 __half）
hpc::Tensor<__half> A_h(M * K);
hpc::Tensor<__half> B_h(K * N);
hpc::Tensor<__half> C_h(M * N);

hpc::gemm::gemm<__half, hpc::gemm::GemmOpt::TensorCoreWMMA>(
    A_h.data(), B_h.data(), C_h.data(), M, N, K);

// CUTLASS 基准测试（仅 float）
hpc::gemm::gemm_cutlass<float>(
    A.data(), B.data(), C.data(), M, N, K);
```

---

## 模块 04: 卷积

### 隐式 GEMM 卷积 (Implicit GEMM Convolution)

生产就绪的卷积实现。

**头文件**: `convolution/conv_implicit_gemm.cuh`

```cpp
#include "convolution/conv_implicit_gemm.cuh"

namespace hpc::convolution {

struct ConvParams {
    int batch;
    int in_channels;
    int out_channels;
    int in_height;
    int in_width;
    int kernel_h;
    int kernel_w;
    int stride_h;
    int stride_w;
    int pad_h;
    int pad_w;
    int dilation_h;
    int dilation_w;
};

template <typename T>
void conv2d_implicit_gemm(const T* input, const T* weight, T* output,
                          const ConvParams& params,
                          cudaStream_t stream = nullptr);

} // namespace hpc::convolution
```

---

### Winograd 卷积

优化的 3×3 卷积（实验性）。

**头文件**: `convolution/conv_winograd.cuh`

```cpp
#include "convolution/conv_winograd.cuh"

namespace hpc::convolution {

struct WinogradConfig {
    int tile_size = 4;
    bool use_winograd = true;
};

void conv2d_winograd(const float* input, const float* weight, float* output,
                     const ConvParams& params,
                     const WinogradConfig& config = {},
                     cudaStream_t stream = nullptr);

} // namespace hpc::convolution
```

---

## 模块 05: 注意力

### FlashAttention 前向传播

IO 感知的注意力机制实现。

**头文件**: `attention/flash_attention.cuh`

```cpp
#include "attention/flash_attention.cuh"

namespace hpc::attention {

struct FlashAttnConfig {
    int batch_size;
    int num_heads;
    int seq_len;
    int head_dim;    // 当前仅支持 64
    float scale;     // 通常为 1/sqrt(head_dim)
    bool causal;     // 是否应用因果掩码
};

template <typename T>
    requires std::is_same_v<T, float> || std::is_same_v<T, __half>
void flash_attention_forward(const T* Q, const T* K, const T* V,
                             T* O,
                             const FlashAttnConfig& config,
                             cudaStream_t stream = nullptr);

} // namespace hpc::attention
```

当前已交付实现仅特化 `float`，并要求 `head_dim == 64`。

**示例**:

```cpp
#include "attention/flash_attention.cuh"

int batch = 2, heads = 8, seq = 512, dim = 64;
int total = batch * heads * seq * dim;

hpc::Tensor<float> Q(total), K(total), V(total), O(total);

hpc::attention::FlashAttnConfig config{
    .batch_size = batch,
    .num_heads = heads,
    .seq_len = seq,
    .head_dim = dim,
    .scale = 1.0f / std::sqrt((float)dim),
    .causal = true
};

hpc::attention::flash_attention_forward<float>(
    Q.data(), K.data(), V.data(), O.data(), config);
```

---

### RoPE (旋转位置编码)

**头文件**: `attention/rope.cuh`

```cpp
#include "attention/rope.cuh"

namespace hpc::attention {

template <typename T>
void apply_rope(T* query, T* key,
                int batch, int num_heads, int seq_len, int head_dim,
                const float* cos_cache, const float* sin_cache,
                cudaStream_t stream = nullptr);

} // namespace hpc::attention
```

---

### TopK

用于 MoE（混合专家）路由。

**头文件**: `attention/topk.cuh`

```cpp
#include "attention/topk.cuh"

namespace hpc::attention {

template <typename T>
void topk(const T* input, T* output, int* indices,
          int batch, int n, int k,
          cudaStream_t stream = nullptr);

} // namespace hpc::attention
```

---

## 模块 06: 量化

### INT8 量化

逐行对称量化。

**头文件**: `quantization/int8_quant.cuh`

```cpp
#include "quantization/int8_quant.cuh"

namespace hpc::quantization {

void quantize_int8(const float* input, int8_t* output, float* scale,
                   int rows, int cols, cudaStream_t stream = nullptr);

void dequantize_int8(const int8_t* input, const float* scale,
                     float* output, int rows, int cols,
                     cudaStream_t stream = nullptr);

} // namespace hpc::quantization
```

---

## 模块 07: CUDA 13 特性

### TMA 拷贝

张量内存加速器（Tensor Memory Accelerator）工具。

**头文件**: `cuda13/tma.cuh`

```cpp
#include "cuda13/tma.cuh"

namespace hpc::cuda13 {

struct TMAConfig {
    int cluster_width = 1;
    int cluster_height = 1;
    int pipeline_depth = 2;
    bool use_tma = true;
};

template <typename T, int NUM_CHANNELS = 8>
void tma_copy_2d(const T* src, T* dst,
                 int rows, int cols,
                 const TMAConfig& config,
                 cudaStream_t stream = nullptr);

} // namespace hpc::cuda13
```

---

### 线程块集群 (Thread Block Clusters)

**头文件**: `cuda13/cluster.cuh`

```cpp
#include "cuda13/cluster.cuh"

namespace hpc::cuda13 {

struct ClusterConfig {
    dim3 cluster_dims;
    dim3 grid_dims;
    dim3 block_dims;
    bool use_cluster = true;
};

template <typename T>
void cluster_reduce(const T* input, T* output, size_t n,
                    const ClusterConfig& config,
                    cudaStream_t stream = nullptr);

} // namespace hpc::cuda13
```

---

### FP8 GEMM

**头文件**: `cuda13/fp8_gemm.cuh`

```cpp
#include "cuda13/fp8_gemm.cuh"

namespace hpc::cuda13 {

enum class FP8Format { e4m3, e5m2 };

struct FP8GEMMConfig {
    int tile_m = 128;
    int tile_n = 128;
    int tile_k = 64;
    FP8Format format_a = FP8Format::e4m3;
    FP8Format format_b = FP8Format::e4m3;
    float scale_a = 1.0f;
    float scale_b = 1.0f;
    bool use_fp8 = true;
};

void fp8_gemm(const __half* A, const __half* B, __half* C,
              int M, int N, int K,
              const FP8GEMMConfig& config,
              cudaStream_t stream = nullptr);

} // namespace hpc::cuda13
```

---

## Python API

### 模块结构

```python
import hpc_ai_opt

# 可用子模块
hpc_ai_opt.elementwise   # ReLU, Sigmoid, VectorAdd
hpc_ai_opt.reduction     # Softmax, LayerNorm, RMSNorm
hpc_ai_opt.gemm          # 矩阵乘法
```

### 使用模式

```python
import torch
import hpc_ai_opt

# 创建张量
x = torch.randn(1024, 1024, device="cuda", dtype=torch.float32)
y = torch.empty_like(x)

# 调用 CUDA 内核
hpc_ai_opt.elementwise.relu(x, y)

# 结果直接写入 y
```

### 可用函数

| 子模块 | 函数 | 签名 |
|-----------|----------|-----------|
| `elementwise` | `relu` | `(input, output)` |
| `elementwise` | `sigmoid` | `(input, output)` |
| `reduction` | `softmax` | `(input, output, batch, seq_len)` |
| `gemm` | `sgemm` | `(A, B, C, M, N, K)` |

---

## 类型约束

所有模板函数使用 C++20 概念（concepts）:

```cpp
// CudaNumeric 概念
template <typename T>
concept CudaNumeric = std::is_arithmetic_v<T> ||
                      std::is_same_v<T, __half> ||
                      std::is_same_v<T, __nv_bfloat16>;

// 在函数签名中的使用
template <CudaNumeric T>
void function(const T* input, T* output, size_t n);
```

---

## 错误处理

所有函数在以下情况下抛出 `std::invalid_argument`:
- 空指针参数
- 无效维度（在需要正数时为负数或零）
- 不支持的参数组合

```cpp
try {
    hpc::gemm::gemm<float>(nullptr, B, C, M, N, K);
} catch (const std::invalid_argument& e) {
    std::cerr << "Error: " << e.what() << std::endl;
}
```

CUDA 错误通过 `CUDA_CHECK_LAST()` 报告，它会抛出包含 CUDA 错误详情的 `std::runtime_error`。
