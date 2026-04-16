# 📚 HPC-AI-Optimization-Lab Documentation

Welcome to the comprehensive documentation for HPC-AI-Optimization-Lab! This documentation provides in-depth guides for CUDA kernel optimization techniques used in AI workloads.

---

## 🌍 Language / 语言

**English** | [简体中文](../zh-CN/README.md)

---

## 🚀 Quick Navigation

### Getting Started

| Resource | Description |
|----------|-------------|
| [Main README](../../README.md) | Project overview, build instructions, quick start |
| [Architecture Overview](ARCHITECTURE.md) | Design patterns, module organization |
| [API Reference](API_REFERENCE.md) | Complete API documentation |
| [Examples](../../examples/README.md) | Runnable code examples |

### Technical Guides

| Document | Topic | Difficulty | Est. Time |
|----------|-------|------------|-----------|
| [Memory Optimization](02_memory_optimization.md) | Coalesced access, vectorization, shared memory | ⭐⭐ | 2-3 hrs |
| [GEMM Optimization](01_gemm_optimization.md) | 7-step matrix multiplication optimization | ⭐⭐⭐⭐ | 4-6 hrs |
| [Reduction Optimization](03_reduction_optimization.md) | Warp shuffle, online softmax, LayerNorm | ⭐⭐⭐ | 3-4 hrs |
| [FlashAttention](04_flash_attention.md) | IO-aware attention, tiling, online softmax | ⭐⭐⭐⭐ | 4-5 hrs |
| [CUDA 13 Features](05_cuda13_features.md) | Hopper architecture: TMA, Clusters, FP8 | ⭐⭐⭐⭐⭐ | 5-6 hrs |

---

## 📖 Learning Paths

### 🌱 Beginner Path (1-2 weeks)

**Goal**: Understand CUDA memory model and basic optimizations

```
Step 1: Memory Optimization
        └── Coalesced access patterns
        └── Vectorized loads (float4)
        └── Grid stride loops
        
Step 2: Reduction Operations  
        └── Warp shuffle primitives
        └── Block-level reduction
        └── Online Softmax
        
Step 3: GEMM Basics
        └── Naive implementation
        └── Shared memory tiling (Steps 1-2)
```

**Recommended Reading Order**:
1. [Memory Optimization](02_memory_optimization.md)
2. [Reduction Optimization](03_reduction_optimization.md)
3. [GEMM Optimization](01_gemm_optimization.md) (Steps 1-4)

### 🔥 Intermediate Path (2-4 weeks)

**Goal**: Master Tensor Core usage and advanced techniques

```
Step 1: GEMM Advanced
        └── Double buffering (Step 3)
        └── Register tiling (Step 4)
        └── Tensor Core WMMA (Step 5)
        
Step 2: FlashAttention
        └── IO complexity analysis
        └── Tiling strategy
        └── Online softmax integration
```

**Recommended Reading Order**:
1. [GEMM Optimization](01_gemm_optimization.md) (Steps 5-7)
2. [FlashAttention](04_flash_attention.md)

### 🚀 Expert Path (ongoing)

**Goal**: Cutting-edge techniques and research

```
Step 1: Hopper Architecture
        └── TMA (Tensor Memory Accelerator)
        └── Thread Block Clusters
        └── FP8 quantization
        
Step 2: Advanced Topics
        └── CUTLASS source code analysis
        └── Custom kernel development
```

