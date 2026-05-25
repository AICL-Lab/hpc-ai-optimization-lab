# 架构概览

本文档描述 HPC-AI-Optimization-Lab 的架构、设计模式与模块组织。

---

## 设计理念

### 核心原则

1. **渐进式优化 (Progressive Optimization)**：每个模块从基础实现到专家级优化逐步展开，让学习者理解每种优化手段背后的原理。

2. **RAII 资源管理 (RAII Resource Management)**：所有 GPU 资源（显存、流、事件）均通过 RAII 封装管理，防止泄露并保证异常安全。

3. **现代 C++ (Modern C++)**：充分利用 C++20 特性，包括概念 (concepts)、requires 子句以及 `std::span` 等。

4. **测试驱动开发 (Test-Driven Development)**：所有内核均包含单元测试（GoogleTest）与基于属性的测试（RapidCheck）。

5. **零开销抽象 (Zero-Cost Abstractions)**：基于模板的设计允许在编译期选择优化策略，不产生运行时开销。

---

## 项目架构

```
hpc-ai-optimization-lab/
├── src/                    # 内核实现
│   ├── common/             # 共享工具（仅头文件）
│   ├── elementwise/     # 模块 1：逐元素操作
│   ├── reduction/       # 模块 2：归约操作
│   ├── gemm/            # 模块 3：矩阵乘法
│   ├── convolution/     # 模块 4：卷积操作
│   ├── attention/       # 模块 5：注意力机制
│   └── quantization/    # 模块 6：量化工具
├── tests/                  # 测试套件
├── examples/               # 可运行示例
├── python/                 # Python 绑定
└── docs/                   # 文档
```

---

## 公共库 (`src/common/`)

公共库为所有模块提供基础工具。

### 核心组件

| 文件 | 用途 | 关键类型/函数 |
|------|------|---------------|
| `cuda_check.cuh` | 错误处理 | `CUDA_CHECK`, `CUDA_CHECK_LAST` |
| `types.cuh` | 类型特性 | `CudaNumeric` 概念, `accumulator_t` |
| `tensor.cuh` | GPU 显存封装 | `Tensor<T>` RAII 类 |
| `timer.cuh` | 性能计时 | `CudaTimer` 类 |
| `reduce.cuh` | 归约原语 | `warp_reduce_sum`, `block_reduce_sum` |
| `launch.cuh` | 内核启动辅助 | `LaunchConfig`, 网格尺寸计算 |
| `opt_level.cuh` | 优化级别选择 | `OptLevel` 枚举 |

### Tensor 类设计

```cpp
template <CudaNumeric T>
class Tensor {
public:
    explicit Tensor(size_t size);
    ~Tensor();
    
    // 移动语义（禁止拷贝）
    Tensor(Tensor&&) noexcept;
    Tensor& operator=(Tensor&&) noexcept;
    
    // 访问器
    T* data() noexcept;
    size_t size() const noexcept;
    
    // 主机-设备传输
    void copy_from_host(const T* data);
    void copy_to_host(T* data) const;
    std::vector<T> to_host() const;
    
    // 异步变体
    void copy_from_host_async(const T* data, cudaStream_t stream);
    
    // 操作
    void zero();
    
private:
    size_t size_;
    T* data_;
};
```

### 类型系统

```cpp
// CUDA 兼容类型的概念

template <typename T>
concept CudaNumeric = std::is_arithmetic_v<T> ||
                      std::is_same_v<T, __half> ||
                      std::is_same_v<T, __nv_bfloat16>;

// 累加器类型选择
template <typename T>
using accumulator_t = typename AccumulatorType<T>::type;
// float -> float, __half -> float, __nv_bfloat16 -> float
```

---

## 模块结构模式

每个内核模块遵循一致的结构：

### 文件组织

```
src/XX_modulename/
├── CMakeLists.txt      # 构建配置
├── kernel_name.cuh     # 公开接口（头文件）
└── kernel_name.cu      # 实现（模板特化）
```

### 头文件接口模式

