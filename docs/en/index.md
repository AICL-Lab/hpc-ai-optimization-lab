---
layout: home

hero:
  name: HPC-AI-Optimization-Lab
  text: CUDA Kernel Optimization Lab
  tagline: A comprehensive educational laboratory for high-performance GPU kernels. From memory coalescing to Tensor Cores, from naive implementations to production-ready FlashAttention.
  image:
    src: /logo-large.svg
    alt: HPC-AI-Optimization-Lab Logo
  actions:
    - theme: brand
      text: Get Started
      link: /en/guide/
    - theme: alt
      text: View on GitHub
      link: https://github.com/LessUp/hpc-ai-optimization-lab

features:
  - icon: 🔢
    title: Elementwise Operations
    details: ReLU, Sigmoid, Transpose. Learn coalesced access, vectorization, and bank conflict avoidance. The foundation of GPU programming.
  - icon: 📊
    title: Reduction Operations
    details: Softmax, LayerNorm, RMSNorm. Master warp shuffle, block reduction, and online algorithms. Essential for attention mechanisms.
  - icon: 🧮
    title: GEMM Optimization
    details: 7-step matrix multiplication journey. From naive (0.5 TFLOPS) to Tensor Core (70+ TFLOPS). 140x speedup potential.
  - icon: 🖼️
    title: Convolution
    details: Implicit GEMM convolution implementation. Understand how convolutions map to matrix multiplication for optimal performance.
  - icon: 🎯
    title: FlashAttention
    details: IO-aware attention mechanism with online softmax. Reduce HBM traffic from O(N²) to O(N). State-of-the-art transformer optimization.
  - icon: 🚀
    title: CUDA 13 Features
    details: Experimental TMA, Thread Block Clusters, FP8 GEMM. Cutting-edge Hopper architecture features with fallback implementations.
---

<script setup>
import HomeHero from '/.vitepress/theme/components/HomeHero.vue'
import HomeFeatures from '/.vitepress/theme/components/HomeFeatures.vue'
</script>

<HomeHero />

## 🏠 Welcome to HPC-AI-Optimization-Lab

This documentation site provides comprehensive guides for CUDA kernel optimization techniques used in modern AI workloads. Whether you're a beginner learning GPU programming or an experienced developer optimizing production kernels, you'll find valuable insights here.

## 🎓 Learning Paths

### Beginner (Weeks 1-2)
Start with the fundamentals of GPU memory and parallel reduction:

1. **[Memory Optimization](/en/guide/memory)** - Understand coalesced access, vectorization (float4), shared memory patterns, and bank conflict avoidance.
2. **[Reduction Optimization](/en/guide/reduction)** - Learn warp shuffle, block-level reduction, online softmax, and Welford's algorithm.
3. **GEMM Steps 1-4** - Build a solid foundation with naive → shared memory tiling implementations.

### Intermediate (Weeks 2-4)
Master Tensor Core usage and attention mechanisms:

1. **GEMM Steps 5-7** - Implement Tensor Core WMMA/MMA, register tiling, and software pipelining.
2. **[FlashAttention](/en/guide/flash-attention)** - Understand IO-aware design, tiling strategies, and online softmax integration.

### Advanced (Ongoing)
Explore cutting-edge features and research:

1. **[CUDA 13 Features](/en/guide/cuda13)** - Experiment with TMA, Thread Block Clusters, and FP8 quantization.
2. **CUTLASS Deep Dive** - Study the reference implementation for production kernels.

## 📊 Performance Benchmarks

| Implementation | Speedup | TFLOPS (FP16) | Key Technique |
|---------------|---------|---------------|---------------|
| Naive GEMM | 1x | ~0.5 | Baseline |
| Shared Memory Tiling | 4x | ~2.0 | Tile size optimization |
| Double Buffering | 7x | ~3.5 | Memory latency hiding |
| Register Tiling | 12x | ~6.0 | Reduced shared memory traffic |
| Tensor Core WMMA | 100x | ~50 | Hardware acceleration |
| Tensor Core MMA PTX | 120x | ~60 | Fine-grained control |
| Software Pipeline | 140x | ~70+ | Multi-stage overlap |

## 🛠️ Quick Start

```bash
# Clone the repository
git clone https://github.com/LessUp/hpc-ai-optimization-lab.git
cd hpc-ai-optimization-lab

# Build with CMake
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j$(nproc)

# Run tests
ctest --test-dir build --output-on-failure

# Build with Python bindings
cmake -S . -B build -DBUILD_PYTHON_BINDINGS=ON
cmake --build build
export PYTHONPATH="$(pwd)/build/python:$PYTHONPATH"
```

## 📚 Documentation Structure

- **Guides**: In-depth tutorials covering each optimization topic
- **API Reference**: Auto-generated C++/CUDA and Python API docs
- **Examples**: Practical code examples for each module

<HomeFeatures />

## 🤝 Contributing

We welcome contributions! Please see our [GitHub repository](https://github.com/LessUp/hpc-ai-optimization-lab) for guidelines on submitting issues and pull requests.

## 📄 License

This project is licensed under the Apache 2.0 License.

---

<div style="text-align: center; margin-top: 4rem; padding: 2rem; background: var(--vp-c-bg-elv); border-radius: 16px;">
  <h3 style="margin-top: 0;">Start Your Optimization Journey</h3>
  <p>From naive kernels to Tensor Core mastery — level up your CUDA skills today.</p>
  <a href="/en/guide/" style="display: inline-flex; align-items: center; gap: 8px; padding: 14px 28px; background: linear-gradient(135deg, var(--vp-c-brand-1), var(--vp-c-brand-2)); color: #0b0f19; font-weight: 600; border-radius: 10px; text-decoration: none;">
    Get Started →
  </a>
</div>
