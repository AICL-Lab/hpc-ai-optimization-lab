---
title: 参考文献与延伸阅读
description: CUDA 内核优化的学术论文、开源项目和学习资源
---

# 参考文献与延伸阅读

本页面收集了影响本项目设计和实现的关键资源。

## 📄 学术论文

### GEMM 与 Tensor Cores

- **[CUTLASS: Fast Linear Algebra in CUDA C++](https://arxiv.org/abs/1809.00337)** (2017)
  - CUTLASS 的基础论文，NVIDIA 的 GEMM 模板库。
  - 引入关键概念：线程块分块、warp 级矩阵乘法、尾声融合。

- **[Programming Tensor Cores](https://developer.nvidia.com/blog/programming-tensor-cores/)** (NVIDIA 博客)
  - 在 CUDA 中编程 Tensor Cores 的官方指南。
  - 涵盖 WMMA API 和内联 PTX 以获得最大性能。

- **[A Work-Efficient Parallel Scan](https://developer.nvidia.com/gpugems/GPUGems3/gpugems3_ch39.pdf)** (GPU Gems 3)
  - 并行前缀和（scan）算法的经典参考。
  - 许多归约操作的基础。

### FlashAttention

- **[FlashAttention: Fast and Memory-Efficient Exact Attention](https://arxiv.org/abs/2205.14135)** (2022)
  - Dao 等人的突破性注意力算法。
  - 引入分块策略将内存访问从 O(N²) 减少到 O(N)。

- **[FlashAttention-2: Faster Attention with Better Parallelism and Work Partitioning](https://arxiv.org/abs/2307.08691)** (2023)
  - 改进版本，更好的 GPU 利用率。
  - 关键优化：减少非矩阵乘 FLOPs，更好的工作分区。

### 量化

- **[FP8 Formats for Deep Learning](https://arxiv.org/abs/2209.05433)** (2022)
  - NVIDIA、Intel、ARM 关于 FP8 格式的联合论文。
  - 定义了现代 GPU 中使用的 E4M3 和 E5M2 格式。

## 🛠️ 开源项目

### NVIDIA 库

| 项目 | 描述 | 链接 |
|------|------|------|
| **CUTLASS** | CUDA 线性代数模板 | [github.com/NVIDIA/cutlass](https://github.com/NVIDIA/cutlass) |
| **TensorRT** | 高性能推理优化器 | [developer.nvidia.com/tensorrt](https://developer.nvidia.com/tensorrt) |
| **cuBLAS** | CUDA 基础线性代数 | [docs.nvidia.com/cuda/cublas](https://docs.nvidia.com/cuda/cublas/) |

### 社区项目

| 项目 | 描述 | 链接 |
|------|------|------|
| **FlashAttention** | 官方实现 | [github.com/Dao-AILab/flash-attention](https://github.com/Dao-AILab/flash-attention) |
| **vLLM** | 高吞吐量 LLM 服务 | [github.com/vllm-project/vllm](https://github.com/vllm-project/vllm) |
| **llama.cpp** | LLaMA 的 CPU/GPU 推理 | [github.com/ggerganov/llama.cpp](https://github.com/ggerganov/llama.cpp) |
| **TensorRT-LLM** | NVIDIA 的 LLM 推理库 | [github.com/NVIDIA/TensorRT-LLM](https://github.com/NVIDIA/TensorRT-LLM) |

## 📚 教程与学习资源

### NVIDIA 文档

- **[CUDA C++ 编程指南](https://docs.nvidia.com/cuda/cuda-c-programming-guide/)**
  - 官方 CUDA 编程参考。
  - 涵盖内存模型、执行模型、同步。

- **[CUDA C++ 最佳实践指南](https://docs.nvidia.com/cuda/cuda-c-best-practices-guide/)**
  - 优化指南和常见陷阱。
  - 编写生产内核前的必读材料。

- **[分析器用户指南](https://docs.nvidia.com/cuda/profiler-users-guide/)**
  - Nsight Systems 和 Nsight Compute 使用方法。
  - 如何分析和优化内核性能。

### 教育资源

- **[GPU Mode](https://github.com/gpu-mode/gpu-mode)** - GPU 编程讲座系列
- **[CUDA Mode](https://github.com/cudai/cudai)** - CUDA 学习资源
- **[Mark Harris' GPU Gems](https://developer.nvidia.com/gpugems/GPUGems3/gpugems3_part01.html)** - 经典 GPU 优化技术

## 📊 性能基准

### 参考数据

| 来源 | 描述 |
|------|------|
| [NVIDIA A100 数据手册](https://www.nvidia.com/content/dam/nvidia/en-us/solutions/data-center/products/a100/pdf/a100-datasheet.pdf) | 官方 A100 规格（TFLOPS、内存带宽） |
| [cuBLAS 性能](https://docs.nvidia.com/cuda/cublas/#performance) | 参考 GEMM 性能数据 |
| [TensorRT 基准](https://developer.nvidia.com/deep-learning-performance-training-inference) | NVIDIA 官方推理基准 |

### 社区基准

- **[Stanford DAWN 深度学习基准](https://dawn.cs.stanford.edu/benchmark/)**
- **[MLPerf 推理结果](https://mlcommons.org/benchmarks/inference/)**

## 📖 如何引用

如果你在研究中使用本项目或觉得有帮助，请引用：

```bibtex
@software{hpc_ai_optimization_lab,
  title = {HPC-AI-Optimization-Lab: CUDA Kernel Optimization Learning Platform},
  author = {LessUp},
  year = {2024},
  url = {https://github.com/LessUp/hpc-ai-optimization-lab},
  version = {0.3.0},
  note = {渐进式 GEMM、FlashAttention 和量化实现}
}
```

---

## 🔗 快速链接

- [GitHub 仓库](https://github.com/LessUp/hpc-ai-optimization-lab)
- [文档站点](https://lessup.github.io/hpc-ai-optimization-lab/)
- [问题追踪](https://github.com/LessUp/hpc-ai-optimization-lab/issues)
