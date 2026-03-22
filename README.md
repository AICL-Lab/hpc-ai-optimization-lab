# HPC-AI-Optimization-Lab

English | [简体中文](README.zh-CN.md)

A CUDA optimization lab for AI kernels, organized as a set of focused kernel modules, tests, examples, and lightweight Python bindings.

## What is in the repository

- `src/common/`: shared CUDA utilities such as tensor wrappers, timers, launch helpers, and reduction primitives
- `src/01_elementwise/` to `src/07_cuda13_features/`: numbered kernel modules covering elementwise ops, reductions, GEMM, convolution, attention, quantization, and newer CUDA features
- `tests/`: GoogleTest + RapidCheck coverage across kernel modules
- `examples/`: currently shipped CUDA and Python examples
- `python/`: nanobind bindings plus benchmark scripts
- `docs/`: optimization notes and Python binding docs

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

The current bindings are intentionally thin:
- CUDA tensors are passed in directly
- output tensors are allocated by the caller
- some kernels require explicit shape arguments

## Requirements

- CUDA Toolkit 13.1+
- CMake 3.24+
- A C++20 compiler
- An NVIDIA GPU with CUDA support
- PyTorch with CUDA support for the Python example path

## Documentation

- `docs/README.md`
- `docs/python/index.rst`
- `docs/01_gemm_optimization.md`
- `docs/04_flash_attention.md`
