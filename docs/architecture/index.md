# Architecture Overview

This directory keeps high-level repository architecture context for maintainers.

Public, reader-facing architecture references live in:

- `docs/en/api/architecture.md`
- `docs/zh-CN/api/architecture.md`

---

## System architecture (high level)

```text
Examples / benchmarks / tests
              |
              v
    Python bindings (nanobind)
              |
              v
 CUDA kernels (elementwise / reduction / gemm /
  convolution / attention / quantization)
              |
              v
 Shared infrastructure (tensor / timer / CUDA checks /
 types / reduction helpers / launch helpers)
              |
              v
 Build + runtime layer (CMake / FetchContent / CUDA / Docker)
```

---

## Maintainer guidance

1. Keep architecture notes tied to shipped code paths, not repository process history.
2. Prefer the bilingual API architecture pages for public explanation and this directory for lightweight maintainer context only.
3. Rewrite or remove stale architecture claims instead of layering new notes over old ones.