**Recommended Reading Order**:
1. [CUDA 13 Features](05_cuda13_features.md)
2. External: [CUTLASS Documentation](https://github.com/NVIDIA/cutlass)
3. External: [FlashAttention-2 Paper](https://arxiv.org/abs/2307.08691)

---

## 🛠️ Quick Reference

### Optimization Techniques Summary

| Technique | Use Case | Expected Speedup | Documentation |
|-----------|----------|------------------|---------------|
| Coalesced Access | All kernels | 2-10× | [Memory Optimization](02_memory_optimization.md) |
| Vectorization (float4) | Elementwise | 1.5-2× | [Memory Optimization](02_memory_optimization.md) |
| Grid Stride Loop | Large inputs | 1.2-1.5× | [Memory Optimization](02_memory_optimization.md) |
| Shared Memory | Data reuse | 2-5× | [GEMM Optimization](01_gemm_optimization.md) |
| Warp Shuffle | Reductions | 5-10× | [Reduction Optimization](03_reduction_optimization.md) |
| Tensor Core | Matrix ops | 10-20× | [GEMM Optimization](01_gemm_optimization.md) |
| TMA | Large copies | 1.5-2× | [CUDA 13 Features](05_cuda13_features.md) |
| Online Softmax | Attention | 3-5× | [FlashAttention](04_flash_attention.md) |

### Performance Profiling Commands

```bash
# Nsight Compute - Kernel-level analysis
ncu --set full -o profile ./your_app

# Nsight Systems - System timeline
nsys profile -o timeline ./your_app

# Roofline analysis
ncu --set roofline -o roofline ./your_app

# Memory throughput analysis
ncu --metrics gpu__time_duration.sum,dram__throughput.avg.pct_of_peak_sustained_elapsed \
    ./your_app
```

### Common Issues & Solutions

| Issue | Cause | Solution | Reference |
|-------|-------|----------|-----------|
| Low occupancy | Too many registers | Reduce register usage, use shared memory | Memory Opt §4 |
| Bank conflicts | Shared memory access pattern | Add padding | Memory Opt §4 |
| Poor bandwidth | Non-coalesced access | Restructure memory access | Memory Opt §1 |
| Low Tensor Core util | Dimensions not multiple of 16 | Pad matrices | GEMM Opt §5 |
| Numerical instability | FP precision issues | Use Welford's algorithm | Reduction Opt §5 |

---

## 🏗️ Building Documentation

### Prerequisites

```bash
# Doxygen - C++/CUDA API documentation
sudo apt-get install doxygen graphviz

# Sphinx - Python API documentation
pip install sphinx sphinx-rtd-theme breathe myst-parser sphinx-copybutton
```

### Generate Documentation

```bash
# C++/CUDA API documentation
doxygen Doxyfile
# Output: docs/api/html/

# Python API documentation
cd python
sphinx-build -b html . _build/html
# Output: python/_build/html/
```

### Preview Locally

```bash
# C++ API docs
python -m http.server 8000 -d api/html

# Python API docs  
python -m http.server 8001 -d python/_build/html
```

---

## 📚 External Resources

### Official NVIDIA Documentation

- [CUDA C++ Programming Guide](https://docs.nvidia.com/cuda/cuda-c-programming-guide/)
- [CUDA C++ Best Practices Guide](https://docs.nvidia.com/cuda/cuda-c-best-practices-guide/)
- [CUDA Optimization Guide](https://docs.nvidia.com/cuda/cuda-optimization-guide/)
- [CUTLASS Documentation](https://github.com/NVIDIA/cutlass/tree/main/media/docs)

### Foundational Papers

| Paper | Topic | Link |
|-------|-------|------|
| FlashAttention | IO-aware attention | [arXiv:2205.14135](https://arxiv.org/abs/2205.14135) |
| FlashAttention-2 | Parallelism improvements | [arXiv:2307.08691](https://arxiv.org/abs/2307.08691) |
| Online Softmax | Single-pass softmax | [arXiv:1805.02867](https://arxiv.org/abs/1805.02867) |

### Recommended Blogs & Tutorials

- [How to Optimize a CUDA Matmul Kernel](https://siboehm.com/articles/22/CUDA-MMM) - Step-by-step GEMM optimization
- [NVIDIA Developer Blog](https://developer.nvidia.com/blog/) - Official tutorials
- [Mark Harris' Optimizing CUDA](https://developer.download.nvidia.com/assets/cuda/files/reduction.pdf) - Classic reduction

---

## 🤝 Contributing to Documentation

We welcome documentation improvements! When contributing:

1. **Format**: Use Markdown with proper headings
2. **Code Examples**: Include compilable, runnable snippets
3. **Performance Data**: Add benchmark numbers where applicable
4. **References**: Cite relevant papers and resources

### Documentation Style Guide

```markdown
# Section Title

Brief introduction paragraph.

## Subsection

### Code Example

```cpp
// Always include necessary headers
#include "03_gemm/gemm.cuh"

int matrix_size = 1024;  // Use meaningful names
```

### Performance Table

| Implementation | Time (ms) | Bandwidth (GB/s) |
|----------------|-----------|------------------|
| Naive | 10.0 | 50 |
| Optimized | 2.0 | 250 |
```

---

## 📞 Getting Help

- **Documentation Issues**: Open an issue with `[docs]` label
- **Code Questions**: Check existing issues or create a new one
- **Feature Requests**: Use the feature request template

---

<div align="center">

**Happy Learning! 🚀**

If you find this documentation helpful, please consider ⭐ starring the repository!

[Documentation Portal](https://lessup.github.io/hpc-ai-optimization-lab) · 
[Report Bug](https://github.com/LessUp/hpc-ai-optimization-lab/issues) · 
[Request Feature](https://github.com/LessUp/hpc-ai-optimization-lab/issues)

</div>
