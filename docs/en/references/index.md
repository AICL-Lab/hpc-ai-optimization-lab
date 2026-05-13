---
title: References & Further Reading
description: Academic papers, open source projects, and learning resources for CUDA kernel optimization
---

# References & Further Reading

This page collects the key resources that informed this project's design and implementation.

## 📄 Academic Papers

### GEMM & Tensor Cores

- **[CUTLASS: Fast Linear Algebra in CUDA C++](https://arxiv.org/abs/1809.00337)** (2017)
  - The foundational paper for CUTLASS, NVIDIA's template library for GEMM.
  - Introduces key concepts: thread block tiles, warp-level matrix multiply, epilogue fusion.

- **[Programming Tensor Cores](https://developer.nvidia.com/blog/programming-tensor-cores/)** (NVIDIA Blog)
  - Official guide to programming Tensor Cores in CUDA.
  - Covers WMMA API and inline PTX for maximum performance.

- **[A Work-Efficient Parallel Scan](https://developer.nvidia.com/gpugems/GPUGems3/gpugems3_ch39.pdf)** (GPU Gems 3)
  - Classic reference for parallel prefix sum (scan) algorithms.
  - Foundation for many reduction operations.

### FlashAttention

- **[FlashAttention: Fast and Memory-Efficient Exact Attention](https://arxiv.org/abs/2205.14135)** (2022)
  - Dao et al.'s breakthrough attention algorithm.
  - Introduces tiling strategy to reduce memory access from O(N²) to O(N).

- **[FlashAttention-2: Faster Attention with Better Parallelism and Work Partitioning](https://arxiv.org/abs/2307.08691)** (2023)
  - Improved version with better GPU utilization.
  - Key optimizations: reduced non-matmul FLOPs, better work partitioning.

### Quantization

- **[FP8 Formats for Deep Learning](https://arxiv.org/abs/2209.05433)** (2022)
  - NVIDIA, Intel, ARM joint paper on FP8 formats.
  - Defines E4M3 and E5M2 formats used in modern GPUs.

## 🛠️ Open Source Projects

### NVIDIA Libraries

| Project | Description | Link |
|---------|-------------|------|
| **CUTLASS** | CUDA Templates for Linear Algebra | [github.com/NVIDIA/cutlass](https://github.com/NVIDIA/cutlass) |
| **TensorRT** | High-performance inference optimizer | [developer.nvidia.com/tensorrt](https://developer.nvidia.com/tensorrt) |
| **cuBLAS** | CUDA Basic Linear Algebra | [docs.nvidia.com/cuda/cublas](https://docs.nvidia.com/cuda/cublas/) |

### Community Projects

| Project | Description | Link |
|---------|-------------|------|
| **FlashAttention** | Official implementation | [github.com/Dao-AILab/flash-attention](https://github.com/Dao-AILab/flash-attention) |
| **vLLM** | High-throughput LLM serving | [github.com/vllm-project/vllm](https://github.com/vllm-project/vllm) |
| **llama.cpp** | CPU/GPU inference for LLaMA | [github.com/ggerganov/llama.cpp](https://github.com/ggerganov/llama.cpp) |
| **TensorRT-LLM** | NVIDIA's LLM inference library | [github.com/NVIDIA/TensorRT-LLM](https://github.com/NVIDIA/TensorRT-LLM) |

## 📚 Tutorials & Learning Resources

### NVIDIA Documentation

- **[CUDA C++ Programming Guide](https://docs.nvidia.com/cuda/cuda-c-programming-guide/)**
  - Official CUDA programming reference.
  - Covers memory model, execution model, synchronization.

- **[CUDA C++ Best Practices Guide](https://docs.nvidia.com/cuda/cuda-c-best-practices-guide/)**
  - Optimization guidelines and common pitfalls.
  - Essential reading before writing production kernels.

- **[Profiler User's Guide](https://docs.nvidia.com/cuda/profiler-users-guide/)**
  - Nsight Systems and Nsight Compute usage.
  - How to profile and optimize kernel performance.

### Educational Resources

- **[GPU Mode](https://github.com/gpu-mode/gpu-mode)** - Lecture series on GPU programming
- **[CUDA Mode](https://github.com/cudai/cudai)** - CUDA learning resources
- **[Mark Harris' GPU Gems](https://developer.nvidia.com/gpugems/GPUGems3/gpugems3_part01.html)** - Classic GPU optimization techniques

## 📊 Performance Benchmarks

### Reference Data

| Source | Description |
|--------|-------------|
| [NVIDIA A100 Datasheet](https://www.nvidia.com/content/dam/nvidia/en-us/solutions/data-center/products/a100/pdf/a100-datasheet.pdf) | Official A100 specifications (TFLOPS, memory bandwidth) |
| [cuBLAS Performance](https://docs.nvidia.com/cuda/cublas/#performance) | Reference GEMM performance numbers |
| [TensorRT Benchmarks](https://developer.nvidia.com/deep-learning-performance-training-inference) | NVIDIA's official inference benchmarks |

### Community Benchmarks

- **[Stanford DAWN Deep Learning Benchmarks](https://dawn.cs.stanford.edu/benchmark/)**
- **[MLPerf Inference Results](https://mlcommons.org/benchmarks/inference/)**

## 📖 How to Cite

If you use this project in your research or find it helpful, please cite:

```bibtex
@software{hpc_ai_optimization_lab,
  title = {HPC-AI-Optimization-Lab: CUDA Kernel Optimization Learning Platform},
  author = {LessUp},
  year = {2024},
  url = {https://github.com/LessUp/hpc-ai-optimization-lab},
  version = {0.3.0},
  note = {Progressive GEMM, FlashAttention, and quantization implementations}
}
```

Or in APA format:

> LessUp. (2024). HPC-AI-Optimization-Lab: CUDA Kernel Optimization Learning Platform (Version 0.3.0). https://github.com/LessUp/hpc-ai-optimization-lab

---

## 🔗 Quick Links

- [GitHub Repository](https://github.com/LessUp/hpc-ai-optimization-lab)
- [Documentation](https://lessup.github.io/hpc-ai-optimization-lab/)
- [Issue Tracker](https://github.com/LessUp/hpc-ai-optimization-lab/issues)
