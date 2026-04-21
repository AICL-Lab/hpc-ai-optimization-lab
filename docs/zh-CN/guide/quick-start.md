# Quick Start Guide

Get up and running with HPC-AI-Optimization-Lab in minutes.

## Overview

This quick start will show you how to:
1. Build your first optimized kernel
2. Run a benchmark
3. Use Python bindings (optional)

## 5-Minute Quick Start

### Step 1: Build the Project

```bash
# Clone and enter the repository
git clone https://github.com/LessUp/hpc-ai-optimization-lab.git
cd hpc-ai-optimization-lab

# Create and enter build directory
mkdir build && cd build

# Configure for release build
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build with 8 parallel jobs
cmake --build . -j8
```

### Step 2: Run Tests

```bash
# Run all tests
ctest --output-on-failure

# Expected output: 100% tests passed
```

### Step 3: Run an Example

```bash
# Run the ReLU example
./bin/examples/relu_example

# Expected output:
# ReLU kernel launched successfully!
# Output verified ✓
```

## Building Specific Modules

If you only need certain modules:

```bash
# Build only elementwise module
cmake --build . --target elementwise

# Build all tests for elementwise
cmake --build . --target test_elementwise
```

## Using Python Bindings

### Enable Python Support

```bash
# Reconfigure with Python bindings
cmake .. \
  -DCMAKE_BUILD_TYPE=Release \
  -DBUILD_PYTHON_BINDINGS=ON

# Rebuild
cmake --build . -j8
```

### Use in Python

```python
import hpc_ai_opt
import numpy as np

# Create input tensor
x = np.random.randn(1024).astype(np.float32)

# Apply ReLU kernel
y = hpc_ai_opt.relu(x)

print(f"Input: {x[:5]}")
print(f"Output: {y[:5]}")
```

## Next Steps

Now that you have the basics, explore:

| Topic | Guide |
|-------|-------|
| Memory Optimization | [Memory Guide](memory.md) |
| GEMM Optimization | [GEMM Guide](gemm.md) |
| Performance Tuning | [Performance Tuning](performance-tuning.md) |
| API Reference | [API Reference](../api/index.md) |

## Common Commands Cheat Sheet

```bash
# Build
cmake .. -DCMAKE_BUILD_TYPE=Release && cmake --build . -j8

# Test
ctest --output-on-failure

# Benchmark
./examples/gemm/gemm_benchmark

# Clean build
rm -rf build && mkdir build && cd build

# Python install (optional)
pip install -e python/
```

## Getting Help

- **Documentation**: Browse the [Guides](index.md)
- **Issues**: Check [Troubleshooting](troubleshooting.md)
- **Community**: Open a [GitHub Issue](https://github.com/LessUp/hpc-ai-optimization-lab/issues)
