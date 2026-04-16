# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

---

## [Unreleased]

## [0.3.0] - 2026-04-16

### Documentation Internationalization / 文档国际化

This release focuses on making the documentation accessible to both English and Chinese readers through complete bilingual documentation suite.

本次发布专注于通过完整的双语文档集使文档对英文和中文读者都可访问。

#### English Documentation Suite

- **[GEMM Optimization](docs/en/01_gemm_optimization.md)** - 7-step matrix multiplication optimization journey (translated from Chinese)
- **[Memory Optimization](docs/en/02_memory_optimization.md)** - Coalesced access, vectorization, shared memory patterns (translated from Chinese)
- **[Reduction Optimization](docs/en/03_reduction_optimization.md)** - Warp shuffle, online softmax, LayerNorm algorithms (translated from Chinese)
- **[FlashAttention](docs/en/04_flash_attention.md)** - IO-aware attention mechanism with tiling strategy (translated from Chinese)
- **[CUDA 13 Features](docs/en/05_cuda13_features.md)** - Hopper architecture: TMA, Clusters, FP8 support (translated from Chinese)
- **[API Reference](docs/en/API_REFERENCE.md)** - Complete C++/CUDA API documentation
- **[Architecture Overview](docs/en/ARCHITECTURE.md)** - Project design patterns and module organization

#### 中文文档集 / Chinese Documentation Suite

- **[GEMM 优化](docs/zh-CN/01_gemm_optimization.md)** - 7步矩阵乘法优化之旅
- **[访存优化](docs/zh-CN/02_memory_optimization.md)** - 合并访问、向量化、共享内存模式
- **[归约优化](docs/zh-CN/03_reduction_optimization.md)** - Warp洗牌、在线Softmax、LayerNorm算法
- **[FlashAttention](docs/zh-CN/04_flash_attention.md)** - 基于IO感知的注意力机制与分块策略
- **[CUDA 13 特性](docs/zh-CN/05_cuda13_features.md)** - Hopper架构：TMA、集群、FP8支持
- **[API 参考](docs/zh-CN/API_REFERENCE.md)** - 完整C++/CUDA API文档（从英文翻译）
- **[架构概览](docs/zh-CN/ARCHITECTURE.md)** - 项目设计模式与模块组织（从英文翻译）

### Reorganization / 结构重组

- Restructured `docs/` directory with `en/` and `zh-CN/` subdirectories
- Created professional documentation navigation with language switching
- Moved historical changelogs to `changelog/archive/`
- Added comprehensive release notes for v0.3.0

- 重组 `docs/` 目录，创建 `en/` 和 `zh-CN/` 子目录
- 创建专业文档导航与语言切换
- 将历史变更日志移至 `changelog/archive/`
- 添加 v0.3.0 详细发布说明

### Improvements in Previous Unreleased Changes / 先前未发布改进

- Comprehensive API reference documentation (`docs/API_REFERENCE.md`)
- Architecture design documentation (`docs/ARCHITECTURE.md`)
- Learning paths and quick reference in documentation portal
- Support matrix documentation for stable vs experimental features
- Enhanced documentation portal with modern dark theme
- Rewrote README.md with badges, tables, and clear module overview
- Rewrote README.zh-CN.md (Chinese) with matching improvements

---

## [0.2.0] - 2025-01-15

### Added

- Project quality improvement infrastructure
  - Apache 2.0 LICENSE
  - CONTRIBUTING.md with development guidelines
  - CODE_OF_CONDUCT.md (Contributor Covenant)
  - GitHub Issue templates (Bug Report, Feature Request)
  - GitHub Pull Request template
- CI/CD with GitHub Actions
  - Format check (clang-format, ruff)
  - Entry consistency verification
  - Documentation build (Doxygen + Sphinx)
  - GitHub Pages deployment
- Code quality tools configuration
  - `.clang-format` (Google style)
  - `.clang-tidy` (static analysis)
  - `.editorconfig` (editor settings)
  - `.pre-commit-config.yaml` (git hooks)
- Documentation system
  - Doxygen configuration for C++/CUDA API
  - Sphinx configuration for Python API
  - Documentation portal (index.html, guides.html)
  - Modern dark theme CSS

### Changed

- Unified Python extension module name to `hpc_ai_opt`
- Connected `examples/` to top-level CMake build via `BUILD_EXAMPLES`
- Updated documentation to reflect repository structure

---

## [0.1.0] - 2024-01-01

### Added

- **Initial Release** of HPC-AI-Optimization-Lab

