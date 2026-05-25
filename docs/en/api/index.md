# API Reference

This document provides comprehensive API documentation for all public interfaces in HPC-AI-Optimization-Lab.

---

## Table of Contents

- [Common Library](#common-library)
- [Module 01: Elementwise](#module-01-elementwise)
- [Module 02: Reduction](#module-02-reduction)
- [Module 03: GEMM](#module-03-gemm)
- [Module 04: Convolution](#module-04-convolution)
- [Module 05: Attention](#module-05-attention)
- [Module 06: Quantization](#module-06-quantization)
- [Python API](#python-api)

---

## Common Library

### Tensor\<T\>

RAII wrapper for GPU memory management.

**Header**: `common/tensor.cuh`

```cpp
#include "common/tensor.cuh"

namespace hpc {

template <CudaNumeric T>
class Tensor {
public:
    // Constructor
    explicit Tensor(size_t size);
    
    // Destructor (automatic cleanup)
    ~Tensor();
    
    // Move semantics (no copy)
    Tensor(Tensor&& other) noexcept;
    Tensor& operator=(Tensor&& other) noexcept;
    
    // Accessors
    T* data() noexcept;
    const T* data() const noexcept;
    size_t size() const noexcept;
    size_t bytes() const noexcept;
    bool empty() const noexcept;
    
    // Host ↔ Device transfers
    void copy_from_host(const T* host_data);
    void copy_to_host(T* host_data) const;
    void copy_from_host(const std::vector<T>& host_vec);
    std::vector<T> to_host() const;
    
    // Async transfers
    void copy_from_host_async(const T* host_data, cudaStream_t stream);
    void copy_to_host_async(T* host_data, cudaStream_t stream) const;
    
    // Operations
    void zero();
};

} // namespace hpc
```

**Example**:

```cpp
#include "common/tensor.cuh"

int main() {
    // Create tensor with 1024 floats
    hpc::Tensor<float> data(1024);
    
    // Initialize with host data
    std::vector<float> host_data(1024, 1.0f);
    data.copy_from_host(host_data);
    
    // Use in kernel
    my_kernel<<<1, 256>>>(data.data(), data.size());
    cudaDeviceSynchronize();
    
    // Retrieve results
    auto result = data.to_host();
    
    return 0;
}  // Automatic cleanup
```

---

### CudaTimer

GPU-side performance timing.

**Header**: `common/timer.cuh`

```cpp
#include "common/timer.cuh"

namespace hpc {

class CudaTimer {
public:
    CudaTimer();
    ~CudaTimer();
    
    // Non-copyable, movable
    CudaTimer(CudaTimer&&) noexcept;
    CudaTimer& operator=(CudaTimer&&) noexcept;
    
    // Timing operations
    void start(cudaStream_t stream = nullptr);
    void stop(cudaStream_t stream = nullptr);
    
    // Results
    float elapsed_ms() const;
};

} // namespace hpc
```

**Example**:

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

### CUDA_CHECK Macro

Error handling macro for CUDA operations.

**Header**: `common/cuda_check.cuh`

```cpp
#include "common/cuda_check.cuh"

// Check return value
CUDA_CHECK(cudaMalloc(&ptr, size));

// Check last kernel launch
kernel<<<grid, block>>>(args);
CUDA_CHECK_LAST();
```

---

## Module 01: Elementwise

### ReLU

Rectified Linear Unit activation function.

**Header**: `elementwise/relu.cuh`

```cpp
#include "elementwise/relu.cuh"

namespace hpc::elementwise {

enum class OptLevel {
    Naive,       // Basic per-element processing
    Vectorized,  // float4 load/store
    GridStride   // Grid stride loop (recommended)
};

template <typename T, OptLevel Level = OptLevel::GridStride>
    requires std::is_same_v<T, float> || std::is_same_v<T, __half>
void relu(const T* input, T* output, size_t n, 
          cudaStream_t stream = nullptr);

} // namespace hpc::elementwise
```

**Example**:

```cpp
#include "elementwise/relu.cuh"
#include "common/tensor.cuh"

// Using default optimization (GridStride)
hpc::Tensor<float> input(1024);
hpc::Tensor<float> output(1024);

hpc::elementwise::relu<float>(input.data(), output.data(), 1024);

// Using specific optimization level
hpc::elementwise::relu<float, hpc::elementwise::OptLevel::Vectorized>(
    input.data(), output.data(), 1024);
```

---

### Sigmoid

Sigmoid activation function.

**Header**: `elementwise/sigmoid.cuh`

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

Elementwise vector addition.

**Header**: `elementwise/vector_add.cuh`

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

Matrix transpose with optimization options.

**Header**: `elementwise/transpose.cuh`

```cpp
#include "elementwise/transpose.cuh"

namespace hpc::elementwise {

enum class TransposeOpt {
    Naive,           // Direct read-row write-col
    SharedMemory,    // Use shared memory
    SharedMemPadded  // Shared memory with padding (recommended)
};

template <typename T, TransposeOpt Opt = TransposeOpt::SharedMemPadded>
    requires std::is_same_v<T, float> || std::is_same_v<T, __half>
void transpose(const T* input, T* output, int rows, int cols,
               cudaStream_t stream = nullptr);

} // namespace hpc::elementwise
```

---

## Module 02: Reduction

### Softmax

Numerically stable softmax with online algorithm.

**Header**: `reduction/softmax.cuh`

```cpp
#include "reduction/softmax.cuh"

namespace hpc::reduction {

enum class SoftmaxOpt {
    Naive,          // Two-pass with global atomics
    WarpShuffle,    // Warp-level reduction
    OnlineSoftmax,  // Single-pass (recommended)
    Fused           // Fused with L2 cache persistence
};

template <typename T, SoftmaxOpt Opt = SoftmaxOpt::OnlineSoftmax>
    requires std::is_same_v<T, float> || std::is_same_v<T, __half>
void softmax(const T* input, T* output, int batch, int seq_len,
             cudaStream_t stream = nullptr);

} // namespace hpc::reduction
```

**Example**:

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

Layer normalization.

**Header**: `reduction/layernorm.cuh`

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

Root Mean Square Layer Normalization.

**Header**: `reduction/rmsnorm.cuh`

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

## Module 03: GEMM

### General Matrix Multiply

7-step optimization progression.

**Header**: `gemm/gemm.cuh`

```cpp
#include "gemm/gemm.cuh"

namespace hpc::gemm {

enum class GemmOpt {
    Naive,            // Step 1: Global memory only
    SharedMemTiling,  // Step 2: Shared memory tiling
    DoubleBuffer,     // Step 3: Double buffering
    RegisterTiling,   // Step 4: Register tiling
    TensorCoreWMMA,   // Step 5: WMMA API ✅
    TensorCoreMMA,    // Step 6: MMA PTX (delegates to Step 5) 🚧
    SoftwarePipeline  // Step 7: Software pipelining ✅
};

template <typename T, GemmOpt Opt = GemmOpt::SharedMemTiling>
    requires std::is_same_v<T, float> || 
             std::is_same_v<T, __half> || 
             std::is_same_v<T, int8_t>
void gemm(const T* A, const T* B, T* C,
          int M, int N, int K,
          float alpha = 1.0f, float beta = 0.0f,
          cudaStream_t stream = nullptr);

// CUTLASS baseline (float only, experimental)
template <typename T>
void gemm_cutlass(const T* A, const T* B, T* C,
                  int M, int N, int K,
                  float alpha = 1.0f, float beta = 0.0f,
                  cudaStream_t stream = nullptr);

} // namespace hpc::gemm
```

**Type Support Matrix**:

| Type | Steps 1-4 | Step 5 (WMMA) | Step 6 (MMA) | Step 7 (Pipeline) | CUTLASS |
|------|-----------|---------------|--------------|-------------------|---------|
| `float` | ✅ | ✅ | ✅† | ✅ | ✅ |
| `__half` | ✅ | ✅ | ✅† | ✅ | 🚧 |
| `int8_t` | ✅‡ | 🚧 | 🚧 | 🚧 | 🚧 |

† Step 6 currently delegates to Step 5 for stability  
‡ INT8 GEMM currently only has SharedMemTiling optimization fully implemented; other optimization levels delegate to SharedMemTiling

**Example**:

```cpp
#include "gemm/gemm.cuh"

int M = 1024, N = 1024, K = 1024;

hpc::Tensor<float> A(M * K);
hpc::Tensor<float> B(K * N);
hpc::Tensor<float> C(M * N);

// Basic shared memory tiling
hpc::gemm::gemm<float, hpc::gemm::GemmOpt::SharedMemTiling>(
    A.data(), B.data(), C.data(), M, N, K);

// Tensor Core (requires __half)
hpc::Tensor<__half> A_h(M * K);
hpc::Tensor<__half> B_h(K * N);
hpc::Tensor<__half> C_h(M * N);

hpc::gemm::gemm<__half, hpc::gemm::GemmOpt::TensorCoreWMMA>(
    A_h.data(), B_h.data(), C_h.data(), M, N, K);

// CUTLASS baseline (float only)
hpc::gemm::gemm_cutlass<float>(
    A.data(), B.data(), C.data(), M, N, K);
```

---

## Module 04: Convolution

### Implicit GEMM Convolution

Production-ready convolution implementation.

**Header**: `convolution/conv_implicit_gemm.cuh`

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

## Module 05: Attention

### FlashAttention Forward

IO-aware attention mechanism.

**Header**: `attention/flash_attention.cuh`

```cpp
#include "attention/flash_attention.cuh"

namespace hpc::attention {

struct FlashAttnConfig {
    int batch_size;
    int num_heads;
    int seq_len;
    int head_dim;    // Currently supports 64
    float scale;     // Typically 1/sqrt(head_dim)
    bool causal;     // Apply causal mask
};

template <typename T>
    requires std::is_same_v<T, float> || std::is_same_v<T, __half>
void flash_attention_forward(const T* Q, const T* K, const T* V,
                             T* O,
                             const FlashAttnConfig& config,
                             cudaStream_t stream = nullptr);

} // namespace hpc::attention
```

Current shipped implementation specializes `float` with `head_dim == 64`.

**Example**:

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

### RoPE (Rotary Positional Embedding)

**Header**: `attention/rope.cuh`

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

For MoE routing.

**Header**: `attention/topk.cuh`

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

## Module 06: Quantization

### INT8 Quantization

Per-row symmetric quantization.

**Header**: `quantization/int8_quant.cuh`

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

## Python API

### Module Structure

```python
import hpc_ai_opt

# Available submodules
hpc_ai_opt.elementwise   # ReLU, Sigmoid, VectorAdd
hpc_ai_opt.reduction     # Softmax, LayerNorm, RMSNorm
hpc_ai_opt.gemm          # Matrix multiplication
```

### Usage Pattern

```python
import torch
import hpc_ai_opt

# Create tensors
x = torch.randn(1024, 1024, device="cuda", dtype=torch.float32)
y = torch.empty_like(x)

# Call CUDA kernel
hpc_ai_opt.elementwise.relu(x, y)

# Results are in-place in y
```

### Available Functions

| Submodule | Function | Signature |
|-----------|----------|-----------|
| `elementwise` | `relu` | `(input, output)` |
| `elementwise` | `sigmoid` | `(input, output)` |
| `reduction` | `softmax` | `(input, output, batch, seq_len)` |
| `gemm` | `sgemm` | `(A, B, C, M, N, K)` |

---

## Type Constraints

All template functions use C++20 concepts:

```cpp
// CudaNumeric concept
template <typename T>
concept CudaNumeric = std::is_arithmetic_v<T> ||
                      std::is_same_v<T, __half> ||
                      std::is_same_v<T, __nv_bfloat16>;

// Usage in function signatures
template <CudaNumeric T>
void function(const T* input, T* output, size_t n);
```

---

## Error Handling

All functions throw `std::invalid_argument` for:
- Null pointer arguments
- Invalid dimensions (negative or zero where positive required)
- Unsupported parameter combinations

```cpp
try {
    hpc::gemm::gemm<float>(nullptr, B, C, M, N, K);
} catch (const std::invalid_argument& e) {
    std::cerr << "Error: " << e.what() << std::endl;
}
```

CUDA errors are reported via `CUDA_CHECK_LAST()` which throws `std::runtime_error` with CUDA error details.
