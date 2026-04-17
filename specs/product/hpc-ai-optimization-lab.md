# Product Specification: HPC-AI-Optimization-Lab

> **Status**: Complete (100%)
> **Last Updated**: 2026-04-17
> **Type**: Product Requirements Document (PRD)

## Project Overview

**HPC-AI-Optimization-Lab** is an educational and production-ready CUDA kernel library designed for AI inference workloads.

### Core Objectives

| Objective | Description |
|-----------|-------------|
| Educational Focus | Each module demonstrates a clear optimization progression from naive to expert-level |
| Production Quality | Comprehensive test coverage (GoogleTest unit tests + RapidCheck property tests) |
| Modern C++ | RAII, Concepts, constexpr, requires clauses |
| Python Integration | Nanobind zero-copy bindings with PyTorch Tensor support |

### Tech Stack

| Component | Version | Purpose |
|-----------|---------|---------|
| CUDA | 12.4+ | GPU programming |
| C++ | 20 | Kernel implementation |
| CMake | 3.24+ | Build system |
| Python | 3.8+ | Bindings and testing |

---

## Requirements by Module

### R1: Project Infrastructure

| ID | Requirement | Verification | Status |
|----|-------------|--------------|--------|
| R1.1 | CMake 3.24+ target-based configuration | `cmake --version` >= 3.24 | Complete |
| R1.2 | FetchContent dependency management | Successfully fetches gtest, nanobind, fmt, cutlass | Complete |
| R1.3 | GPU architecture auto-detection | `-gencode` parameters set correctly | Complete |
| R1.4 | Docker development environment | CUDA 12.4+ image builds successfully | Complete |
| R1.5 | Cross-platform support | Linux primary, Windows/WSL2 compatible | Complete |

### R2: Common Library

| ID | Requirement | Implementation File | Status |
|----|-------------|---------------------|--------|
| R2.1 | CUDA error checking macros | `cuda_check.cuh` | Complete |
| R2.2 | High-precision GPU timer | `timer.cuh` | Complete |
| R2.3 | RAII Tensor memory management | `tensor.cuh` | Complete |
| R2.4 | Half/BF16 type wrappers | `types.cuh` | Complete |
| R2.5 | Reduction primitives | `reduce.cuh` | Complete |
| R2.6 | Kernel launch utilities | `launch.cuh` | Complete |

### R3: Elementwise Module

| ID | Requirement | Optimization Levels | Performance Target | Status |
|----|-------------|---------------------|-------------------|--------|
| R3.1 | ReLU Kernel | Naive -> Vectorized -> GridStride | 80%+ memory bandwidth | Complete |
| R3.2 | Sigmoid Kernel | Naive -> Vectorized -> GridStride | 80%+ memory bandwidth | Complete |
| R3.3 | Vector Add Kernel | Naive -> Vectorized -> GridStride | 80%+ memory bandwidth | Complete |
| R3.4 | Transpose Kernel | Naive -> SharedMem -> SharedMemPadded | No bank conflicts | Complete |
| R3.5 | float4 vectorized loads | All elementwise kernels | 2-4x speedup | Complete |
| R3.6 | Grid Stride Loop | Handle arbitrary input sizes | No out-of-bounds | Complete |

### R4: Reduction Module

| ID | Requirement | Optimization Levels | Key Technology | Status |
|----|-------------|---------------------|----------------|--------|
| R4.1 | Softmax | Naive -> WarpShuffle -> Online | Single-pass algorithm | Complete |
| R4.2 | LayerNorm | Warp Shuffle + Block Reduce | Welford's algorithm | Complete |
| R4.3 | RMSNorm | Optimized implementation | Numerically stable | Complete |
| R4.4 | Warp Shuffle Primitives | Replace shared memory | `__shfl_down_sync` | Complete |
| R4.5 | Online Softmax | Single-pass algorithm | Online max/sum updates | Complete |

### R5: GEMM Module

