# HPC-AI-Optimization-Lab

[简体中文](README.md) | English

<p align="center">
  <b>A Living Textbook for High-Performance CUDA Kernel Development</b>
</p>

![CUDA](https://img.shields.io/badge/CUDA-13.1+-76B900?style=flat-square&logo=nvidia)
![C++20](https://img.shields.io/badge/C++-20-00599C?style=flat-square&logo=cplusplus)
![Architecture](https://img.shields.io/badge/Architecture-Hopper%2FBlackwell-green?style=flat-square)
![License](https://img.shields.io/badge/License-MIT-blue?style=flat-square)

---

## Overview

A systematic CUDA high-performance computing tutorial, from naive implementations to extreme optimization, covering core operators needed by modern AI models (LLM, Diffusion).

## Modules

| Module | Description | Key Techniques |
|--------|-------------|----------------|
| **GEMM** | Matrix multiplication optimization | Tiled → Register Blocked → Tensor Core |
| **Attention** | FlashAttention variants | Online Softmax, causal masking |
| **Normalization** | LayerNorm, RMSNorm | Warp shuffle, vectorized loads |
| **Elementwise** | Activation functions | GELU, SiLU, vectorized |
| **Quantization** | INT8/FP8 | Calibration, per-channel scaling |
| **Fusion** | Kernel fusion patterns | Bias+Act, LayerNorm+Residual |

## Quick Start

```bash
git clone https://github.com/LessUp/hpc-ai-optimization-lab.git
cd hpc-ai-optimization-lab

cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j$(nproc)
ctest --test-dir build --output-on-failure
```

## Requirements

- CUDA Toolkit 13.1+ (Hopper/Blackwell recommended)
- CMake 3.20+, C++20 compiler
- GPU: SM 8.0+ (Ampere or newer)

## Project Structure

```
hpc-ai-optimization-lab/
├── src/                    # Kernel implementations
│   ├── gemm/               # GEMM optimization levels
│   ├── attention/           # Attention kernels
│   ├── normalization/       # Norm kernels
│   ├── elementwise/         # Activation kernels
│   └── quantization/        # Quantization kernels
├── include/                # Public headers
├── tests/                  # Google Test suite
├── benchmarks/             # Performance benchmarks
├── docs/                   # Documentation
└── .github/workflows/      # CI
```

## Key Topics

- **Memory Hierarchy**: Global → Shared → Register optimization
- **Tensor Core Programming**: WMMA / MMA for mixed-precision compute
- **Async Operations**: TMA, async copy, pipeline overlapping
- **Warp-Level Primitives**: Shuffle, vote, cooperative groups
- **Kernel Fusion**: Reducing HBM round-trips

## License

MIT License
