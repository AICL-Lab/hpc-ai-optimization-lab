# HPC-AI-Optimization-Lab - QWEN Context

## Project Overview

**HPC-AI-Optimization-Lab** is an educational and production-ready CUDA kernel library designed for AI inference workloads. It provides step-by-step optimized implementations of critical GPU operations, from basic elementwise operations to advanced Tensor Core matrix multiplication.

### Key Characteristics

- **Purpose**: Learn CUDA optimization from first principles while providing production-ready kernels
- **Target Audience**: Students, researchers, and engineers working on AI/HPC workloads
- **Languages**: C++20, CUDA 12.4+
- **Build System**: CMake 3.24+
- **License**: Apache 2.0

### Core Modules

| Module | Description | Status |
|--------|-------------|--------|
| `elementwise` | ReLU, Sigmoid, VectorAdd, Transpose | ✅ Stable |
| `reduction` | Softmax, LayerNorm, RMSNorm | ✅ Stable |
| `gemm` | 7-step GEMM optimization (flagship module) | ✅ Stable |
| `convolution` | Implicit GEMM, Winograd | ✅ Stable |
| `attention` | FlashAttention, RoPE, TopK | ✅ Stable |
| `quantization` | INT8/FP8 quantization | ✅ Stable |
| `cuda13` | Hopper features (TMA, Clusters, FP8) | 🧪 Experimental |

### Tech Stack

- **Languages**: C++20, CUDA 12.4+
- **Build System**: CMake 3.24+
- **Python Bindings**: nanobind
- **Testing**: GoogleTest (unit tests), RapidCheck (property-based tests)
- **Formatting**: clang-format (Google style), Black (Python)
- **Static Analysis**: clang-tidy
- **Documentation**: Doxygen (C++ API), Sphinx (Python API), VitePress (user docs)
- **CI/CD**: GitHub Actions

---

## Building and Running

### Prerequisites

- CUDA Toolkit 12.4+
- CMake 3.24+
- C++ Compiler: GCC 11+ / Clang 14+
- NVIDIA GPU with Compute Capability 7.0+ (Volta or newer)

### Build Commands

```bash
# Basic build (core library only)
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j$(nproc)

# With examples and Python bindings
cmake -S . -B build \
    -DCMAKE_BUILD_TYPE=Release \
    -DBUILD_EXAMPLES=ON \
    -DBUILD_PYTHON_BINDINGS=ON

# Target specific GPU architectures
cmake -S . -B build -DCMAKE_CUDA_ARCHITECTURES="80;90"  # A100 + H100
```

### Testing

```bash
# Run all tests
ctest --test-dir build --output-on-failure

# Run specific test suite
./build/tests/gemm/test_gemm
```

### Examples

```bash
# Build with examples
cmake -S . -B build -DBUILD_EXAMPLES=ON
cmake --build build

# Run examples
./build/examples/elementwise/relu_example
./build/examples/gemm/gemm_benchmark
```

### Python Bindings

```bash
# Build with Python bindings
cmake -S . -B build -DBUILD_PYTHON_BINDINGS=ON
cmake --build build
export PYTHONPATH="$(pwd)/build/python:${PYTHONPATH}"

# Verify installation
python -c "import hpc_ai_opt; print('Module loaded successfully!')"
```

### Documentation

```bash
# Build VitePress documentation
cd docs && npm install && npm run docs:build

# Build Doxygen API docs
npm run doxygen

# Build Sphinx Python API docs
npm run sphinx
```

---

## Project Structure

