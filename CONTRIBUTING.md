# Contributing Guide | 贡献指南

Thank you for your interest in contributing to HPC-AI-Optimization-Lab! This document provides guidelines and instructions for contributing.

感谢您对 HPC-AI-Optimization-Lab 的关注！本文档提供贡献指南和说明。

---

## Table of Contents | 目录

- [Code of Conduct](#code-of-conduct)
- [Getting Started](#getting-started)
- [Development Setup](#development-setup)
- [Code Style](#code-style)
- [Testing Guidelines](#testing-guidelines)
- [Commit Guidelines](#commit-guidelines)
- [Pull Request Process](#pull-request-process)

---

## Code of Conduct

This project adheres to the [Contributor Covenant Code of Conduct](CODE_OF_CONDUCT.md). By participating, you are expected to uphold this code. Please report unacceptable behavior to the project maintainers.

---

## Getting Started

### Ways to Contribute

| Type | Description |
|------|-------------|
| 🐛 Bug Reports | Submit issues with reproducible examples |
| 💡 Feature Requests | Propose new kernels or optimizations |
| 📝 Documentation | Improve guides, add examples |
| 🔧 Code | Implement features, fix bugs |
| 🧪 Tests | Add test coverage, improve test quality |

### Before You Start

1. Check [existing issues](https://github.com/LessUp/hpc-ai-optimization-lab/issues) to avoid duplicates
2. For major changes, open a discussion issue first
3. Ensure you have a CUDA-capable GPU and development environment

---

## Development Setup

### System Requirements

| Requirement | Minimum | Recommended |
|-------------|---------|-------------|
| CUDA Toolkit | 12.4 | 12.4+ |
| CMake | 3.24 | 3.28+ |
| C++ Compiler | GCC 11 / Clang 14 | GCC 12+ / Clang 15+ |
| GPU | SM 7.0 (Volta) | SM 8.0+ (Ampere) |
| Python (optional) | 3.8 | 3.10+ |

### Initial Setup

```bash
# Fork and clone
git clone https://github.com/YOUR_USERNAME/hpc-ai-optimization-lab.git
cd hpc-ai-optimization-lab

# Create development branch
git checkout -b feature/your-feature

# Configure with debug symbols
cmake -S . -B build \
    -DCMAKE_BUILD_TYPE=Debug \
    -DBUILD_EXAMPLES=ON \
    -DBUILD_PYTHON_BINDINGS=ON

# Build
cmake --build build -j$(nproc)

# Run tests
ctest --test-dir build --output-on-failure
```

### Docker Development

```bash
cd docker
docker-compose up -d
docker exec -it hpc-ai-lab bash
```

### Pre-commit Hooks

```bash
pip install pre-commit
pre-commit install
```

This runs automatic checks before each commit:
- Code formatting (clang-format)
- Static analysis (clang-tidy)
- Trailing whitespace, file endings

---

## Code Style

### C++/CUDA Guidelines

#### Naming Conventions

| Type | Convention | Example |
|------|------------|---------|
| Namespaces | `snake_case` | `hpc::elementwise` |
| Classes/Structs | `PascalCase` | `Tensor`, `CudaTimer` |
| Functions | `snake_case` | `copy_from_host`, `relu` |
| Variables | `snake_case` | `block_size`, `num_threads` |
| Constants | `UPPER_SNAKE_CASE` | `TILE_SIZE`, `WARP_SIZE` |
| Macros | `UPPER_SNAKE_CASE` | `CUDA_CHECK` |
| Template Parameters | `PascalCase` | `typename T`, `int BlockSize` |

#### File Organization

```cpp
// kernel_name.cuh - Header file
#pragma once

#include <cuda_runtime.h>
#include <concepts>

namespace hpc::module {

// Public interface only
template <typename T, OptLevel Level = OptLevel::Default>
void kernel_name(const T* input, T* output, size_t n,
                 cudaStream_t stream = nullptr);

} // namespace hpc::module
```

```cpp
// kernel_name.cu - Implementation file
#include "kernel_name.cuh"
#include "../common/cuda_check.cuh"

namespace hpc::module {

// Anonymous namespace for private kernels
namespace {

template <typename T>
__global__ void kernel_impl(const T* __restrict__ input,
                             T* __restrict__ output, size_t n) {
    // Implementation
}

} // anonymous namespace

// Explicit specializations
template <>
void kernel_name<float, OptLevel::Default>(...) {
    kernel_impl<float><<<grid, block, 0, stream>>>(...);
    CUDA_CHECK_LAST();
}

} // namespace hpc::module
```

#### CUDA Best Practices

```cpp
// ✓ Good: Use restrict qualifier
__global__ void kernel(const float* __restrict__ input,
                        float* __restrict__ output) { }

// ✓ Good: Use constexpr for compile-time constants
constexpr int TILE_SIZE = 32;

// ✓ Good: Use __forceinline__ for small device functions
__device__ __forceinline__ float warp_reduce_sum(float val) { }

// ✓ Good: Use appropriate memory spaces
__shared__ float tile[TILE_SIZE][TILE_SIZE];  // Block-level sharing
__constant__ float params[256];                 // Read-only, broadcast

// ✗ Bad: Mutable global state
__device__ int global_counter;  // Avoid if possible
```

#### Modern C++ Features

```cpp
// Use concepts for template constraints
template <typename T>
    requires std::is_same_v<T, float> || std::is_same_v<T, __half>
void kernel_function(const T* input, T* output);

// Use RAII for resource management
hpc::Tensor<float> data(n);  // Automatic cleanup

// Use [[nodiscard]] for important return values
[[nodiscard]] bool almost_equal(float a, float b);

// Use constexpr where possible
constexpr size_t calculate_tile_size(int smem_per_block) {
    return smem_per_block / sizeof(float);
}
```

### Python Style

Follow [PEP 8](https://peps.python.org/pep-0008/) with these additions:

```python
# Keep bindings thin - just validation and CUDA calls
def relu(input: torch.Tensor, output: torch.Tensor) -> None:
    """
    Apply ReLU activation.
    
    Args:
        input: Input tensor (CUDA, float32)
        output: Output tensor (CUDA, float32, same shape as input)
    
    Raises:
        ValueError: If tensors are not CUDA tensors or shapes mismatch
    """
    if not input.is_cuda or not output.is_cuda:
        raise ValueError("Tensors must be on CUDA device")
    if input.shape != output.shape:
        raise ValueError("Input and output shapes must match")
    
    _cuda_relu(input, output)  # Call C++ binding
```

---

## Testing Guidelines

### Test Structure

```
tests/
├── test_utils.hpp          # Shared test utilities
├── module_name/
│   └── test_kernel.cpp     # Kernel-specific tests
```

### Unit Tests

```cpp
#include <gtest/gtest.h>
#include "01_elementwise/relu.cuh"
#include "common/tensor.cuh"
#include "../test_utils.hpp"

TEST(ReluTest, BasicCorrectness) {
    // Arrange
    std::vector<float> input = {-1.0f, 0.0f, 1.0f, 2.0f};
    std::vector<float> expected = {0.0f, 0.0f, 1.0f, 2.0f};
    
    hpc::Tensor<float> d_input(input.size());
    hpc::Tensor<float> d_output(input.size());
    d_input.copy_from_host(input);
    
    // Act
    hpc::elementwise::relu<float>(d_input.data(), d_output.data(), input.size());
    cudaDeviceSynchronize();
    
    // Assert
    auto result = d_output.to_host();
    EXPECT_TRUE(hpc::test::vectors_almost_equal(result, expected));
}

TEST(ReluTest, HandlesEmptyInput) {
    hpc::Tensor<float> d_input(0);
    hpc::Tensor<float> d_output(0);
    
    EXPECT_NO_THROW(
        hpc::elementwise::relu<float>(d_input.data(), d_output.data(), 0)
    );
}
```

### Property-Based Tests

```cpp
#include <rapidcheck/gtest.h>

RC_GTEST_PROP(ReluTest, Idempotent, ()) {
    // Property: relu(relu(x)) == relu(x)
    auto size = *rc::gen::inRange<size_t>(1, 1024);
    auto input = *rc::gen::container<std::vector<float>>(size, 
        rc::gen::map(rc::gen::arbitrary<float>(), [](float x) {
            return std::clamp(x, -10.0f, 10.0f);
        }));
    
    hpc::Tensor<float> d_input(size);
    hpc::Tensor<float> d_output1(size);
    hpc::Tensor<float> d_output2(size);
    
    d_input.copy_from_host(input);
    
    hpc::elementwise::relu<float>(d_input.data(), d_output1.data(), size);
    hpc::elementwise::relu<float>(d_output1.data(), d_output2.data(), size);
    cudaDeviceSynchronize();
    
    auto result1 = d_output1.to_host();
    auto result2 = d_output2.to_host();
    
    RC_ASSERT(result1 == result2);
}
```

### Performance Benchmarks

```cpp
TEST(GemmTest, PerformanceBenchmark) {
    constexpr int M = 1024, N = 1024, K = 1024;
    
    hpc::Tensor<float> A(M * K), B(K * N), C(M * N);
    
    hpc::CudaTimer timer;
    constexpr int WARMUP = 10;
    constexpr int ITERATIONS = 100;
    
    // Warmup
    for (int i = 0; i < WARMUP; ++i) {
        hpc::gemm::gemm<float>(A.data(), B.data(), C.data(), M, N, K);
    }
    cudaDeviceSynchronize();
    
    // Benchmark
    timer.start();
    for (int i = 0; i < ITERATIONS; ++i) {
        hpc::gemm::gemm<float>(A.data(), B.data(), C.data(), M, N, K);
    }
    timer.stop();
    
    float avg_time_ms = timer.elapsed_ms() / ITERATIONS;
    float tflops = 2.0 * M * N * K / (avg_time_ms * 1e9);
    
    std::cout << "Average time: " << avg_time_ms << " ms\n";
    std::cout << "Performance: " << tflops << " TFLOPS\n";
}
```

---

## Commit Guidelines

### Conventional Commits

We follow [Conventional Commits](https://www.conventionalcommits.org/):

```
<type>(<scope>): <description>

[optional body]

[optional footer]
```

### Types

| Type | Description | Example |
|------|-------------|---------|
| `feat` | New feature | `feat(gemm): add FP8 support` |
| `fix` | Bug fix | `fix(softmax): handle empty input` |
| `docs` | Documentation | `docs: update README installation steps` |
| `style` | Formatting | `style: fix clang-format issues` |
| `refactor` | Code restructuring | `refactor(tensor): use RAII pattern` |
| `perf` | Performance | `perf(gemm): optimize register usage` |
| `test` | Tests | `test(attention): add property-based tests` |
| `chore` | Build/tooling | `chore: update CUDA version in Docker` |

### Examples

```bash
# Good commits
git commit -m "feat(gemm): add Tensor Core WMMA implementation"
git commit -m "fix(softmax): resolve numerical overflow for large inputs"
git commit -m "docs: add FlashAttention optimization guide"

# With body
git commit -m "perf(gemm): implement double buffering" << 'EOF'
- Add double buffer shared memory
- Overlap memory load with computation
- 1.75x speedup on A100

Benchmarks: M=N=K=4096, time reduced from 2.0ms to 1.14ms
EOF
```

---

## Pull Request Process

### Before Submitting

- [ ] Code compiles without warnings
- [ ] All tests pass locally (`ctest --test-dir build --output-on-failure`)
- [ ] New code has test coverage
- [ ] Documentation updated if needed
- [ ] Commit messages follow conventional commits
- [ ] Branch is up to date with main

### PR Template

```markdown
## Description
Brief description of changes.

## Type of Change
- [ ] Bug fix
- [ ] New feature
- [ ] Documentation update
- [ ] Performance improvement
- [ ] Refactoring

## Testing
Describe tests added/modified.

## Checklist
- [ ] Tests pass locally
- [ ] Documentation updated
- [ ] No compiler warnings
```

### Review Process

1. **Automated Checks**: CI runs formatting, linting, documentation builds
2. **Code Review**: At least one maintainer reviews
3. **Testing**: Reviewer verifies functionality on GPU machine if needed
4. **Approval**: Maintainer approves and merges

### After Merge

- Delete your feature branch
- Update your local main branch
- Look for your contribution in the next release notes

---

## Questions?

- Open a [Discussion](https://github.com/LessUp/hpc-ai-optimization-lab/discussions) for questions
- Check [existing issues](https://github.com/LessUp/hpc-ai-optimization-lab/issues) before creating new ones
- Review [documentation](docs/) for technical details

---

Thank you for contributing! 🎉