```cpp
#pragma once

#include <cuda_runtime.h>
#include <concepts>

namespace hpc::module {

// 优化级别
enum class OptLevel {
    Naive,
    Intermediate,
    Advanced
};

// 带约束的模板声明
template <typename T, OptLevel Level = OptLevel::Advanced>
    requires std::is_same_v<T, float> || std::is_same_v<T, __half>
void kernel_name(const T* input, T* output, size_t n, 
                 cudaStream_t stream = nullptr);

} // namespace hpc::module
```

### 实现模式

```cpp
#include "kernel_name.cuh"
#include "../common/cuda_check.cuh"

namespace hpc::module {

// 私有内核实现
namespace {

template <typename T>
__global__ void kernel_naive(...) { /* ... */ }

template <typename T>
__global__ void kernel_optimized(...) { /* ... */ }

} // 匿名命名空间

// 显式模板特化
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

## 模块详情

### 模块 01：逐元素操作 (Elementwise Operations)

**目的**：演示显存访问优化技术。

| 操作 | 优化手段 |
|------|----------|
| ReLU | Naive、向量化 (Vectorized)、Grid Stride |
| Sigmoid | Naive、向量化 (Vectorized)、Grid Stride |
| Vector Add | Naive、向量化 (Vectorized)、Grid Stride |
| Transpose | Naive、共享内存 (Shared Memory)、填充 (Padded) |

**关键学习点**：内存合并访问 (Memory coalescing)、向量化 (Vectorization)、存储体冲突避免 (Bank conflict avoidance)。

### 模块 02：归约操作 (Reduction Operations)

**目的**：演示并行归约技术。

| 操作 | 优化手段 |
|------|----------|
| Softmax | 在线算法 (Online algorithm，单遍扫描) |
| LayerNorm | Welford 算法 |
| RMSNorm | 简化归一化 |

**关键学习点**：Warp 洗牌 (Warp shuffle)、块级归约 (Block reduction)、数值稳定性。

### 模块 03：GEMM (通用矩阵乘法)

**目的**：矩阵乘法优化的完整实践之旅。

| 步骤 | 技术 | 核心概念 |
|------|------|----------|
| 1 | Naive | 基线实现 |
| 2 | Shared Memory (共享内存) | 数据复用 |
| 3 | Double Buffer (双缓冲) | 延迟隐藏 |
| 4 | Register Tiling (寄存器分块) | 减少共享内存访问 |
| 5 | WMMA | Tensor Core API |
| 6 | MMA PTX | 底层 Tensor Core |
| 7 | Software Pipeline (软件流水线) | 多阶段重叠 |

**关键学习点**：完整的优化流程。

### 模块 04：卷积 (Convolution)

**目的**：图像卷积实现。

| 操作 | 描述 |
|------|------|
| Implicit GEMM | 生产就绪，已验证 |

### 模块 05：注意力 (Attention)

**目的**：Transformer 注意力机制。

| 操作 | 描述 |
|------|------|
| FlashAttention | IO 感知注意力，支持在线 Softmax |
| RoPE | 旋转位置编码 (Rotary Positional Embedding) |
| TopK | MoE 路由 |

### 模块 06：量化 (Quantization)

**目的**：低精度运算。

| 操作 | 描述 |
|------|------|
| INT8 Quantize | 逐行缩放 (Per-row scaling) |
| INT8 Dequantize | 从量化值还原 |
| FP8 Scaling | Hopper 架构占位符 |

## 构建系统

### CMake 结构

```cmake
# 根 CMakeLists.txt
project(HPC-AI-Optimization-Lab)

# 公共库（仅头文件，接口）
add_library(hpc_common INTERFACE)

# 模块库
function(hpc_add_cuda_library target)
    add_library(${target} ${ARGN})
    target_link_libraries(${target} PUBLIC hpc_common)
    # CUDA 特定选项...
endfunction()

# 每个模块
add_subdirectory(src/elementwise)  # 创建 hpc_elementwise
add_subdirectory(src/reduction)    # 创建 hpc_reduction
# ...
```

### 依赖图

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
```

---

## 测试架构

### 测试组织

