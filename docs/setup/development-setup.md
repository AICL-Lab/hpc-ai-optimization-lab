# Development Setup Guide

This guide walks you through setting up the development environment for HPC-AI-Optimization-Lab.

---

## System Requirements

| Requirement | Minimum | Recommended |
|-------------|---------|-------------|
| CUDA Toolkit | 12.4 | 12.4+ |
| CMake | 3.24 | 3.28+ |
| C++ Compiler | GCC 11 / Clang 14 | GCC 12+ / Clang 15+ |
| GPU | SM 7.0 (Volta) | SM 8.0+ (Ampere) |
| Python (optional) | 3.8 | 3.10+ |

---

## Initial Setup

### 1. Clone the Repository

```bash
git clone https://github.com/AICL-Lab/hpc-ai-optimization-lab.git
cd hpc-ai-optimization-lab
```

### 2. Configure and Build

```bash
# Debug build with examples and Python bindings
cmake -S . -B build \
    -DCMAKE_BUILD_TYPE=Debug \
    -DBUILD_EXAMPLES=ON \
    -DBUILD_PYTHON_BINDINGS=ON

# Build
cmake --build build -j$(nproc)
```

### 3. Run Tests

```bash
# Run all tests
ctest --test-dir build --output-on-failure

# Run specific test suite
./build/tests/gemm/test_gemm
```

---

## Docker Development

If you prefer a containerized environment:

```bash
cd docker
docker-compose up -d
docker exec -it hpc-ai-lab bash
```

---

## Pre-commit Hooks

Install pre-commit hooks for automated code quality checks:

```bash
pip install pre-commit
pre-commit install
```

This runs automatic checks before each commit:
- Code formatting (clang-format)
- Static analysis (clang-tidy)
- Trailing whitespace, file endings

---

## Build Configurations

### Development Build

```bash
cmake -S . -B build \
    -DCMAKE_BUILD_TYPE=Debug \
    -DBUILD_EXAMPLES=ON \
    -DBUILD_PYTHON_BINDINGS=ON
```

### Release Build for Specific GPUs

```bash
cmake -S . -B build \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_CUDA_ARCHITECTURES="80;90"  # A100 + H100
```

---

## Python Bindings

```bash
cmake -S . -B build -DBUILD_PYTHON_BINDINGS=ON
cmake --build build
export PYTHONPATH="$(pwd)/build/python:${PYTHONPATH}"

# Verify installation
python -c "import hpc_ai_opt; print('Module loaded successfully!')"

# Run Python example
python examples/python/basic_usage.py
```

---

## Troubleshooting

### Common Issues

| Issue | Solution |
|-------|----------|
| CUDA not found | Ensure CUDA Toolkit 12.4+ is installed |
| CMake version too old | Upgrade CMake to 3.24+ |
| GPU architecture not detected | Set `CMAKE_CUDA_ARCHITECTURES` manually |
| Python binding import error | Check `PYTHONPATH` includes build/python |

For more issues, see [Troubleshooting Guide](../en/guide/troubleshooting.md).
