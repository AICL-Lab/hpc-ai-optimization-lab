# HPC-AI-Optimization-Lab

<div align="center">

**A Comprehensive CUDA Kernel Optimization Laboratory for AI Workloads**

[![CUDA](https://img.shields.io/badge/CUDA-12.4+-76B900?logo=nvidia)](https://developer.nvidia.com/cuda-toolkit)
[![C++20](https://img.shields.io/badge/C++-20-00599C?logo=cplusplus)](https://en.cppreference.com/w/cpp/20)
[![CMake](https://img.shields.io/badge/CMake-3.24+-064F8C?logo=cmake)](https://cmake.org/)
[![License](https://img.shields.io/badge/License-Apache%202.0-green)](LICENSE)

English | [简体中文](README.zh-CN.md)

</div>

---

## 🎯 Overview

HPC-AI-Optimization-Lab is an educational and production-ready CUDA kernel library designed for AI inference workloads. It provides step-by-step optimized implementations of critical GPU operations, from basic elementwise operations to advanced Tensor Core matrix multiplication.

### Key Features

- **📚 Progressive Learning Path**: Each module demonstrates optimization techniques from naive to expert-level
- **🔬 Production Quality**: All kernels include comprehensive test coverage (GoogleTest + RapidCheck)
- **🚀 Modern C++20**: Uses concepts, RAII, and contemporary design patterns
- **🐍 Python Bindings**: Optional nanobind-based Python interface for rapid prototyping

---

## 📁 Project Structure

```
hpc-ai-optimization-lab/
├── src/
│   ├── common/           # Shared utilities (Tensor, Timer, CUDA checks)
│   ├── elementwise/   # ReLU, Sigmoid, Vector Add, Transpose
│   ├── reduction/     # Softmax, LayerNorm, RMSNorm
│   ├── gemm/          # 7-step GEMM optimization journey
│   ├── convolution/   # Implicit GEMM, Winograd convolution
│   ├── attention/     # FlashAttention, RoPE, TopK
│   ├── quantization/  # INT8/FP8 quantization utilities
│   └── cuda13/ # Experimental Hopper architecture features
├── tests/                # Comprehensive test suite
├── examples/             # CUDA and Python examples
├── python/               # Nanobind Python bindings
└── docs/                 # Technical documentation
```

---

## 🚀 Quick Start

### Prerequisites

| Requirement | Version |
|-------------|---------|
| CUDA Toolkit | 12.4+ |
| CMake | 3.24+ |
| C++ Compiler | GCC 11+ / Clang 14+ / MSVC 2022+ |
| NVIDIA GPU | Compute Capability 7.0+ |

### Build

```bash
# Clone repository
git clone https://github.com/LessUp/hpc-ai-optimization-lab.git
cd hpc-ai-optimization-lab

# Configure and build
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j$(nproc)

# Run tests
ctest --test-dir build --output-on-failure
```

### Build with Examples

```bash
cmake -S . -B build -DBUILD_EXAMPLES=ON
cmake --build build --target relu_example gemm_benchmark

# Run examples
./build/examples/relu_example
./build/examples/gemm_benchmark
```

### Build Python Bindings

```bash
cmake -S . -B build -DBUILD_PYTHON_BINDINGS=ON
cmake --build build
export PYTHONPATH="$(pwd)/build/python:${PYTHONPATH}"

# Verify installation
python -c "import hpc_ai_opt; print('Module loaded successfully!')"

# Run Python example
python examples/python/basic_usage.py
```

---

## 📚 Module Overview

### 01 - Elementwise Operations

Optimizations covered:
- Naive implementation
- Vectorized loads/stores (`float4`)
- Grid stride loops for arbitrary input sizes
- Shared memory for transpose operations

```cpp
#include "elementwise/relu.cuh"

// Using optimized Grid Stride implementation
hpc::elementwise::relu<float, hpc::elementwise::OptLevel::GridStride>(
    d_input, d_output, n, stream);
```

### 02 - Reduction Operations

Optimizations covered:
- Warp shuffle primitives
- Block-level reduction
- Online Softmax algorithm
- Welford's algorithm for numerical stability

```cpp
#include "reduction/softmax.cuh"

// Online Softmax - single pass algorithm
hpc::reduction::softmax<float, hpc::reduction::SoftmaxOpt::OnlineSoftmax>(
    d_input, d_output, batch, seq_len, stream);
```

### 03 - GEMM (7-Step Optimization)

The flagship module demonstrating progressive GEMM optimization:

| Step | Technique | FP32 TFLOPS | Key Insight |
|------|-----------|-------------|-------------|
| 1 | Naive | ~0.5 | Baseline - each thread computes one element |
| 2 | Shared Memory Tiling | ~2.0 | Reduce global memory access by TILE_SIZE |
| 3 | Double Buffering | ~3.5 | Hide memory latency with computation overlap |
| 4 | Register Tiling | ~6.0 | Reduce shared memory bank conflicts |
| 5 | Tensor Core WMMA | ~50+ | Hardware-accelerated matrix operations |
| 6 | Tensor Core MMA PTX | ~60+ | Fine-grained Tensor Core control |
| 7 | Software Pipelining | ~70+ | Multi-stage execution overlap |

```cpp
#include "gemm/gemm.cuh"

// Using Tensor Core optimization
hpc::gemm::gemm<__half, hpc::gemm::GemmOpt::TensorCoreWMMA>(
    d_A, d_B, d_C, M, N, K, 1.0f, 0.0f, stream);
```

### 04 - Convolution

- Implicit GEMM convolution (validated, production-ready)
- Winograd convolution (3×3 kernels, experimental fallback)

### 05 - Attention

- FlashAttention forward pass with online softmax
- RoPE (Rotary Positional Embedding)
- MoE TopK routing

```cpp
#include "attention/flash_attention.cuh"

hpc::attention::FlashAttnConfig config{
    .batch_size = batch,
    .num_heads = heads,
    .seq_len = seq_len,
    .head_dim = 64,
    .scale = 1.0f / std::sqrt(64.0f),
    .causal = true
};

hpc::attention::flash_attention_forward<float>(
    d_Q, d_K, d_V, d_O, config, stream);
```

### 06 - Quantization

- INT8 per-row quantization/dequantization
- FP8 scaling utilities (placeholder for future Hopper support)

### 07 - CUDA 13 Features (Experimental)

> **Note**: These modules provide educational examples and fallback implementations. Full Hopper feature support requires SM 9.0+.

- TMA (Tensor Memory Accelerator) - async copy fallback
- Thread Block Clusters - portable reduction fallback
- FP8 GEMM - scaled FP16 demonstration

---

## 🧪 Testing

The project uses a two-tier testing strategy:

### Unit Tests (GoogleTest)

```bash
# Run all tests
ctest --test-dir build --output-on-failure

# Run specific test suite
./build/tests/gemm/test_gemm
```

### Property-Based Tests (RapidCheck)

Property-based tests automatically generate test cases to find edge cases:

```cpp
RC_GTEST_PROP(GemmTest, Correctness, ()) {
    auto M = *rc::gen::inRange<int>(1, 64);
    auto N = *rc::gen::inRange<int>(1, 64);
    auto K = *rc::gen::inRange<int>(1, 64);
    // ... automatically verifies correctness for all combinations
}
```

---

## 📖 Documentation

### Technical Guides

| Document | Description | Difficulty |
|----------|-------------|------------|
| [GEMM Optimization](docs/en/01_gemm_optimization.md) | 7-step matrix optimization journey | ⭐⭐⭐⭐ |
| [Memory Optimization](docs/en/02_memory_optimization.md) | Coalesced access, vectorization, shared memory | ⭐⭐ |
| [Reduction Optimization](docs/en/03_reduction_optimization.md) | Warp shuffle, online algorithms | ⭐⭐⭐ |
| [FlashAttention](docs/en/04_flash_attention.md) | IO-aware attention, tiling, online softmax | ⭐⭐⭐⭐ |
| [CUDA 13 Features](docs/en/05_cuda13_features.md) | Hopper architecture: TMA, Clusters, FP8 | ⭐⭐⭐⭐⭐ |
| [API Reference](docs/en/API_REFERENCE.md) | Complete C++/CUDA/Python API docs | ⭐⭐⭐ |
| [Architecture](docs/en/ARCHITECTURE.md) | Design patterns and module organization | ⭐⭐ |

### Recommended Learning Path

```
Beginner (1-2 weeks):
└── Memory Optimization → Reduction → GEMM (Steps 1-4)

Intermediate (2-4 weeks):
└── GEMM (Steps 5-7) → FlashAttention

Advanced (ongoing):
└── CUDA 13 Features → CUTLASS source code → Research papers
```

---

## 🔧 Configuration Options

| CMake Option | Default | Description |
|--------------|---------|-------------|
| `BUILD_EXAMPLES` | OFF | Build CUDA and Python examples |
| `BUILD_PYTHON_BINDINGS` | OFF | Build nanobind Python module |
| `CMAKE_CUDA_ARCHITECTURES` | native | Target GPU architecture(s) |

### Example Configurations

```bash
# Development build with all features
cmake -S . -B build \
    -DCMAKE_BUILD_TYPE=Debug \
    -DBUILD_EXAMPLES=ON \
    -DBUILD_PYTHON_BINDINGS=ON

# Release build for specific GPU
cmake -S . -B build \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_CUDA_ARCHITECTURES="80;90"  # A100 + H100
```

---

## 🐳 Docker Environment

```bash
cd docker
docker-compose up -d
docker exec -it hpc-ai-lab bash
```

---

## 🤝 Contributing

We welcome contributions! Please see [CONTRIBUTING.md](CONTRIBUTING.md) for guidelines.

> **CI Scope Note**: This repository does **not** currently provide full native CUDA build-and-test coverage in CI. The CI pipeline focuses on code formatting, consistency checks, and documentation builds. GPU-dependent tests require local execution or self-hosted runners.

### Quick Contribution Steps

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/amazing-feature`)
3. Make changes and add tests
4. Ensure tests pass locally (`ctest --test-dir build --output-on-failure`)
5. Commit with conventional commits (`git commit -m 'feat: add amazing feature'`)
6. Push and create a Pull Request

---

## 📄 License

This project is licensed under the Apache License 2.0 - see [LICENSE](LICENSE) for details.

---

## 🙏 Acknowledgments

- [NVIDIA CUTLASS](https://github.com/NVIDIA/cutlass) - Reference implementations
- [FlashAttention](https://github.com/Dao-AILab/flash-attention) - Attention optimization techniques
- [How to Optimize a CUDA Matmul Kernel](https://siboehm.com/articles/22/CUDA-MMM) - Excellent tutorial

---

## 📊 Support Matrix

### Stable / Production-Ready

| Module | FP32 | FP16 | INT8 | Notes |
|--------|------|------|------|-------|
| Elementwise | ✅ | ✅ | - | All optimization levels |
| Reduction | ✅ | ✅ | - | Online algorithms |
| GEMM | ✅ | ✅ | ✅ | 7-step progression |
| Convolution | ✅ | - | - | Implicit GEMM validated |
| Attention | ✅ | - | - | head_dim=64 only |
| Quantization | ✅ | - | ✅ | Per-row scaling |

### Experimental / Fallback

| Module | Status | Notes |
|--------|--------|-------|
| Winograd Conv | Fallback | Uses implicit GEMM path |
| TMA | Fallback | Uses async copy instead |
| Thread Block Clusters | Fallback | Uses block reduction |
| FP8 GEMM | Demo | Scaled FP16 behavior |

---

<div align="center">

**Happy Learning! 🚀**

[Report Bug](https://github.com/LessUp/hpc-ai-optimization-lab/issues) · [Request Feature](https://github.com/LessUp/hpc-ai-optimization-lab/issues)

</div>
