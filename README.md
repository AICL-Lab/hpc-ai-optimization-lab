# HPC-AI-Optimization-Lab

English | [简体中文](README.zh-CN.md)

A CUDA kernel lab for AI workloads, organized as focused modules for elementwise ops, reductions, GEMM, convolution, attention, quantization, and experimental newer-CUDA paths.

## What is in the repository

- `src/common/`: shared CUDA utilities such as tensor wrappers, timers, launch helpers, and reduction primitives
- `src/01_elementwise/` to `src/07_cuda13_features/`: numbered kernel modules covering elementwise ops, reductions, GEMM, convolution, attention, quantization, and experimental newer-CUDA features
- `tests/`: GoogleTest + RapidCheck coverage across kernel modules
- `examples/`: shipped CUDA and Python examples
- `python/`: nanobind bindings plus benchmark scripts
- `docs/`: optimization notes and Python binding docs

## Support matrix

### Stable / validated focus of this repository

- Core CUDA kernels under `src/01_elementwise` to `src/06_quantization`
- CMake-based native builds and CTest-based validation
- Thin Python bindings for a subset of elementwise, reduction, and GEMM kernels

### Experimental / fallback areas

The following modules currently exist as educational or compatibility-oriented paths rather than production-grade implementations:

- `src/04_convolution/conv_winograd.cu`: currently falls back to the validated implicit-GEMM convolution path
- `src/07_cuda13_features/tma.cu`: currently uses a regular kernel copy fallback
- `src/07_cuda13_features/cluster.cu`: currently uses a portable block-reduction fallback
- `src/07_cuda13_features/fp8_gemm.cu`: currently demonstrates scaled float behavior rather than a true Hopper FP8 kernel

## Build the C++/CUDA project

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j$(nproc)
ctest --test-dir build --output-on-failure
```

## Build the Python bindings

The current Python extension is named `hpc_ai_opt` and exposes low-level submodules such as `elementwise`, `reduction`, and `gemm`.

```bash
cmake -S . -B build -DBUILD_PYTHON_BINDINGS=ON
cmake --build build
export PYTHONPATH="$(pwd)/build/python:${PYTHONPATH}"
python -c "import hpc_ai_opt; print(hpc_ai_opt.__doc__)"
python examples/python/basic_usage.py
```

The bindings are intentionally thin:
- CUDA tensors are passed in directly
- output tensors are allocated by the caller
- several kernels require explicit shape arguments
- wrappers validate basic tensor sizes/arguments, then launch CUDA work asynchronously

## Build the shipped examples

```bash
cmake -S . -B build -DBUILD_EXAMPLES=ON
cmake --build build --target relu_example gemm_benchmark
```

## Current Python API shape

```python
import torch
import hpc_ai_opt

x = torch.randn(1024, 1024, device="cuda", dtype=torch.float32)
y = torch.empty_like(x)

hpc_ai_opt.elementwise.relu(x, y)
```

## Requirements

- CUDA Toolkit 12.4+
- CMake 3.24+
- A C++20 compiler
- An NVIDIA GPU with CUDA support
- PyTorch with CUDA support for the Python example path

Notes:
- The Docker development environment is currently based on CUDA 12.4.1.
- Experimental modules under `src/07_cuda13_features/` are not evidence of full Hopper/Blackwell feature coverage.
- `flash_attention` currently supports `float` with `head_dim == 64` in the shipped implementation.

## CI and verification scope

The default GitHub Actions workflow is intentionally lightweight and currently validates:
- formatting
- repository/documentation consistency checks
- documentation builds

It does **not** currently provide full native CUDA build-and-test coverage on GitHub-hosted runners.
For native verification, run the local CMake + CTest flow shown above on a machine with a working CUDA toolchain and GPU.

## Documentation

- `docs/README.md`
- `docs/python/index.rst`
- `docs/01_gemm_optimization.md`
- `docs/04_flash_attention.md`