```
hpc-ai-optimization-lab/
├── src/                        # CUDA kernel implementations
│   ├── common/                 # Shared utilities (Tensor, Timer, CUDA checks)
│   ├── elementwise/            # ReLU, Sigmoid, VectorAdd, Transpose
│   ├── reduction/              # Softmax, LayerNorm, RMSNorm
│   ├── gemm/                   # 7-step GEMM optimization (flagship!)
│   ├── convolution/            # Implicit GEMM, Winograd
│   ├── attention/              # FlashAttention, RoPE, TopK
│   ├── quantization/           # INT8/FP8 quantization
│   └── cuda13/                 # Hopper features (TMA, Clusters, FP8)
│
├── tests/                      # Comprehensive test suite
│   ├── common/                 # Utility tests
│   ├── elementwise/            # Elementwise tests
│   ├── reduction/              # Reduction tests
│   ├── gemm/                   # GEMM tests (property-based)
│   ├── convolution/            # Convolution tests
│   ├── attention/              # Attention tests
│   ├── quantization/           # Quantization tests
│   └── cuda13/                 # CUDA 13 tests
│
├── examples/                   # Standalone examples
│   ├── elementwise/            # ReLU example
│   ├── reduction/              # Softmax benchmark
│   ├── gemm/                   # GEMM benchmark
│   ├── convolution/            # Conv example
│   ├── attention/              # FlashAttention example
│   ├── quantization/           # Quantization example
│   ├── cuda13/                 # CUDA 13 example
│   └── python/                 # Python usage examples
│
├── python/                     # Python bindings (nanobind)
│   ├── bindings/               # C++ binding code
│   └── benchmark/              # Python benchmarks
│
├── docs/                       # Documentation (VitePress + Doxygen)
│   ├── en/                     # English documentation
│   ├── zh-CN/                  # Chinese documentation
│   └── .vitepress/             # VitePress configuration
│
├── specs/                      # Spec-Driven Development documents
│   ├── product/                # Product requirements
│   ├── rfc/                    # Technical RFCs
│   ├── api/                    # API definitions
│   ├── db/                     # Database schemas
│   └── testing/                # Test tracking
│
├── docker/                     # Docker environment
├── scripts/                    # Utility scripts
├── cmake/                      # CMake helpers
└── .github/                    # CI/CD workflows
```

---

## Development Conventions

### Spec-Driven Development (SDD)

This project strictly follows **Spec-Driven Development**. All code implementations must use specification documents in `/specs/` as the single source of truth.

**AI Agent Workflow:**
1. **Review Specs**: Read `/specs/product/`, `/specs/rfc/`, `/specs/api/` before coding
2. **Spec-First Update**: Propose spec changes before code changes for new features
3. **Code Implementation**: Follow specs 100% (no gold-plating)
4. **Test Against Spec**: Write tests based on spec acceptance criteria

See `AGENTS.md` for detailed AI agent workflow instructions.

### Code Style

**C++/CUDA:**
- Follow Google C++ Style Guide
- Use `clang-format` for formatting (configured in `.clang-format`)
- Use `clang-tidy` for static analysis (configured in `.clang-tidy`)
- Files: `.cu`, `.cuh`, `.cpp`, `.hpp`, `.h`

**Python:**
- Use `Black` for formatting
- Use `isort` for import sorting
- Use `ruff` for linting

**CMake:**
- Use `cmake-format` for formatting

**Markdown:**
- Use `markdownlint` for linting
- Line length limit: 120 characters (relaxed rules for tables and code blocks)

### Pre-commit Hooks

Install pre-commit hooks before development:

```bash
pip install pre-commit
pre-commit install
```

Hooks run automatically on commit and include:
- Trailing whitespace removal
- YAML/JSON validation
- C++ formatting (clang-format)
- Python formatting (Black) and linting (ruff)
- CMake formatting
- Markdown linting
- Shell script linting (shellcheck)
- Commit message validation (commitizen)

### Testing Practices

**Two-Tier Testing Strategy:**

1. **Unit Tests (GoogleTest)**
   - Test all public APIs
   - Test specific input/output cases
   - Run via `ctest`

2. **Property-Based Tests (RapidCheck)**
   - Automatically generate test cases
   - Find edge cases and boundary conditions
   - Test numerical stability
   - Run alongside unit tests

**Test Coverage Targets:**
- Elementwise: 95%+
- Reduction: 90%+
- GEMM: 98%+
- Attention: 92%+
- Overall: 95%+

**Tolerance Guidelines:**
- FP32: 1e-5
- FP16: 1e-3

### Commit Message Format

