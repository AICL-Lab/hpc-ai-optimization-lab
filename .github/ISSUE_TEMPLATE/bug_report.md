---
name: Bug Report
about: Create a report to help us improve
title: '[BUG] '
labels: bug
assignees: ''
---

## 🐛 Bug Description

A clear and concise description of what the bug is.

## 🔧 Environment

Please fill in the following information:

| Item | Value |
|------|-------|
| **OS** | e.g., Ubuntu 22.04 |
| **CUDA Version** | e.g., 12.4 |
| **GPU** | e.g., RTX 4090 (SM 89) |
| **Driver Version** | e.g., 545.23.08 |
| **CMake Version** | e.g., 3.28 |
| **Compiler** | e.g., GCC 11.4 |
| **Python Version** | e.g., 3.11 (if using bindings) |

## 📋 Steps to Reproduce

1. Clone repository: `git clone https://github.com/LessUp/hpc-ai-optimization-lab.git`
2. Build command: `cmake -S . -B build && cmake --build build`
3. Run command: `...`
4. See error

### Minimal Reproducible Example

```cpp
// If applicable, provide minimal code to reproduce the issue
#include "module/kernel.cuh"

int main() {
    // ...
    return 0;
}
```

## ✅ Expected Behavior

A clear and concise description of what you expected to happen.

## ❌ Actual Behavior

A clear and concise description of what actually happened.

## 📜 Error Output

```
Paste any error messages, warnings, or logs here
```

## 📊 Performance Issue? (if applicable)

If this is a performance-related bug, please include:

| Metric | Expected | Actual |
|--------|----------|--------|
| Execution Time | | |
| TFLOPS | | |
| Memory Bandwidth | | |

## 🔍 Additional Context

Add any other context about the problem here:
- Did this work in a previous version?
- Are you using any special build flags?
- Any workarounds you've tried?

## ✅ Checklist

- [ ] I have searched [existing issues](https://github.com/LessUp/hpc-ai-optimization-lab/issues) to ensure this bug hasn't been reported
- [ ] I have provided all required environment information
- [ ] I have included steps to reproduce the bug
- [ ] I have included relevant error messages/logs
- [ ] I have tested with the latest version on the main branch
