# Getting Started

Welcome to **HPC-AI-Optimization-Lab** — a comprehensive educational laboratory for high-performance CUDA kernels.

## What You'll Learn

This documentation covers the full spectrum of CUDA kernel optimization, from fundamental memory access patterns to cutting-edge Tensor Core usage:

- **Memory Optimization** - Coalesced access, vectorization, shared memory patterns
- **Reduction Operations** - Warp shuffle, block reduction, online algorithms
- **GEMM Optimization** - 7-step journey from naive kernels to Tensor Core-oriented implementations
- **FlashAttention** - IO-aware attention mechanism

## Prerequisites

- CUDA Toolkit 12.4+
- CMake 3.24+
- C++20 compatible compiler
- NVIDIA GPU (Compute Capability 7.0+)

## Quick Start

### Clone and Build

```bash
# Clone the repository
git clone https://github.com/AICL-Lab/hpc-ai-optimization-lab.git
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
./build/examples/relu_example
./build/examples/gemm_benchmark
```

### Python Bindings

```bash
cmake -S . -B build -DBUILD_PYTHON_BINDINGS=ON
cmake --build build
export PYTHONPATH="$(pwd)/build/python:$PYTHONPATH"
python examples/python/basic_usage.py
```

## Documentation Structure

| Section | Description | Difficulty |
|---------|-------------|------------|
| [Memory Optimization](memory) | Coalesced access, vectorization, shared memory | ⭐⭐ |
| [Reduction Optimization](reduction) | Warp shuffle, online softmax, LayerNorm | ⭐⭐⭐ |
| [GEMM Optimization](gemm) | 7-step matrix multiplication journey | ⭐⭐⭐⭐ |
| [FlashAttention](flash-attention) | IO-aware attention mechanism | ⭐⭐⭐⭐ |

## API References

- [API Reference](/en/api/index) - consolidated C++ / CUDA / Python reference notes
- [Architecture Overview](/en/api/architecture) - Design patterns and module organization

## Next Steps

Choose your learning path:

- **Beginner** → Start with [Memory Optimization](memory)
- **Intermediate** → Jump to [GEMM Optimization](gemm)
- **Advanced** → Explore [FlashAttention](flash-attention) and the architecture/API reference
