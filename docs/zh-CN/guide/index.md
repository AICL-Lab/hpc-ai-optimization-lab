# 入门指南

欢迎来到 **HPC-AI-Optimization-Lab** —— 面向高性能 CUDA 内核的综合教育实验室。

## 您将学到什么

本文档涵盖 CUDA 内核优化的完整范围，从基础内存访问模式到前沿张量核心使用：

- **访存优化** - 合并访问、向量化、共享内存模式
- **归约操作** - 线程束洗牌、块级归约、在线算法
- **GEMM 优化** - 从朴素实现到面向 Tensor Core 的 7 步演进
- **FlashAttention** - 基于 IO 感知的注意力机制
- **CUDA 13 特性** - 实验性 Hopper 架构特性

## 前置要求

- CUDA Toolkit 12.4+
- CMake 3.24+
- C++20 兼容编译器
- NVIDIA GPU（计算能力 7.0+）

## 快速开始

### 克隆并构建

```bash
# 克隆仓库
git clone https://github.com/LessUp/hpc-ai-optimization-lab.git
cd hpc-ai-optimization-lab

# 配置并构建
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j$(nproc)

# 运行测试
ctest --test-dir build --output-on-failure
```

### 构建示例

```bash
cmake -S . -B build -DBUILD_EXAMPLES=ON
cmake --build build --target relu_example gemm_benchmark
./build/examples/relu_example
./build/examples/gemm_benchmark
```

### Python 绑定

```bash
cmake -S . -B build -DBUILD_PYTHON_BINDINGS=ON
cmake --build build
export PYTHONPATH="$(pwd)/build/python:$PYTHONPATH"
python examples/python/basic_usage.py
```

## 文档结构

| 章节 | 描述 | 难度 |
|-----|------|------|
| [访存优化](memory) | 合并访问、向量化、共享内存 | ⭐⭐ |
| [归约优化](reduction) | 线程束洗牌、在线 Softmax、LayerNorm | ⭐⭐⭐ |
| [GEMM 优化](gemm) | 7 步矩阵乘法之旅 | ⭐⭐⭐⭐ |
| [FlashAttention](flash-attention) | 基于 IO 感知的注意力机制 | ⭐⭐⭐⭐ |
| [CUDA 13 特性](cuda13) | Hopper 架构：TMA、集群、FP8 | ⭐⭐⭐⭐⭐ |

## API 参考

- [API 参考](/zh-CN/api/index) - 聚合后的 C++ / CUDA / Python 参考说明
- [架构概览](/zh-CN/api/architecture) - 设计模式与模块组织

## 下一步

选择您的学习路径：

- **初学者** → 从 [访存优化](memory) 开始
- **进阶** → 跳转到 [GEMM 优化](gemm)
- **专家** → 探索 [FlashAttention](flash-attention) 或 [CUDA 13 特性](cuda13)