Follow [Conventional Commits](https://www.conventionalcommits.org/) specification:

```
<type>: <description>

[optional body]

[optional footer(s)]
```

**Types:**
- `feat`: New feature
- `fix`: Bug fix
- `docs`: Documentation changes
- `style`: Code style changes (formatting)
- `refactor`: Code refactoring
- `test`: Test changes
- `chore`: Build/config changes

**Example:**
```
feat: add Tensor Core WMMA support for GEMM
fix: resolve race condition in softmax kernel
docs: update installation guide
```

### Pull Request Process

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/amazing-feature`)
3. Make changes and add tests
4. Ensure tests pass locally (`ctest --test-dir build --output-on-failure`)
5. Ensure code passes pre-commit hooks
6. Commit with conventional commits
7. Push and create a Pull Request

---

## CI/CD Workflows

### CI Workflow (`.github/workflows/ci.yml`)

**Triggers:** Push to master/main, pull requests

**Jobs:**
1. **Format Check**: clang-format validation
2. **Entry Consistency Check**: Verify documented files exist
3. **Docs Build**: VitePress documentation build
4. **CI Success**: All required jobs pass

**Note:** GPU-dependent tests require local execution or self-hosted runners.

### Documentation Workflow (`.github/workflows/pages.yml`)

**Triggers:** Push to master/main (docs/ paths)

**Jobs:**
1. **Build Documentation**: VitePress + Doxygen + Sphinx
2. **Deploy to GitHub Pages**: Deploy built site

---

## Docker Environment

Use pre-configured Docker for hassle-free development:

```bash
cd docker
docker-compose up -d
docker exec -it hpc-ai-lab bash

# Inside container: everything is pre-installed!
cmake -S . -B build && cmake --build build -j$(nproc)
ctest --test-dir build
```

---

## Key Architectural Patterns

### Module Interface Pattern

Each kernel module follows a unified interface:

```cpp
namespace hpc::module {

template <typename T, OptLevel Level = OptLevel::Advanced>
    requires std::is_same_v<T, float> || std::is_same_v<T, __half>
void kernel_name(const T* input, T* output, size_t n,
                 cudaStream_t stream = nullptr);

} // namespace hpc::module
```

### Resource Management

- Use RAII for GPU memory management (see `Tensor<T>` class)
- Delete copy constructors, use move semantics
- Use `CUDA_CHECK` macros for error handling

---

## Important Files

| File | Purpose |
|------|---------|
| `CMakeLists.txt` | Root CMake build configuration |
| `AGENTS.md` | AI agent workflow instructions (SDD) |
| `CONTRIBUTING.md` | Contributor guidelines |
| `.pre-commit-config.yaml` | Pre-commit hooks configuration |
| `.clang-format` | Clang-format configuration (Google style) |
| `.clang-tidy` | Clang-tidy static analysis config |
| `docs/package.json` | VitePress documentation site config |
| `specs/` | Spec-Driven Development documents |

---

## Quick Reference

### Common Tasks

```bash
# Initial setup
git clone <repo> && cd hpc-ai-optimization-lab
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j$(nproc)

# Run tests
ctest --test-dir build --output-on-failure

# Run specific example
./build/examples/gemm/gemm_benchmark

# Format code (if pre-commit not installed)
clang-format -i src/**/*.cu src/**/*.cuh

# Build documentation
cd docs && npm run docs:build
```

### CMake Options

| Option | Default | Description |
|--------|---------|-------------|
| `BUILD_EXAMPLES` | OFF | Build CUDA and Python examples |
| `BUILD_PYTHON_BINDINGS` | OFF | Build nanobind Python module |
| `CMAKE_CUDA_ARCHITECTURES` | native | Target GPU architecture(s) |

---

## Support Matrix

### Production-Ready ✅

| Module | FP32 | FP16 | BF16 | INT8 | FP8 |
|--------|------|------|------|------|-----|
| Elementwise | ✅ | ✅ | ✅ | - | - |
| Reduction | ✅ | ✅ | ✅ | - | - |
| GEMM | ✅ | ✅ | ✅ | ✅ | 🚧 |
| Convolution | ✅ | ✅ | - | - | - |
| Attention | ✅ | ✅ | - | - | - |
| Quantization | ✅ | ✅ | - | ✅ | 🚧 |

### Experimental 🧪

| Feature | Status | Notes |
|---------|--------|-------|
| FP8 GEMM | Demo | Scaled FP16 behavior |
| TMA | Fallback | Async copy instead |
| Thread Block Clusters | Fallback | Block reduction |
| Winograd Conv | Fallback | Implicit GEMM path |

---

## Project Roadmap

### Completed (v0.1.0 - v0.3.0) ✅

- Elementwise operations (4 kernels)
- Reduction operations (3 kernels)
- GEMM optimization (7 steps)
- FlashAttention + RoPE + TopK
- INT8/FP8 quantization
- CUDA 13 Hopper features
- Python bindings (nanobind)
- Comprehensive documentation

### In Progress (v0.4.0) 🚧

- FP8 GEMM (Hopper native)
- Multi-GPU support
- CUTLASS integration
- Performance regression tests

### Planned (v0.5.0+) 🎯

- MoE (Mixture of Experts) support
- Sparse GEMM optimization
- Auto-tuning framework
- PyTorch integration

---

## Links

- **GitHub**: https://github.com/LessUp/hpc-ai-optimization-lab
- **Documentation**: https://lessup.github.io/hpc-ai-optimization-lab/
- **Issues**: https://github.com/LessUp/hpc-ai-optimization-lab/issues
