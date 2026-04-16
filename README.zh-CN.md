# HPC-AI-Optimization-Lab

<div align="center">

**面向 AI 工作负载的综合 CUDA 内核优化实验室**

[![CUDA](https://img.shields.io/badge/CUDA-12.4+-76B900?logo=nvidia)](https://developer.nvidia.com/cuda-toolkit)
[![C++20](https://img.shields.io/badge/C++-20-00599C?logo=cplusplus)](https://en.cppreference.com/w/cpp/20)
[![CMake](https://img.shields.io/badge/CMake-3.24+-064F8C?logo=cmake)](https://cmake.org/)
[![License](https://img.shields.io/badge/License-Apache%202.0-green)](LICENSE)

[English](README.md) | 简体中文

</div>

---

## 🎯 概述

HPC-AI-Optimization-Lab 是一个面向 AI 推理工作负载的教育性和生产级 CUDA 内核库。它提供了关键 GPU 操作从基础到专家级的逐步优化实现。

### 核心特性

- **📚 渐进式学习路径**：每个模块展示从基础到专家级的优化技术
- **🔬 生产级质量**：所有内核包含完整测试覆盖（GoogleTest + RapidCheck）
- **🚀 现代 C++20**：使用概念、RAII 和现代设计模式
- **🐍 Python 绑定**：可选的 nanobind Python 接口用于快速原型开发

---

## 📁 项目结构

```
hpc-ai-optimization-lab/
├── src/
│   ├── common/           # 共享工具（Tensor、Timer、CUDA 检查）
│   ├── 01_elementwise/   # ReLU、Sigmoid、向量加法、转置
│   ├── 02_reduction/     # Softmax、LayerNorm、RMSNorm
│   ├── 03_gemm/          # 7 步 GEMM 优化之旅
│   ├── 04_convolution/   # 隐式 GEMM、Winograd 卷积
│   ├── 05_attention/     # FlashAttention、RoPE、TopK
│   ├── 06_quantization/  # INT8/FP8 量化工具
│   └── 07_cuda13_features/ # 实验性 Hopper 架构特性
├── tests/                # 完整测试套件
├── examples/             # CUDA 和 Python 示例
├── python/               # Nanobind Python 绑定
└── docs/                 # 技术文档
```

---

## 🚀 快速开始

### 环境要求

| 需求 | 版本 |
|------|------|
| CUDA Toolkit | 12.4+ |
| CMake | 3.24+ |
| C++ 编译器 | GCC 11+ / Clang 14+ |
| NVIDIA GPU | 计算能力 7.0+ |

### 构建

```bash
# 克隆仓库
git clone https://github.com/LessUp/hpc-ai-optimization-lab.git
cd hpc-ai-optimization-lab

# 配置和构建
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j$(nproc)

# 运行测试
ctest --test-dir build --output-on-failure
```

### 构建示例

```bash
cmake -S . -B build -DBUILD_EXAMPLES=ON
cmake --build build --target relu_example gemm_benchmark

# 运行示例
./build/examples/relu_example
./build/examples/gemm_benchmark
```

### 构建 Python 绑定

```bash
cmake -S . -B build -DBUILD_PYTHON_BINDINGS=ON
cmake --build build
export PYTHONPATH="$(pwd)/build/python:${PYTHONPATH}"

# 验证安装
python -c "import hpc_ai_opt; print('模块加载成功！')"

# 运行 Python 示例
python examples/python/basic_usage.py
```

---

## 📚 模块概述

### 01 - 逐元素操作

涵盖的优化技术：
- 朴素实现
- 向量化加载/存储（`float4`）
- Grid Stride 循环处理任意大小输入
- 共享内存用于转置操作

```cpp
#include "01_elementwise/relu.cuh"

// 使用优化的 Grid Stride 实现
hpc::elementwise::relu<float, hpc::elementwise::OptLevel::GridStride>(
    d_input, d_output, n, stream);
```

### 02 - 归约操作

涵盖的优化技术：
- Warp Shuffle 原语
- Block 级归约
- Online Softmax 算法
- Welford 算法保证数值稳定性

```cpp
#include "02_reduction/softmax.cuh"

// Online Softmax - 单次遍历算法
hpc::reduction::softmax<float, hpc::reduction::SoftmaxOpt::OnlineSoftmax>(
    d_input, d_output, batch, seq_len, stream);
```

### 03 - GEMM（7 步优化）

旗舰模块，展示渐进式 GEMM 优化：

| 步骤 | 技术 | FP32 TFLOPS | 关键洞察 |
|------|------|-------------|---------|
| 1 | 朴素 | ~0.5 | 基线 - 每个线程计算一个元素 |
| 2 | 共享内存分块 | ~2.0 | 减少 TILE_SIZE 倍的全局内存访问 |
| 3 | 双缓冲 | ~3.5 | 计算与访存重叠隐藏延迟 |
| 4 | 寄存器分块 | ~6.0 | 减少共享内存 bank 冲突 |
| 5 | Tensor Core WMMA | ~50+ | 硬件加速矩阵操作 |
| 6 | Tensor Core MMA PTX | ~60+ | 细粒度 Tensor Core 控制 |
| 7 | 软件流水线 | ~70+ | 多阶段执行重叠 |

```cpp
#include "03_gemm/gemm.cuh"

// 使用 Tensor Core 优化
hpc::gemm::gemm<__half, hpc::gemm::GemmOpt::TensorCoreWMMA>(
    d_A, d_B, d_C, M, N, K, 1.0f, 0.0f, stream);
```

### 04 - 卷积

- 隐式 GEMM 卷积（已验证，生产就绪）
- Winograd 卷积（3×3 核，实验性回退）

### 05 - 注意力机制

- FlashAttention 前向传播（在线 Softmax）
- RoPE（旋转位置编码）
- MoE TopK 路由

### 06 - 量化

- INT8 逐行量化/反量化
- FP8 缩放工具（Hopper 支持占位符）

### 07 - CUDA 13 特性（实验性）

> **注意**：这些模块提供教育示例和回退实现。完整的 Hopper 特性支持需要 SM 9.0+。

- TMA（张量内存加速器）- 异步复制回退
- Thread Block Clusters - Block 归约回退
- FP8 GEMM - 缩放 FP16 演示

---

## 🧪 测试

项目采用双层测试策略：

### 单元测试（GoogleTest）

```bash
# 运行所有测试
ctest --test-dir build --output-on-failure

# 运行特定测试套件
./build/tests/gemm/test_gemm
```

### 属性测试（RapidCheck）

属性测试自动生成测试用例以发现边界情况：

```cpp
RC_GTEST_PROP(GemmTest, Correctness, ()) {
    auto M = *rc::gen::inRange<int>(1, 64);
    auto N = *rc::gen::inRange<int>(1, 64);
    auto K = *rc::gen::inRange<int>(1, 64);
    // ... 自动验证所有组合的正确性
}
```

---

## 📖 文档

### 技术指南

| 文档 | 描述 | 难度 |
|------|------|------|
| [GEMM 优化](docs/zh-CN/01_gemm_optimization.md) | 7步矩阵乘法优化之旅 | ⭐⭐⭐⭐ |
| [访存优化](docs/zh-CN/02_memory_optimization.md) | 合并访问、向量化、共享内存 | ⭐⭐ |
| [归约优化](docs/zh-CN/03_reduction_optimization.md) | Warp洗牌、在线算法 | ⭐⭐⭐ |
| [FlashAttention](docs/zh-CN/04_flash_attention.md) | IO感知的注意力机制 | ⭐⭐⭐⭐ |
| [CUDA 13 特性](docs/zh-CN/05_cuda13_features.md) | Hopper架构特性 | ⭐⭐⭐⭐⭐ |
| [API 参考](docs/zh-CN/API_REFERENCE.md) | 完整API参考文档 | ⭐⭐⭐ |
| [架构概览](docs/zh-CN/ARCHITECTURE.md) | 设计模式与模块组织 | ⭐⭐ |

### 推荐学习路径

```
初学者（1-2 周）：
└── 内存优化 → 归约 → GEMM（步骤 1-4）

进阶（2-4 周）：
└── GEMM（步骤 5-7）→ FlashAttention

专家（持续学习）：
└── CUDA 13 特性 → CUTLASS 源码 → 研究论文
```

---

## 🔧 配置选项

| CMake 选项 | 默认值 | 描述 |
|------------|--------|------|
| `BUILD_EXAMPLES` | OFF | 构建 CUDA 和 Python 示例 |
| `BUILD_PYTHON_BINDINGS` | OFF | 构建 nanobind Python 模块 |
| `CMAKE_CUDA_ARCHITECTURES` | native | 目标 GPU 架构 |

---

## 🐳 Docker 环境

```bash
cd docker
docker-compose up -d
docker exec -it hpc-ai-lab bash
```

---

## 🤝 贡献

欢迎贡献！请查看 [CONTRIBUTING.md](CONTRIBUTING.md) 了解指南。

---

## 📄 许可证

本项目采用 Apache License 2.0 许可证 - 详见 [LICENSE](LICENSE)。

---

## 🙏 致谢

- [NVIDIA CUTLASS](https://github.com/NVIDIA/cutlass) - 参考实现
- [FlashAttention](https://github.com/Dao-AILab/flash-attention) - 注意力优化技术
- [How to Optimize a CUDA Matmul Kernel](https://siboehm.com/articles/22/CUDA-MMM) - 优秀教程

---

## 📊 支持矩阵

### 稳定 / 生产就绪

| 模块 | FP32 | FP16 | INT8 | 备注 |
|------|------|------|------|------|
| 逐元素 | ✅ | ✅ | - | 所有优化级别 |
| 归约 | ✅ | ✅ | - | 在线算法 |
| GEMM | ✅ | ✅ | ✅ | 7 步渐进 |
| 卷积 | ✅ | - | - | 隐式 GEMM 已验证 |
| 注意力 | ✅ | - | - | 仅支持 head_dim=64 |
| 量化 | ✅ | - | ✅ | 逐行缩放 |

### 实验性 / 回退

| 模块 | 状态 | 备注 |
|------|------|------|
| Winograd 卷积 | 回退 | 使用隐式 GEMM 路径 |
| TMA | 回退 | 使用异步复制代替 |
| Thread Block Clusters | 回退 | 使用 Block 归约 |
| FP8 GEMM | 演示 | 缩放 FP16 行为 |

---

<div align="center">

**学习愉快！🚀**

[报告 Bug](https://github.com/LessUp/hpc-ai-optimization-lab/issues) · [请求功能](https://github.com/LessUp/hpc-ai-optimization-lab/issues)

</div>
