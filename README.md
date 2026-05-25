<div align="center">

<!-- Placeholder for Project Banner - recommend size: 1200x400 -->
<!-- ![HPC-AI-Optimization-Lab Banner](docs/assets/banner.png) -->

<h1>HPC-AI-Optimization-Lab</h1>

<p align="center">
  <strong>CUDA kernel optimization lab for GEMM, FlashAttention, quantization, and GPU performance learning.</strong>
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
    <img src="https://img.shields.io/badge/Docs-VitePress-blue?style=flat-square" alt="Documentation">
  </a>
</p>

<p align="center">
  <a href="README.md">English</a> | 
  <a href="README.zh-CN.md">简体中文</a>
</p>

</div>

---

## Table of Contents

- [Overview](#overview)
- [Key Features](#key-features)
- [Quick Start](#quick-start)
  - [Installation](#installation)
  - [Verify Installation](#verify-installation)
  - [Run Your First Kernel](#run-your-first-kernel)
- [Performance](#performance)
- [Documentation](#documentation)
- [Project Structure](#project-structure)
- [Usage Examples](#usage-examples)
- [Testing](#testing)
- [Contributing](#contributing)
- [Roadmap](#roadmap)
- [Support Matrix](#support-matrix)
- [License](#license)

---

## Overview

**HPC-AI-Optimization-Lab** bridges the gap between educational CUDA tutorials and production-grade libraries. Learn GPU optimization through progressive kernel implementations—from simple elementwise ops to Tensor Core-accelerated matrix multiplication.

### Why This Project?

Most CUDA resources fall into two extremes:
- 📚 **Tutorials** teach concepts but lack production quality
- ⚡ **Libraries** (cuBLAS, CUTLASS) are optimized but complex to learn from

**We provide the middle ground:**

| Aspect | Our Approach |
|--------|-------------|
| **Learning Path** | Progressive 7-step optimization journey with detailed explanations |
| **Code Quality** | Tested, benchmarked, following NVIDIA best practices |
| **API Design** | Simple, consistent interfaces for both C++ and Python |
| **Modern AI** | FlashAttention, RoPE, INT8/FP8 quantization included |

### Perfect For

- 🎓 **Students** learning CUDA optimization from first principles
- 🔬 **Researchers** prototyping new kernel optimizations
- 🏭 **Engineers** needing production-ready kernels for AI inference

---

## Key Features

- **Progressive Learning**: 7-step GEMM optimization showing each technique's impact
- **Comprehensive Coverage**: Elementwise, Reduction, GEMM, Attention, Convolution, Quantization
- **Modern Hardware**: CUDA 12.4+, Hopper features (TMA, Clusters), FP8 support
- **Dual API**: Clean C++17/20 templates + Python bindings (nanobind)
- **Tested Across Core Modules**: GoogleTest + RapidCheck cover the main module surface

---

## Quick Start

### Installation

```bash
# Clone repository
git clone https://github.com/LessUp/hpc-ai-optimization-lab.git
cd hpc-ai-optimization-lab

# Create build directory
mkdir build && cd build

# Configure and build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . -j$(nproc)
```

**Requirements:**
| Component | Minimum | Recommended |
|-----------|---------|-------------|
| CUDA Toolkit | 12.4 | 12.4+ |
| CMake | 3.24 | 3.28+ |
| Compiler | GCC 11 / Clang 14 | GCC 12+ / Clang 15+ |
| GPU | SM 7.0 (Volta) | SM 8.0+ (Ampere) |

**Build Options:**
```bash
# With examples and Python bindings
cmake .. -DBUILD_EXAMPLES=ON -DBUILD_PYTHON_BINDINGS=ON

# Target specific architectures
cmake .. -DCMAKE_CUDA_ARCHITECTURES="80;90"  # A100 + H100
```

### Verify Installation

```bash
# Run test suite
ctest --output-on-failure

# Expected output: the configured test preset runs successfully for your local environment
```

### Run Your First Kernel

```bash
# Elementwise operation example
./examples/elementwise/relu_example

# GEMM benchmark with all optimization steps
./examples/gemm/gemm_benchmark

# Python example (if bindings enabled)
python3 ../examples/python/basic_usage.py
```

<details>
<summary><b>Having issues?</b> Common troubleshooting</summary>

**CMake can't find CUDA:**
```bash
export PATH=/usr/local/cuda/bin:$PATH
export CMAKE_CUDA_COMPILER=/usr/local/cuda/bin/nvcc
```

**Out of memory during build:**
```bash
# Reduce parallel jobs
cmake --build . -j4
```

See [Troubleshooting Guide](docs/en/guide/troubleshooting.md) for more.

</details>

---

## Performance

### GEMM Optimization Journey

FP32 matrix multiplication (4096×4096) on NVIDIA A100:

| Step | Technique | Performance | Speedup | Status |
|:----:|-----------|-------------|:-------:|:------:|
| 1 | Naive implementation | 0.5 TFLOPS | 1× | ✅ |
| 2 | Shared memory tiling | 2.0 TFLOPS | 4× | ✅ |
| 3 | Double buffering | 3.5 TFLOPS | 7× | ✅ |
| 4 | Register tiling | 6.0 TFLOPS | 12× | ✅ |
| 5 | **Tensor Core (WMMA)** | **50+ TFLOPS** | **100×** | ✅ |
| 6 | Tensor Core (MMA PTX)* | ~60 TFLOPS† | ~120× | 🚧 |
| 7 | Software pipelining | ~70 TFLOPS† | ~140× | ✅ |

> 💡 The progression from Step 1 to Step 5 demonstrates why modern AI hardware achieves remarkable speedups through specialized units.
> 
> *Step 6 currently delegates to Step 5 for stability. Step 7 ships as the maintained software-pipelined path.  
> †Performance values are projected estimates.

### Module Status

| Module | Key Operations | Status |
|--------|---------------|:------:|
| **Elementwise** | ReLU, Sigmoid, Transpose | ✅ Stable |
| **Reduction** | Softmax, LayerNorm, RMSNorm | ✅ Stable |
| **GEMM** | Matrix multiplication (7-steps) | ✅ Stable |
| **Attention** | FlashAttention, RoPE | ✅ Stable |
| **Convolution** | Implicit GEMM | ✅ Stable |
| **Quantization** | INT8, FP8 | ✅ Stable |

---

## Documentation

📚 **Full documentation:** [https://lessup.github.io/hpc-ai-optimization-lab/](https://lessup.github.io/hpc-ai-optimization-lab/)

### Quick Links

| Topic | English | 中文 |
|-------|---------|------|
| Installation | [Guide](docs/en/guide/installation.md) | [安装指南](docs/zh-CN/guide/installation.md) |
| Quick Start | [5-min Guide](docs/en/guide/quick-start.md) | [快速入门](docs/zh-CN/guide/quick-start.md) |
| GEMM Tutorial | [7-Step Journey](docs/en/guide/gemm.md) | [GEMM优化](docs/zh-CN/guide/gemm.md) |
| Memory Optimization | [Guide](docs/en/guide/memory.md) | [访存优化](docs/zh-CN/guide/memory.md) |
| FlashAttention | [Guide](docs/en/guide/flash-attention.md) | [FlashAttention](docs/zh-CN/guide/flash-attention.md) |
| API Reference | [C++/Python](docs/en/api/index.md) | [API参考](docs/zh-CN/api/index.md) |

### Learning Path

```
🌱 Beginner (1-2 weeks)
├── Installation & Quick Start
├── Memory optimization patterns
├── Reduction algorithms
└── GEMM Steps 1-4

🚀 Intermediate (2-4 weeks)
├── GEMM Steps 5-7 (Tensor Cores)
├── FlashAttention implementation
└── Profiling & tuning

🏆 Advanced (ongoing)
├── CUTLASS baseline comparison
├── Local GPU profiling and validation
└── Research implementations
```

---

## Project Structure

```
hpc-ai-optimization-lab/
├── src/                    # CUDA kernel implementations
│   ├── common/             # Tensor, Timer, CUDA utilities
│   ├── elementwise/        # ReLU, Sigmoid, etc.
│   ├── reduction/          # Softmax, LayerNorm, RMSNorm
│   ├── gemm/               # 7-step GEMM optimization
│   ├── attention/          # FlashAttention, RoPE
│   ├── convolution/        # Implicit GEMM
│   └── quantization/       # INT8/FP8
├── tests/                  # Test suite (GoogleTest + RapidCheck)
├── examples/               # Standalone examples
├── python/                 # Python bindings
├── docs/                   # Documentation (VitePress)
└── docker/                 # Development environment
```

---

## Usage Examples

### C++ API

```cpp
#include "gemm/gemm.cuh"
#include "common/tensor.cuh"

// Allocate GPU tensors
constexpr int M = 1024;
constexpr int N = 1024;
constexpr int K = 1024;

hpc::Tensor<float> A(M * K);
hpc::Tensor<float> B(K * N);
hpc::Tensor<float> C(M * N);
C.zero();

// Launch the current shared-memory-tiling GEMM path
hpc::gemm::gemm<float, hpc::gemm::GemmOpt::SharedMemTiling>(
    A.data(), B.data(), C.data(), M, N, K);

// Automatic memory cleanup when tensors go out of scope
```

### Python API

Current Python bindings expose `elementwise`, `reduction`, and `gemm`.

```python
import hpc_ai_opt
import torch

# Create CUDA tensors
a = torch.randn(128, 64, device="cuda", dtype=torch.float32)
b = torch.randn(64, 96, device="cuda", dtype=torch.float32)
c = torch.zeros(128, 96, device="cuda", dtype=torch.float32)

# Execute the currently shipped GEMM binding
hpc_ai_opt.gemm.matmul(a, b, c, 128, 96, 64, 1.0, 0.0)

print(c.shape)
```

Current phase benchmark CLI:

```bash
python3 python/benchmark/benchmark.py --suite gemm --sizes 256,512 --output results.json
```

The Python benchmark entrypoint currently wires the GEMM suite by default and emits reports only from measured result sets.

---

## Testing

```bash
# Run all tests
ctest --output-on-failure

# Run specific module
./tests/gemm/test_gemm

# With verbose output
ctest -V
```

| Test Type | Status |
|-----------|--------|
| Unit Tests (GoogleTest) | Maintained for the public module surface |
| Property Tests (RapidCheck) | Used for edge cases and invariants |
| Local baseline | Reconfigure with CMake presets before trusting results |

---

## Contributing

We welcome contributions! See [CONTRIBUTING.md](CONTRIBUTING.md) for guidelines.

```bash
# Quick contribution workflow
git checkout -b feature/my-optimization
# ... make changes ...
cmake -S . -B build && cmake --build build
ctest --test-dir build --output-on-failure
git commit -m "feat(gemm): add optimization X"
git push origin feature/my-optimization
```

> ⚠️ **CI Note:** This repository does **not** currently provide full native CUDA build-and-test on GitHub-hosted runners. GPU validation still requires local execution or self-hosted infrastructure.

---

## Roadmap

The repository is in a simplify-and-harden phase.

### Current focus

- simplify repository structure and reduce process overhead
- reduce workflow and documentation noise
- improve GitHub Pages and repository presentation
- fix trust-reducing inconsistencies across docs, tooling, and automation

### Not the current priority

- broad new feature expansion
- speculative future release promises
- large new integration surfaces without clear near-term implementation value

---

## Support Matrix

| Module | FP32 | FP16 | BF16 | INT8 | FP8 | Status |
|--------|:----:|:----:|:----:|:----:|:---:|:------:|
| Elementwise | ✅ | ✅ | ✅ | - | - | Stable |
| Reduction | ✅ | ✅ | ✅ | - | - | Stable |
| GEMM | ✅ | ✅ | ✅ | ✅ | 🚧 | Stable |
| Convolution | ✅ | ✅ | - | - | - | Stable |
| Attention | ✅ | ✅ | - | - | - | Stable |
| Quantization | ✅ | ✅ | - | ✅ | 🚧 | Stable |

The support matrix describes the C++/CUDA core. In this phase, Python bindings cover `elementwise`, `reduction`, and `gemm` only.

🚧 = Partial support / In development

---

## License

This project is licensed under the [Apache License 2.0](LICENSE).

---

<div align="center">

**⭐ Star this repo if you find it helpful!**

[Report Bug](https://github.com/LessUp/hpc-ai-optimization-lab/issues) · 
[Request Feature](https://github.com/LessUp/hpc-ai-optimization-lab/issues) · 
[Documentation](https://lessup.github.io/hpc-ai-optimization-lab/)

</div>
