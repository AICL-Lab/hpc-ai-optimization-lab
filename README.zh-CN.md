<div align="center">

<!-- 项目 Banner 占位符 - 推荐尺寸: 1200x400 -->
<!-- ![HPC-AI-Optimization-Lab Banner](docs/assets/banner.png) -->

<h1>HPC-AI-Optimization-Lab</h1>

<p align="center">
  <strong>面向 GEMM、FlashAttention、量化与 GPU 性能学习的 CUDA 内核优化实验仓库。</strong>
</p>

<p align="center">
  <a href="https://github.com/LessUp/hpc-ai-optimization-lab/stargazers">
    <img src="https://img.shields.io/github/stars/LessUp/hpc-ai-optimization-lab?style=flat-square&color=yellow" alt="GitHub Stars">
  </a>
  <a href="https://github.com/LessUp/hpc-ai-optimization-lab/network/members">
    <img src="https://img.shields.io/github/forks/LessUp/hpc-ai-optimization-lab?style=flat-square&color=blue" alt="GitHub Forks">
  </a>
  <a href="https://github.com/LessUp/hpc-ai-optimization-lab/issues">
    <img src="https://img.shields.io/github/issues/LessUp/hpc-ai-optimization-lab?style=flat-square&color=red" alt="GitHub Issues">
  </a>
  <br>
  <a href="https://developer.nvidia.com/cuda-toolkit">
    <img src="https://img.shields.io/badge/CUDA-12.4+-76B900?logo=nvidia&style=flat-square" alt="CUDA 12.4+">
  </a>
  <a href="https://en.cppreference.com/w/cpp/20">
    <img src="https://img.shields.io/badge/C++-20-00599C?logo=cplusplus&style=flat-square" alt="C++20">
  </a>
  <a href="https://cmake.org/">
    <img src="https://img.shields.io/badge/CMake-3.24+-064F8C?logo=cmake&style=flat-square" alt="CMake 3.24+">
  </a>
  <a href="LICENSE">
    <img src="https://img.shields.io/badge/License-Apache%202.0-green?style=flat-square" alt="Apache 2.0">
  </a>
  <a href="https://lessup.github.io/hpc-ai-optimization-lab/">
    <img src="https://img.shields.io/badge/文档-VitePress-blue?style=flat-square" alt="Documentation">
  </a>
</p>

<p align="center">
  <a href="README.md">English</a> | 
  <a href="README.zh-CN.md">简体中文</a>
</p>

</div>

---

## 目录