```
tests/
├── test_utils.hpp         # 共享测试工具
├── CMakeLists.txt         # 测试配置
├── elementwise/           # 模块专用测试
├── reduction/
├── gemm/
├── attention/
├── quantization/
└── convolution/
```

### 测试工具

```cpp
namespace hpc::test {

// 浮点数比较
template <typename T>
bool almost_equal(T a, T b, T rel_tol = 1e-5, T abs_tol = 1e-6);

// 向量比较
template <typename T>
bool vectors_almost_equal(const std::vector<T>& a, const std::vector<T>& b);

// 随机数据生成
template <typename T>
std::vector<T> random_vector(size_t n, T min = -1.0, T max = 1.0);

} // namespace hpc::test
```

### 基于属性的测试

```cpp
// RapidCheck 属性测试
RC_GTEST_PROP(GemmTest, Correctness, ()) {
    auto M = *rc::gen::inRange<int>(1, 64);
    // 自动生成测试用例，发现边界情况
}
```

---

## Python 绑定

### 架构

```
python/
├── CMakeLists.txt      # Nanobind 配置
├── bindings/
│   └── bindings.cpp    # C++ -> Python 绑定
└── benchmark/
    └── benchmark.py    # 性能测试脚本
```

### 绑定模式

```cpp
// bindings.cpp
#include <nanobind/nanobind.h>
#include <nanobind/ndarray.h>

namespace nb = nanobind;

NB_MODULE(hpc_ai_opt, m) {
    // 逐元素子模块
    nb::module_ elementwise = m.def_submodule("elementwise");
    
    elementwise.def("relu", [](nb::ndarray<float> input,
                                nb::ndarray<float> output) {
        // 验证形状、提取指针、调用 CUDA 内核
    });
}
```

---

## 性能考量

### 存储器层次使用

| 存储器类型 | 延迟 | 带宽 | 使用场景 |
|------------|------|------|----------|
| 寄存器 (Registers) | 1 周期 | N/A | 线程本地累加器 |
| 共享内存 (Shared Memory) | ~5 周期 | ~20 TB/s | 块级数据复用 |
| L2 缓存 | ~200 周期 | ~3 TB/s | 自动管理缓存 |
| HBM | ~500 周期 | ~1-3 TB/s | 大数据集 |

### 占用率指导原则

| 内核类型 | 目标占用率 | 原因 |
|----------|------------|------|
| 显存受限 (Memory-bound) | 50%+ | 隐藏显存延迟 |
| 计算受限 (Compute-bound) | 25-50% | 饱和浮点单元 |
| Tensor Core | 12.5-25% | 高寄存器使用 |

---

## 扩展库

### 添加新内核

1. **创建模块目录**：`src/XX_newmodule/`

2. **定义头文件接口**：`kernel.cuh`
   ```cpp
   namespace hpc::newmodule {
   template<typename T>
   void kernel(...);
   }
   ```

3. **实现内核**：`kernel.cu`
   ```cpp
   template<>
   void kernel<float>(...) { /* 特化 */ }
   ```

4. **添加构建配置**：`CMakeLists.txt`
   ```cmake
   hpc_add_cuda_library(hpc_newmodule kernel.cu)
   ```

5. **编写测试**：`tests/newmodule/test_kernel.cpp`

6. **更新根 CMakeLists.txt**：
   ```cmake
   add_subdirectory(src/XX_newmodule)
   ```

---

## 最佳实践

### 错误处理

```cpp
// 务必检查 CUDA 错误
CUDA_CHECK(cudaMalloc(&ptr, size));
kernel<<<grid, block>>>(args);
CUDA_CHECK_LAST();  // 检查内核启动
```

### 资源管理

```cpp
// 优先使用 RAII 而非手动管理
hpc::Tensor<float> data(n);  // 自动清理

// 避免对拥有的资源使用裸指针
// float* data;
// cudaMalloc(&data, ...);  // 容易泄露
```

### 流管理

```cpp
// 使用显式流进行异步操作
cudaStream_t stream;
cudaStreamCreate(&stream);

kernel<<<grid, block, 0, stream>>>(args);

cudaStreamSynchronize(stream);
cudaStreamDestroy(stream);
```
