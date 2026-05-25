---
name: Feature Request
about: Suggest an idea for this project
title: '[FEATURE] '
labels: enhancement
assignees: ''
---

## 💡 Feature Description

A clear and concise description of the feature you'd like to see implemented.

## 🎯 Problem Statement

Is your feature request related to a problem? Please describe:

**As a** [type of user],  
**I want** [goal/desire],  
**So that** [benefit/reason].

Example: As a CUDA developer learning optimization, I want a tutorial on [topic], so that I can understand [concept].

## 🛠️ Proposed Solution

A clear and concise description of what you want to happen.

### Implementation Ideas (Optional)

If you have ideas about how this could be implemented:

```cpp
// Optional: pseudo-code or API design
namespace hpc::module {
    template<typename T>
    void new_kernel(const T* input, T* output, size_t n);
}
```

## 🔄 Alternatives Considered

A clear and concise description of any alternative solutions or features you've considered.

## 📈 Use Cases

Describe specific use cases for this feature:

1. **Use Case 1**: Description of how this feature would be used
2. **Use Case 2**: Another scenario where this is helpful

## 📋 Affected Modules

Which modules would this feature affect?

- [ ] `src/common/` - Common utilities
- [ ] `src/elementwise/` - Elementwise operations
- [ ] `src/reduction/` - Reduction operations
- [ ] `src/gemm/` - GEMM operations
- [ ] `src/convolution/` - Convolution operations
- [ ] `src/attention/` - Attention mechanisms
- [ ] `src/quantization/` - Quantization utilities
- [ ] `python/` - Python bindings
- [ ] `docs/` - Documentation
- [ ] `tests/` - Testing infrastructure

## 🔗 Related Work

Are there any related:
- Research papers?
- Open source projects (CUTLASS, FlashAttention, etc.)?
- GitHub issues or PRs?

## 📊 Performance Expectations (if applicable)

| Metric | Target |
|--------|--------|
| Throughput | e.g., X TFLOPS |
| Latency | e.g., X ms |
| Memory | e.g., X MB |

## ✅ Checklist

- [ ] I have searched [existing issues](https://github.com/LessUp/hpc-ai-optimization-lab/issues) to ensure this feature hasn't been requested
- [ ] I have clearly described the problem this feature would solve
- [ ] I have provided a clear description of the proposed solution
- [ ] I have identified which modules would be affected