| ID | Requirement | Technique | Performance Target | Status |
|----|-------------|-----------|-------------------|--------|
| R5.1 | Step 1: Naive | Global memory | ~0.5 TFLOPS | Complete |
| R5.2 | Step 2: Shared Memory Tiling | Data reuse | ~2.0 TFLOPS | Complete |
| R5.3 | Step 3: Double Buffer | Latency hiding | ~3.5 TFLOPS | Complete |
| R5.4 | Step 4: Register Tiling | Reduce bank conflicts | ~6.0 TFLOPS | Complete |
| R5.5 | Step 5: Tensor Core WMMA | Hardware acceleration | ~50 TFLOPS | Complete |
| R5.6 | Step 6: Tensor Core MMA PTX | Fine-grained control | ~60 TFLOPS | Complete |
| R5.7 | Step 7: Software Pipelining | Multi-stage overlap | ~70 TFLOPS | Complete |
| R5.8 | Multi-precision support | SGEMM, HGEMM, Int8-GEMM | - | Complete |

### R6: Attention Module

| ID | Requirement | Key Technology | Constraints | Status |
|----|-------------|----------------|-------------|--------|
| R6.1 | FlashAttention Forward | IO-aware, tiling | head_dim=64 | Complete |
| R6.2 | Q, K, V Tiling | SRAM resident | - | Complete |
| R6.3 | Online Softmax | Avoid NxN matrix storage | - | Complete |
| R6.4 | RoPE | Complex rotation | - | Complete |
| R6.5 | MoE TopK | Fast sorting/filtering | - | Complete |

### R7: Convolution Module

| ID | Requirement | Description | Status |
|----|-------------|-------------|--------|
| R7.1 | Implicit GEMM Convolution | Production-ready | Complete |
| R7.2 | Winograd Convolution | Fallback to GEMM | Complete |
| R7.3 | Convolution parameter support | stride, padding, dilation | Complete |

### R8: Quantization Module

| ID | Requirement | Description | Status |
|----|-------------|-------------|--------|
| R8.1 | Weight-Only Dequant | Weight dequantization | Complete |
| R8.2 | INT8 Quantization/Dequantization | Per-row scaling | Complete |
| R8.3 | FP8 Scaling | Precision control | Complete |

### R9: CUDA 13 Features (Experimental)

| ID | Requirement | Current Status | Description | Status |
|----|-------------|----------------|-------------|--------|
| R9.1 | TMA async transfer | Fallback | async copy substitute | Complete |
| R9.2 | Thread Block Clusters | Fallback | block reduction substitute | Complete |
| R9.3 | FP8 GEMM | Demo | scaled FP16 demonstration | Complete |

### R10: Python Bindings

| ID | Requirement | Implementation | Status |
|----|-------------|----------------|--------|
| R10.1 | Nanobind zero-copy | Direct pointer passing | Complete |
| R10.2 | PyTorch Tensor support | CUDA tensor | Complete |
| R10.3 | Module structure | elementwise, reduction, gemm | Complete |
| R10.4 | Parameter validation | Friendly error messages | Complete |

---

## Non-Functional Requirements

### NFR1: Performance

| Requirement | Metric |
|-------------|--------|
| GEMM Tensor Core | Achieve 70%+ of theoretical peak |
| Elementwise | Achieve 80%+ of memory bandwidth |
| FlashAttention | Correct at head_dim=64 |

### NFR2: Portability

| Platform | Status |
|----------|--------|
| Linux (Ubuntu 22.04+) | Primary support |
| Windows (WSL2) | Compatible |
| GPU Architecture | SM 7.0+ (Volta+) |

### NFR3: Maintainability

| Requirement | Implementation |
|-------------|----------------|
| Code style | .clang-format (Google style) |
| Static analysis | .clang-tidy |
| Documentation coverage | All public APIs |

---

## Constraints

1. **CUDA Version**: Minimum 12.4, recommended 13.x
2. **GPU Architecture**: Minimum SM 7.0 (Volta), Tensor Core requires SM 7.0+
3. **Compiler**: GCC 11+ / Clang 14+ / MSVC 2022+
4. **Python**: 3.8+ (binding time)

---

## Out of Scope

- Training scenario optimization
- Multi-GPU support
- Dynamic graph mode
- Full Hopper feature support (experimental fallback)

---

## Implementation Status

All 12 phases completed (100%). See [tasks document](../testing/hpc-ai-optimization-lab-tasks.md) for detailed task tracking.
