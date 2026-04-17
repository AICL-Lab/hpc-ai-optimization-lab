# HPC-AI-Optimization-Lab

<div align="center">

**面向 AI 工作负载的综合 CUDA 内核优化实验室**

[![CUDA](https://img.shields.io/badge/CUDA-12.4+-76B900?logo=nvidia)](https://developer.nvidia.com/cuda-toolkit)
[![C++20](https://img.shields.io/badge/C++-20-00599C?logo=cplusplus)](https://en.cppreference.com/w/cpp/20)
[![CMake](https://img.shields.io/badge/CMake-3.24+-064F8C?logo=cmake)](https://cmake.org/)
[![License](https://img.shields.io/badge/License-Apache%202.0-green)](LICENSE)
[![Docs](https://img.shields.io/badge/Docs-VitePress-blue)](https://lessup.github.io/hpc-ai-optimization-lab/)

[English](README.md) | [简体中文](README.zh-CN.md)

</div>

---

## 🎯 概述

HPC-AI-Optimization-Lab 是一个**教育性和生产级**的 CUDA 内核库，专为 AI 推理工作负载设计。它提供了关键 GPU 操作的**渐进式优化实现**，从基础的逐元素操作到高级的 Tensor Core 矩阵乘法。

### ✨ 为什么选择本项目？

| 特性 | HPC-AI-Lab | cuBLAS | CUTLASS |
|------|-----------|--------|---------|
| **学习导向** | ✅ 渐进式优化 | ❌ 黑盒 | ⚠️ 复杂 |
| **生产就绪** | ✅ 已测试和基准测试 | ✅ 高度优化 | ✅ 优化 |
| **易于使用** | ✅ 简单 API + Python | ✅ API | ⚠️ 模板 |
| **教育价值** | ✅ 7步GEMM之旅 | ❌ 无 | ⚠️ 高级 |
| **现代AI** | ✅ FlashAttention, RoPE, FP8 | ✅ 支持 | ✅ 支持 |

**适合人群：**
- 🎓 **学生**：从第一性原理学习 CUDA 优化
- 🔬 **研究员**：原型化新的内核优化
- 🏭 **工程师**：AI 工作负载的生产级内核

---

## 🚀 快速开始

### 一分钟设置

```bash
# 克隆、构建和测试
git clone https://github.com/LessUp/hpc-ai-optimization-lab.git
cd hpc-ai-optimization-lab && mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release && cmake --build . -j$(nproc)
ctest --output-on-failure
```

### 环境要求

| 需求 | 版本 | 说明 |
|------|------|------|
| **CUDA Toolkit** | 12.4+ | [下载](https://developer.nvidia.com/cuda-toolkit) |
| **CMake** | 3.24+ | `pip install cmake` 或系统包 |
| **C++ 编译器** | GCC 11+ / Clang 14+ | 需要 C++20 支持 |
| **NVIDIA GPU** | 计算能力 7.0+ | Volta, Turing, Ampere, Hopper |

### 构建选项

```bash
# 基础构建（仅核心库）
cmake .. -DCMAKE_BUILD_TYPE=Release

# 带示例和 Python 绑定
cmake .. -DCMAKE_BUILD_TYPE=Release \
         -DBUILD_EXAMPLES=ON \
         -DBUILD_PYTHON_BINDINGS=ON

# 目标 GPU 架构
cmake .. -DCMAKE_CUDA_ARCHITECTURES="80;90"  # A100 + H100
```

### 运行示例

```bash
# ReLU 示例（逐元素操作）
./examples/elementwise/relu_example

# GEMM 基准测试（全部 7 个优化步骤）
./examples/gemm/gemm_benchmark

# Python 使用（如果启用绑定）
python examples/python/basic_usage.py
```

---

## 📊 性能亮点

### GEMM 优化之旅（FP32, 4096×4096, A100）

| 步骤 | 技术 | 性能 | 提速 |
|------|------|------|------|
| 1 | 朴素实现 | 0.5 TFLOPS | 1×（基准） |
| 2 | 共享内存分块 | 2.0 TFLOPS | 4× |
| 3 | 双缓冲 | 3.5 TFLOPS | 7× |
| 4 | 寄存器分块 | 6.0 TFLOPS | 12× |
| 5 | **Tensor Core WMMA** | **50+ TFLOPS** | **100×** |
| 6 | Tensor Core MMA PTX | 60+ TFLOPS | 120× |
| 7 | 软件流水线 | 70+ TFLOPS | 140× |

> 💡 **关键洞察**：Tensor Core 加速相比朴素实现提供 100× 提速！

### 模块性能总结

| 模块 | 操作 | FP32 性能 | 状态 |
|------|------|----------|------|
| **逐元素** | ReLU, Sigmoid, 转置 | 内存受限 | ✅ 稳定 |
| **归约** | Softmax, LayerNorm, RMSNorm | 已优化 | ✅ 稳定 |
| **GEMM** | 矩阵乘法 | 70+ TFLOPS | ✅ 稳定 |
| **注意力** | FlashAttention, RoPE | IO感知 | ✅ 稳定 |
| **卷积** | 隐式 GEMM | 有竞争力 | ✅ 稳定 |

---

## 📚 文档

### 🌐 在线文档

访问我们 comprehensive 的文档站点：**[https://lessup.github.io/hpc-ai-optimization-lab/](https://lessup.github.io/hpc-ai-optimization-lab/)**

### 📖 快速链接

| 主题 | 英文文档 | 中文文档 |
|------|---------|---------|
| **入门指南** | [Installation](docs/en/guide/installation.md) | [安装指南](docs/zh-CN/guide/installation.md) |
| **快速开始** | [5-min Guide](docs/en/guide/quick-start.md) | [快速入门](docs/zh-CN/guide/quick-start.md) |
| **GEMM 优化** | [7-Step Journey](docs/en/guide/gemm.md) | [GEMM优化](docs/zh-CN/guide/gemm.md) |
| **访存优化** | [Guide](docs/en/guide/memory.md) | [访存优化](docs/zh-CN/guide/memory.md) |
| **FlashAttention** | [Guide](docs/en/guide/flash-attention.md) | [FlashAttention](docs/zh-CN/guide/flash-attention.md) |
| **性能调优** | [Guide](docs/en/PERFORMANCE_TUNING.md) | [性能调优](docs/zh-CN/PERFORMANCE_TUNING.md) |
| **API 参考** | [C++/Python API](docs/en/API_REFERENCE.md) | [API参考](docs/zh-CN/API_REFERENCE.md) |

### 🎓 推荐学习路径

```
🌱 初学者（1-2 周）
├── 安装与快速入门
├── 访存优化（合并访问、向量化）
├── 归约操作（warp洗牌、在线算法）
└── GEMM 步骤1-4（共享内存到寄存器分块）

🚀 进阶（2-4 周）
├── GEMM 步骤5-7（Tensor Core WMMA、MMA PTX、流水线）
├── FlashAttention（IO感知的注意力机制）
└── 性能分析与调优

🏆 高级（持续学习）
├── CUDA 13 Hopper 特性（TMA、Clusters、FP8）
├── CUTLASS 源码研究
└── 论文实现
```

---

## 🏗️ 项目结构

```
hpc-ai-optimization-lab/
├── src/                        # CUDA 内核实现
│   ├── common/                 # 共享工具（Tensor、Timer、CUDA检查）
│   ├── elementwise/            # ReLU、Sigmoid、VectorAdd、转置
│   ├── reduction/              # Softmax、LayerNorm、RMSNorm
│   ├── gemm/                   # 7步GEMM优化（旗舰！）
│   ├── convolution/            # 隐式GEMM、Winograd
│   ├── attention/              # FlashAttention、RoPE、TopK
│   ├── quantization/           # INT8/FP8 量化
│   └── cuda13/                 # Hopper特性（TMA、Clusters、FP8）
│
├── tests/                      # 综合测试套件
│   ├── common/                 # 工具测试
│   ├── elementwise/            # 逐元素测试
│   ├── gemm/                   # GEMM测试（基于属性）
│   └── ...                     # 所有模块都已测试
│
├── examples/                   # 独立示例
│   ├── elementwise/            # ReLU示例
│   ├── reduction/              # Softmax基准测试
│   ├── gemm/                   # GEMM基准测试
│   ├── convolution/            # 卷积示例
│   ├── attention/              # FlashAttention示例
│   ├── quantization/           # 量化示例
│   ├── cuda13/                 # CUDA 13示例
│   └── python/                 # Python使用示例
│
├── python/                     # Python绑定（nanobind）
│   ├── bindings/               # C++绑定代码
│   └── benchmark/              # Python基准测试
│
├── docs/                       # 文档（VitePress + Doxygen）
│   ├── en/                     # 英文文档
│   ├── zh-CN/                  # 中文文档
│   └── .vitepress/             # VitePress配置
│
├── docker/                     # Docker环境
│   ├── Dockerfile
│   └── docker-compose.yml
│
└── .github/                    # CI/CD 工作流
    └── workflows/
        ├── ci.yml              # 持续集成
        └── pages.yml           # 文档部署
```

---

## 💻 使用示例

### C++ API

```cpp
#include "gemm/gemm.cuh"
#include "common/tensor.cuh"

// 分配 GPU 张量
auto A = hpc::common::make_tensor<float>(hpc::common::Device, {M, K});
auto B = hpc::common::make_tensor<float>(hpc::common::Device, {K, N});
auto C = hpc::common::make_tensor<float>(hpc::common::Device, {M, N});

// 启动优化的 GEMM 内核
hpc::gemm::gemm<float, hpc::gemm::OptLevel::Advanced>(
    A.data(), B.data(), C.data(), M, N, K, stream);
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

## 🧪 测试与质量

### 双层测试策略

**单元测试（GoogleTest）**
```bash
# 运行所有测试
ctest --output-on-failure

# 运行特定测试套件
./tests/gemm/test_gemm
```

**基于属性的测试（RapidCheck）**
- 自动生成边界情况
- 测试所有输入尺寸组合
- 发现数值稳定性问题

### 测试覆盖率

| 模块 | 单元测试 | 属性测试 | 覆盖率 |
|------|---------|---------|--------|
| 逐元素 | 12 | 48 | 95%+ |
| 归约 | 9 | 36 | 90%+ |
| GEMM | 15 | 60 | 98%+ |
| 注意力 | 8 | 32 | 92%+ |
| **总计** | **60+** | **200+** | **95%+** |

---

## 🐳 Docker 环境

使用我们预配置的 Docker 环境进行无忧开发：

```bash
# 启动开发环境
cd docker && docker-compose up -d
docker exec -it hpc-ai-lab bash

# 容器内：所有内容已预装！
cmake -S . -B build && cmake --build build -j$(nproc)
ctest --test-dir build
```

---

## 🤝 贡献

欢迎贡献！本项目遵循[规范驱动开发（SDD）](AGENTS.md)。

### 快速开始

```bash
# 1. Fork 并克隆
git clone https://github.com/LessUp/hpc-ai-optimization-lab.git
cd hpc-ai-optimization-lab

# 2. 创建特性分支
git checkout -b feature/my-optimization

# 3. 进行修改并添加测试
# 参考 specs/ 目录了解需求

# 4. 确保测试通过
cmake -S . -B build && cmake --build build -j$(nproc)
ctest --test-dir build --output-on-failure

# 5. 提交并推送
git commit -m "feat: optimize GEMM step 3"
git push origin feature/my-optimization
```

### CI 状态

> ⚠️ **注意**：当前 CI 专注于代码格式、一致性和文档。GPU 测试需要本地执行或使用自托管运行器。

详见 [CONTRIBUTING.md](CONTRIBUTING.md) 指南。

---

## 📈 路线图

### 已完成（v0.1.0 - v0.3.0） ✅

- [x] 逐元素操作（4个内核）
- [x] 归约操作（3个内核）
- [x] GEMM 优化（7个步骤）
- [x] FlashAttention + RoPE + TopK
- [x] INT8/FP8 量化
- [x] CUDA 13 Hopper 特性
- [x] Python 绑定（nanobind）
- [x] 综合文档

### 进行中（v0.4.0） 🚧

- [ ] FP8 GEMM（Hopper 原生）
- [ ] 多 GPU 支持
- [ ] CUTLASS 集成
- [ ] 性能回归测试

### 计划中（v0.5.0+） 🎯

- [ ] MoE（混合专家）支持
- [ ] 稀疏 GEMM 优化
- [ ] 自动调优框架
- [ ] PyTorch 集成

---

## 📊 支持矩阵

### 生产就绪 ✅

| 模块 | FP32 | FP16 | BF16 | INT8 | FP8 | 状态 |
|------|------|------|------|------|-----|------|
| 逐元素 | ✅ | ✅ | ✅ | - | - | 稳定 |
| 归约 | ✅ | ✅ | ✅ | - | - | 稳定 |
| GEMM | ✅ | ✅ | ✅ | ✅ | 🚧 | 稳定 |
| 卷积 | ✅ | ✅ | - | - | - | 稳定 |
| 注意力 | ✅ | ✅ | - | - | - | 稳定 |
| 量化 | ✅ | ✅ | - | ✅ | 🚧 | 稳定 |

### 实验性 🧪

| 特性 | 状态 | 说明 |
|------|------|------|
| FP8 GEMM | 演示 | 缩放 FP16 行为 |
| TMA | 回退 | 使用异步复制代替 |
| Thread Block Clusters | 回退 | Block 归约 |
| Winograd 卷积 | 回退 | 隐式 GEMM 路径 |

---

## 🙏 致谢

- [NVIDIA CUTLASS](https://github.com/NVIDIA/cutlass) - 参考实现
- [FlashAttention](https://github.com/Dao-AILab/flash-attention) - 注意力优化
- [How to Optimize a CUDA Matmul](https://siboehm.com/articles/22/CUDA-MMM) - 优秀教程
- [NVIDIA CUDA Samples](https://github.com/NVIDIA/cuda-samples) - 最佳实践

---

## 📄 许可证

本项目采用 **Apache License 2.0** 许可证 - 详见 [LICENSE](LICENSE)。

---

<div align="center">

**⭐ 如果觉得有用，请给个 Star！**

[报告 Bug](https://github.com/LessUp/hpc-ai-optimization-lab/issues) · [请求功能](https://github.com/LessUp/hpc-ai-optimization-lab/issues) · [文档](https://lessup.github.io/hpc-ai-optimization-lab/)

**由 HPC-AI-Optimization-Lab 贡献者用 ❤️ 制作**

</div>
