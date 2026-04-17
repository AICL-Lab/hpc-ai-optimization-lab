# Tutorials

Step-by-step tutorials for learning CUDA kernel optimization techniques.

---

## Beginner Tutorials (1-2 weeks)

| Tutorial | Description | Difficulty |
|----------|-------------|------------|
| [Memory Optimization](../en/guide/memory.md) | Coalesced access, vectorization, shared memory | Star 2 |
| [Reduction Optimization](../en/guide/reduction.md) | Warp shuffle, online algorithms | Star 3 |
| [GEMM Steps 1-4](../en/guide/gemm.md) | Basic to intermediate GEMM optimization | Star 4 |

## Intermediate Tutorials (2-4 weeks)

| Tutorial | Description | Difficulty |
|----------|-------------|------------|
| [GEMM Steps 5-7](../en/guide/gemm.md) | Tensor Core WMMA, MMA PTX, software pipelining | Star 4 |
| [FlashAttention](../en/guide/flash-attention.md) | IO-aware attention, tiling, online softmax | Star 4 |

## Advanced Tutorials (ongoing)

| Tutorial | Description | Difficulty |
|----------|-------------|------------|
| [CUDA 13 Features](../en/guide/cuda13.md) | Hopper architecture: TMA, Clusters, FP8 | Star 5 |
| [Performance Tuning](../en/PERFORMANCE_TUNING.md) | GPU-specific optimization guide | Star 3 |

---

## Recommended Learning Path

```
Beginner (1-2 weeks):
+-- Memory Optimization -> Reduction -> GEMM (Steps 1-4)

Intermediate (2-4 weeks):
+-- GEMM (Steps 5-7) -> FlashAttention

Advanced (ongoing):
+-- CUDA 13 Features -> CUTLASS source code -> Research papers
```

---

## Example Code

All tutorials have corresponding example code in the `examples/` directory:

| Tutorial | Example File |
|----------|--------------|
| Elementwise | `examples/elementwise/relu_example.cu` |
| GEMM | `examples/gemm/gemm_benchmark.cu` |
| Python Usage | `examples/python/basic_usage.py` |

See [Examples README](../../examples/README.md) for build and run instructions.
