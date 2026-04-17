# Architecture Overview

This directory contains high-level architecture documentation and design decisions. For detailed technical specifications, see the `/specs` directory.

---

## Architecture Documents

| Document | Description | Link |
|----------|-------------|------|
| System Architecture | Complete system architecture design | [ARCHITECTURE.md](../ARCHITECTURE.md) |
| API Reference | All C++/CUDA/Python APIs | [API_REFERENCE.md](../API_REFERENCE.md) |

---

## RFC Documents (Technical Design)

RFCs (Request for Comments) are detailed technical design documents that define the architecture and design decisions.

| RFC | Title | Description |
|-----|-------|-------------|
| [RFC 0001](../../specs/rfc/0001-core-architecture.md) | Core Architecture | System architecture, design patterns, memory strategy, GEMM optimization, performance analysis |
| [RFC 0002](../../specs/rfc/0002-quality-standards.md) | Quality Standards | CI/CD automation, documentation system, code quality tools |

---

## System Architecture (High-Level)

```
Application Layer (Examples, Benchmarks, Python Scripts)
         |
         v
Python Binding Layer (nanobind module)
         |
         v
Kernel Layer (Elementwise, Reduction, GEMM, Convolution, Attention, Quantization, CUDA 13)
         |
         v
Common Infrastructure (Tensor, Timer, CudaCheck, Types, Reduce, Launch)
         |
         v
Build & Runtime (CMake, FetchContent, CUDA 12.4+, Docker)
```

---

## Core Design Patterns

1. **Module Interface Pattern**: Unified template-based interfaces with optimization levels
2. **RAII Resource Management**: Automatic GPU memory management with move semantics
3. **Zero-Copy Python Bindings**: Direct pointer passing via nanobind

For detailed design patterns and implementation guidelines, see [RFC 0001](../../specs/rfc/0001-core-architecture.md).
