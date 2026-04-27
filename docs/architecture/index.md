# Architecture Overview

This directory keeps high-level architecture context for the current codebase. Active repository workflow and cleanup requirements now live under `openspec/`, while earlier design decisions are preserved in `openspec/archive/`.

---

## Historical design records

- `openspec/archive/2026-04-17-core-architecture/`
- `openspec/archive/2026-04-17-quality-standards/`

These archives preserve the earlier architecture and quality-standard design work that was previously linked through the removed legacy `specs/` tree.

---

## System architecture (high level)

```text
Applications / examples / benchmarks
                |
                v
      Python bindings (nanobind)
                |
                v
  CUDA kernels (elementwise / reduction / gemm /
   convolution / attention / quantization / cuda13)
                |
                v
Common infrastructure (tensor / timer / cuda checks /
types / reduction helpers / launch helpers)
                |
                v
Build and runtime layer (CMake / FetchContent / CUDA / Docker)
```

---

## Core patterns

1. Unified module interfaces for optimized kernel families
2. RAII-based GPU resource management
3. Thin Python bindings over the C++/CUDA core
4. CMake-driven build graph with optional bindings and docs surfaces