- [项目概述](#项目概述)
- [核心特性](#核心特性)
- [快速开始](#快速开始)
  - [安装](#安装)
  - [验证安装](#验证安装)
  - [运行第一个内核](#运行第一个内核)
- [性能表现](#性能表现)
- [文档](#文档)
- [项目结构](#项目结构)
- [使用示例](#使用示例)
- [测试](#测试)
- [贡献](#贡献)
- [路线图](#路线图)
- [支持矩阵](#支持矩阵)
- [许可证](#许可证)

---

## 项目概述

**HPC-AI-Optimization-Lab** 架起了 CUDA 教学教程与生产级库之间的桥梁。通过渐进式的内核优化实践——从简单的逐元素操作到 Tensor Core 加速的矩阵乘法——学习 GPU 优化技术。

### 为什么选择本项目？

目前大多数 CUDA 学习资源处于两个极端：
- 📚 **教程** 讲解概念但缺乏生产级质量
- ⚡ **库**（cuBLAS、CUTLASS）高度优化但学习曲线陡峭

**我们提供中间地带：**

| 维度 | 我们的方式 |
|------|-----------|
| **学习路径** | 7步渐进式优化，每步都有详细说明 |
| **代码质量** | 经过测试和基准测试，遵循 NVIDIA 最佳实践 |
| **API 设计** | C++ 和 Python 都提供简洁一致的接口 |
| **现代 AI** | 包含 FlashAttention、RoPE、INT8/FP8 量化 |

### 适合人群

- 🎓 **学生**：从第一性原理学习 CUDA 优化
- 🔬 **研究者**：原型化新的内核优化方案
- 🏭 **工程师**：AI 推理工作负载的生产级内核

---

## 核心特性

- **渐进式学习**：7步 GEMM 优化，展示每种技术的影响
- **全面覆盖**：逐元素操作、归约、GEMM、注意力机制、卷积、量化
- **现代硬件**：CUDA 12.4+、Hopper 特性（TMA、Clusters）、FP8 支持
- **双语言 API**：C++17/20 模板 + Python 绑定（nanobind）
- **核心模块具备测试覆盖**：GoogleTest + RapidCheck 覆盖主要模块接口与边界情况

---

## 快速开始

### 安装

```bash
# 克隆仓库
git clone https://github.com/LessUp/hpc-ai-optimization-lab.git
cd hpc-ai-optimization-lab

# 创建构建目录
mkdir build && cd build

# 配置并构建
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . -j$(nproc)
```

**环境要求：**
| 组件 | 最低版本 | 推荐版本 |
|------|---------|----------|
| CUDA Toolkit | 12.4 | 12.4+ |
| CMake | 3.24 | 3.28+ |
| 编译器 | GCC 11 / Clang 14 | GCC 12+ / Clang 15+ |
| GPU | SM 7.0 (Volta) | SM 8.0+ (Ampere) |

**构建选项：**
```bash
# 包含示例和 Python 绑定
cmake .. -DBUILD_EXAMPLES=ON -DBUILD_PYTHON_BINDINGS=ON

# 指定目标架构
cmake .. -DCMAKE_CUDA_ARCHITECTURES="80;90"  # A100 + H100
```

### 验证安装

```bash
# 运行测试套件
ctest --output-on-failure

# 预期输出：所选本地测试预设能够在你的环境中正常运行
```

### 运行第一个内核

```bash
# 逐元素操作示例
./examples/elementwise/relu_example

# GEMM 基准测试（包含全部 7 个优化步骤）
./examples/gemm/gemm_benchmark

# Python 示例（如果启用了绑定）
python ../examples/python/basic_usage.py
```

<details>
<summary><b>遇到问题？</b>常见问题排查</summary>

**CMake 找不到 CUDA：**
```bash
export PATH=/usr/local/cuda/bin:$PATH
export CMAKE_CUDA_COMPILER=/usr/local/cuda/bin/nvcc
```

**构建时内存不足：**
```bash
# 减少并行任务数
cmake --build . -j4
```

更多问题请参考[故障排除指南](docs/zh-CN/guide/troubleshooting.md)。

</details>

---

## 性能表现

### GEMM 优化之旅

FP32 矩阵乘法（4096×4096）在 NVIDIA A100 上的表现：

| 步骤 | 技术 | 性能 | 加速 |
|:----:|------|------|:----:|
| 1 | 朴素实现 | 0.5 TFLOPS | 1× |
| 2 | 共享内存分块 | 2.0 TFLOPS | 4× |
| 3 | 双缓冲 | 3.5 TFLOPS | 7× |
| 4 | 寄存器分块 | 6.0 TFLOPS | 12× |
| 5 | **Tensor Core (WMMA)** | **50+ TFLOPS** | **100×** |
| 6 | Tensor Core (MMA PTX) | 60+ TFLOPS | 120× |
| 7 | 软件流水线 | 70+ TFLOPS | 140× |

> 💡 从步骤 1 到步骤 5 的飞跃展示了现代 AI 硬件如何通过专用计算单元实现惊人的加速。

### 模块状态

| 模块 | 关键操作 | 状态 |
|------|---------|:----:|
| **逐元素操作** | ReLU、Sigmoid、转置 | ✅ 稳定 |
| **归约** | Softmax、LayerNorm、RMSNorm | ✅ 稳定 |
| **GEMM** | 矩阵乘法（7步优化） | ✅ 稳定 |
| **注意力机制** | FlashAttention、RoPE | ✅ 稳定 |
| **卷积** | 隐式 GEMM | ✅ 稳定 |
| **量化** | INT8、FP8 | ✅ 稳定 |

---

## 文档

📚 **完整文档：** [https://lessup.github.io/hpc-ai-optimization-lab/](https://lessup.github.io/hpc-ai-optimization-lab/)

### 快速链接

| 主题 | 英文文档 | 中文文档 |
|------|---------|---------|
| 安装指南 | [Installation](docs/en/guide/installation.md) | [安装指南](docs/zh-CN/guide/installation.md) |
| 快速入门 | [5-min Guide](docs/en/guide/quick-start.md) | [快速入门](docs/zh-CN/guide/quick-start.md) |
| GEMM 教程 | [7-Step Journey](docs/en/guide/gemm.md) | [GEMM优化](docs/zh-CN/guide/gemm.md) |
| 访存优化 | [Guide](docs/en/guide/memory.md) | [访存优化](docs/zh-CN/guide/memory.md) |
| FlashAttention | [Guide](docs/en/guide/flash-attention.md) | [FlashAttention](docs/zh-CN/guide/flash-attention.md) |
| API 参考 | [C++/Python](docs/en/api/index.md) | [API参考](docs/zh-CN/api/index.md) |

### 学习路径

```
🌱 初级（1-2 周）
├── 安装与快速开始
├── 访存优化模式
├── 归约算法
└── GEMM 步骤 1-4

🚀 中级（2-4 周）
├── GEMM 步骤 5-7（Tensor Core）
├── FlashAttention 实现
└── 性能分析与调优

🏆 高级（持续学习）
├── CUDA 13 Hopper 特性
├── CUTLASS 深入研究
└── 论文复现
```

---

## 项目结构

```
hpc-ai-optimization-lab/
├── src/                    # CUDA 内核实现
│   ├── common/             # Tensor、Timer、CUDA 工具
│   ├── elementwise/        # ReLU、Sigmoid 等
│   ├── reduction/          # Softmax、LayerNorm、RMSNorm
│   ├── gemm/               # 7步 GEMM 优化
│   ├── attention/          # FlashAttention、RoPE
│   ├── convolution/        # 隐式 GEMM
│   ├── quantization/       # INT8/FP8
│   └── cuda13/             # Hopper 专属特性
├── tests/                  # 测试套件（GoogleTest + RapidCheck）
├── examples/               # 独立示例
├── python/                 # Python 绑定
├── docs/                   # 文档（VitePress）
└── docker/                 # 开发环境
```

---

## 使用示例

### C++ API

```cpp
#include "gemm/gemm.cuh"
#include "common/tensor.cuh"

// 分配 GPU 张量
auto A = hpc::common::make_tensor<float>({1024, 1024});
auto B = hpc::common::make_tensor<float>({1024, 1024});
auto C = hpc::common::make_tensor<float>({1024, 1024});

// 启动优化后的内核
hpc::gemm::gemm<float, hpc::gemm::OptLevel::Advanced>(
    A.data(), B.data(), C.data(), 1024, 1024, 1024);

// 张量超出作用域时自动释放内存
```

### Python API

```python
import hpc_ai_opt
import numpy as np

# 创建输入数据
A = np.random.randn(1024, 1024).astype(np.float32)
B = np.random.randn(1024, 1024).astype(np.float32)

# 执行优化的 GEMM
C = hpc_ai_opt.gemm(A, B)

print(f"结果形状: {C.shape}")
print(f"性能: {hpc_ai_opt.last_tflops:.1f} TFLOPS")
```

---

## 测试

```bash
# 运行所有测试
ctest --output-on-failure

# 运行特定模块
./tests/gemm/test_gemm

# 详细输出
ctest -V
```

| 测试类型 | 状态 |
|---------|------|
| 单元测试（GoogleTest） | 覆盖公开模块接口 |
| 属性测试（RapidCheck） | 用于边界情况与不变量验证 |
| 本地基线 | 使用 CMake preset 重新配置后再信任结果 |

---

## 贡献

我们欢迎贡献！请参阅 [CONTRIBUTING.md](CONTRIBUTING.md) 了解指南。

```bash
# 快速贡献流程
git checkout -b feature/my-optimization
# ... 进行修改 ...
cmake -S . -B build && cmake --build build
ctest --test-dir build --output-on-failure
git commit -m "feat(gemm): add optimization X"
git push origin feature/my-optimization
```

> ⚠️ **CI 注意：** 当前仓库在 GitHub 托管运行器上**并不提供完整的原生 CUDA build-and-test**。GPU 相关验证仍需要本地环境或自托管基础设施。

---

## 路线图

仓库当前处于收尾与加固阶段。

### 当前重点

- 清理仓库结构并完成 OpenSpec 对齐
- 降低 workflow 与文档噪音
- 改进 GitHub Pages 与仓库对外展示
- 修复文档、工具链与自动化中的失真和不一致问题

### 当前不优先

- 大范围新增功能
- 没有边界的未来版本承诺
- 未经活跃 bounded change 支撑的大型新集成面

---

## 支持矩阵

| 模块 | FP32 | FP16 | BF16 | INT8 | FP8 | 状态 |
|------|:----:|:----:|:----:|:----:|:---:|:----:|
| 逐元素操作 | ✅ | ✅ | ✅ | - | - | 稳定 |
| 归约 | ✅ | ✅ | ✅ | - | - | 稳定 |
| GEMM | ✅ | ✅ | ✅ | ✅ | 🚧 | 稳定 |
| 卷积 | ✅ | ✅ | - | - | - | 稳定 |
| 注意力机制 | ✅ | ✅ | - | - | - | 稳定 |
| 量化 | ✅ | ✅ | - | ✅ | 🚧 | 稳定 |

🚧 = 部分支持 / 开发中

---

## 许可证

本项目采用 [Apache License 2.0](LICENSE) 许可证。

---

<div align="center">

**⭐ 如果觉得有帮助，请点个 Star！**

[报告 Bug](https://github.com/LessUp/hpc-ai-optimization-lab/issues) · 
[请求功能](https://github.com/LessUp/hpc-ai-optimization-lab/issues) · 
[文档](https://lessup.github.io/hpc-ai-optimization-lab/)

</div>
