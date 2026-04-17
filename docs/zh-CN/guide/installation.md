# Installation Guide

This guide walks you through installing HPC-AI-Optimization-Lab on your system.

## Prerequisites

Before installing, ensure your system meets the following requirements:

### Hardware Requirements

- **GPU**: NVIDIA CUDA-capable GPU (Compute Capability 7.0+)
- **Recommended**: Volta (7.0), Ampere (8.0), or Hopper (9.0) architecture
- **Memory**: 8GB+ GPU memory recommended for large kernels

### Software Requirements

- **Operating System**: Linux (Ubuntu 20.04+, CentOS 8+, or similar)
- **CUDA Toolkit**: 12.4 or later
- **CMake**: 3.24 or later
- **Compiler**: GCC 11+ or Clang 14+
- **Python** (optional): 3.9+ for Python bindings

## Installation Methods

### Method 1: Clone and Build (Recommended)

```bash
# Clone the repository
git clone https://github.com/LessUp/hpc-ai-optimization-lab.git
cd hpc-ai-optimization-lab

# Create build directory
mkdir build && cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build the project
cmake --build . -j$(nproc)

# Run tests (optional)
ctest --output-on-failure
```

### Method 2: Build with Examples

```bash
# Configure with examples enabled
cmake .. \
  -DCMAKE_BUILD_TYPE=Release \
  -DBUILD_EXAMPLES=ON

# Build
cmake --build . -j$(nproc)

# Run an example
./bin/examples/relu_example
```

### Method 3: Build with Python Bindings

```bash
# Install Python dependencies first
pip install nanobind numpy

# Configure with Python bindings
cmake .. \
  -DCMAKE_BUILD_TYPE=Release \
  -DBUILD_PYTHON_BINDINGS=ON \
  -DPython_EXECUTABLE=$(which python3)

# Build
cmake --build . -j$(nproc)
```

## Verifying Installation

After building, verify your installation:

```bash
# Run all tests
ctest --output-on-failure

# Run a specific test
./tests/elementwise/test_relu

# Check Python binding (if enabled)
python3 -c "import hpc_ai_opt; print(hpc_ai_opt.__version__)"
```

## Troubleshooting

If you encounter issues:

1. **CUDA not found**: Ensure `CUDA_HOME` or `CUDA_PATH` is set
2. **CMake version too old**: Install newer CMake from https://cmake.org/download/
3. **Compilation errors**: Verify GCC 11+ or Clang 14+ is installed
4. **Test failures**: Check GPU availability and CUDA driver version

For more help, see the [Troubleshooting Guide](../TROUBLESHOOTING.md).

## Next Steps

- Follow the [Quick Start Guide](quick-start.md) for your first kernel
- Explore the [Module Documentation](../API_REFERENCE.md)
- Check out [Examples](../../examples/README.md)
