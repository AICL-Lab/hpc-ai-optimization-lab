# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/).

## [Unreleased]

### Added
- New profiling guide documentation
- Best practices guide for kernel development
- Installation and quick-start guides

## [0.3.0] - 2026-04-16

### Added
- FlashAttention kernels with memory analysis
- RoPE (Rotary Position Encoding) kernels
- TopK selection kernels
- INT8 and FP8 quantization kernels
- CUDA 13 Hopper architecture support (TMA, clusters, FP8 GEMM)
- Python bindings via nanobind
- Comprehensive documentation (VitePress + Doxygen + Sphinx)
- Property-based testing with RapidCheck

### Changed
- Unified module interface pattern across all kernels
- Improved error handling with CUDA_CHECK macros
- Enhanced CI/CD workflow with CPU-safe execution

### Fixed
- Memory leak in softmax kernel
- Race condition in reduction operations
- Build issues on older CMake versions

## [0.2.0] - 2026-03-10

### Added
- GEMM optimization with 7 implementation levels
- Winograd convolution kernels
- LayerNorm and RMSNorm kernels
- Doxygen API documentation
- GitHub Pages deployment workflow

### Changed
- Refactored common utilities into `src/common/`
- Improved launch configuration utilities
- Enhanced test coverage to 85%

## [0.1.0] - 2026-02-13

### Added
- Initial project structure
- Elementwise kernels (ReLU, Sigmoid, Transpose, VectorAdd)
- Basic reduction kernels
- GoogleTest unit tests
- CMake build system
- Docker containerization

[Unreleased]: https://github.com/LessUp/hpc-ai-optimization-lab/compare/v0.3.0...HEAD
[0.3.0]: https://github.com/LessUp/hpc-ai-optimization-lab/compare/v0.2.0...v0.3.0
[0.2.0]: https://github.com/LessUp/hpc-ai-optimization-lab/compare/v0.1.0...v0.2.0
[0.1.0]: https://github.com/LessUp/hpc-ai-optimization-lab/releases/tag/v0.1.0