- **Common Library** (`src/common/`)
  - `cuda_check.cuh`: CUDA error checking macros (`CUDA_CHECK`, `CUDA_CHECK_LAST`)
  - `timer.cuh`: High-precision GPU timer using CUDA events
  - `tensor.cuh`: RAII Tensor class with C++20 Concepts
  - `types.cuh`: Half/BF16 type wrappers and utilities

- **Elementwise Module** (`src/01_elementwise/`)
  - ReLU: Naive → Vectorized (float4) → Grid Stride Loop
  - Sigmoid: Three optimization levels
  - Vector Add: Three optimization levels
  - Transpose: Naive → Shared Memory → Shared Memory + Padding

- **Reduction Module** (`src/02_reduction/`)
  - Softmax: Naive → Warp Shuffle → Online Softmax → Fused
  - LayerNorm: Warp Shuffle + Block Reduce
  - RMSNorm: Optimized implementation with Welford's algorithm

- **GEMM Module** (`src/03_gemm/`) - 7-Step Optimization Journey
  - Step 1: Naive Global Memory (~0.5 TFLOPS)
  - Step 2: Shared Memory Tiling (~2.0 TFLOPS)
  - Step 3: Double Buffering (~3.5 TFLOPS)
  - Step 4: Register Tiling (~6.0 TFLOPS)
  - Step 5: Tensor Core WMMA (~50+ TFLOPS)
  - Step 6: Tensor Core MMA PTX (~60+ TFLOPS)
  - Step 7: Software Pipelining (~70+ TFLOPS)
  - Multi-precision support: SGEMM, HGEMM, Int8-GEMM

- **Convolution Module** (`src/04_convolution/`)
  - Implicit GEMM convolution (production-ready)
  - Winograd convolution (3×3 kernels, with fallback)

- **Attention Module** (`src/05_attention/`)
  - FlashAttention Forward Pass with online softmax
  - RoPE (Rotary Positional Embedding)
  - MoE TopK routing

- **Quantization Module** (`src/06_quantization/`)
  - Weight-Only Dequantization
  - INT8 per-row quantization/dequantization
  - FP8 scaling utilities

- **CUDA 13 Features** (`src/07_cuda13_features/`) - Experimental
  - TMA (Tensor Memory Accelerator) - fallback implementation
  - Thread Block Clusters - fallback implementation
  - FP8 GEMM (e4m3/e5m2) - demonstration implementation

- **Testing Infrastructure**
  - GoogleTest unit testing framework
  - RapidCheck property-based testing
  - Comprehensive test coverage for all modules

- **Documentation**
  - GEMM optimization guide (7-step deep dive)
  - Memory optimization guide (coalescing, vectorization)
  - Reduction optimization guide (warp shuffle, online algorithms)
  - FlashAttention guide (tiling, IO analysis)
  - CUDA 13 features guide (experimental)

- **Build System**
  - CMake 3.24+ with modern target-based configuration
  - FetchContent for dependency management (googletest, nanobind, fmt, cutlass)
  - Automatic GPU architecture detection
  - Docker development environment (CUDA 12.4+)

- **Python Integration**
  - Nanobind bindings with zero-copy design
  - PyTorch tensor support
  - Benchmark scripts with PyTorch comparison

---

## Version History

| Version | Date | Description |
|---------|------|-------------|
| 0.2.0 | 2025-01-15 | Project quality improvements, CI/CD, documentation portal |
| 0.1.0 | 2024-01-01 | Initial release |

---

## Upgrade Guide

### From 0.1.0 to 0.2.0

1. **Python Module Name**: The Python module is now consistently named `hpc_ai_opt`
   ```python
   # Old (may still work)
   import hpc_kernels
   
   # New (recommended)
   import hpc_ai_opt
   ```

2. **Build System**: Examples are now opt-in
   ```bash
   # Enable examples
   cmake -S . -B build -DBUILD_EXAMPLES=ON
   ```

3. **CI Changes**: New format checks require clang-format 17+
   ```bash
   # Install pre-commit hooks locally
   pip install pre-commit
   pre-commit install
   ```

---

[Unreleased]: https://github.com/LessUp/hpc-ai-optimization-lab/compare/v0.2.0...HEAD
[0.2.0]: https://github.com/LessUp/hpc-ai-optimization-lab/compare/v0.1.0...v0.2.0
[0.1.0]: https://github.com/LessUp/hpc-ai-optimization-lab/releases/tag/v0.1.0
