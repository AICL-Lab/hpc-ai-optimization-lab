# Profiling Guide

Learn how to profile and analyze CUDA kernels in HPC-AI-Optimization-Lab.

## Overview

Profiling is essential for understanding kernel performance and identifying optimization opportunities. This guide covers the profiling tools and techniques used in this project.

## Built-in Profiling Tools

### CudaTimer

The project includes a lightweight `CudaTimer` utility:

```cpp
#include "common/timer.cuh"

void profile_kernel() {
    hpc::common::CudaTimer timer;
    timer.Start();
    
    // Launch your kernel
    my_kernel<<<blocks, threads, 0, stream>>>(...);
    
    timer.Stop();
    float elapsed_ms = timer.ElapsedMillis();
    
    printf("Kernel execution time: %.3f ms\n", elapsed_ms);
}
```

### Performance Counters

The library provides built-in performance metrics:

```cpp
// TFLOPS calculation for GEMM
float tflops = (2.0f * M * N * K) / (elapsed_ms * 1e9f);
printf("Performance: %.2f TFLOPS\n", tflops);
```

## NVIDIA Nsight Tools

### Nsight Compute (ncu)

Profile individual kernels:

```bash
# Profile a specific kernel
ncu --target-processes all \
    ./bin/examples/gemm_benchmark

# Focus on specific metrics
ncu --metrics \
    smsp__throughput.avg.pct_of_peak_sustained_elapsed,\
    l1tex__t_sectors_pipe_lsu_mem_global_op_ld.sum,\
    l1tex__t_sectors_pipe_lsu_mem_global_op_st.sum \
    ./bin/examples/gemm_benchmark
```

#### Key Metrics to Monitor

| Metric | What It Tells You | Target |
|--------|-------------------|--------|
| `smsp__throughput.avg` | Compute utilization | >70% |
| `l1tex__t_sectors` | Memory traffic | Minimize |
| `achieved_occupancy` | Active warps/SM | >50% |
| `inst_issue_per_cycle` | IPC | >2 |

### Nsight Systems (nsys)

Profile full application timelines:

```bash
# Profile the entire application
nsys profile --stats=true \
    -o gemm_profile \
    ./bin/examples/gemm_benchmark

# View timeline
nsys-ui gemm_profile.nsys-rep
```

#### What to Look For

1. **GPU Utilization Gaps**: Look for idle periods between kernels
2. **Data Transfer Bottlenecks**: Identify CPU↔GPU transfer time
3. **Kernel Serialization**: Check if kernels run sequentially when they could overlap

## CUDA Compute Sanitizer

Debug memory issues and race conditions:

```bash
# Check for memory errors
compute-sanitizer --tool memcheck ./bin/tests/test_gemm

# Check for race conditions
compute-sanitizer --tool racecheck ./bin/tests/test_reduction
```

## Profiling Workflow

### Step-by-Step Process

1. **Baseline Measurement**
   ```bash
   # Run without profiling to get baseline
   ./bin/examples/gemm_benchmark
   ```

2. **Identify Bottleneck**
   ```bash
   # Profile with Nsight Compute
   ncu ./bin/examples/gemm_benchmark
   ```

3. **Analyze Metrics**
   - Check compute throughput
   - Analyze memory bandwidth
   - Review occupancy

4. **Implement Optimization**
   - Apply tiling, vectorization, or other techniques
   - See [Performance Tuning Guide](../PERFORMANCE_TUNING.md)

5. **Verify Improvement**
   ```bash
   # Compare before/after
   ./bin/examples/gemm_benchmark
   ```

## Architecture-Specific Profiling

### Volta (V100)

```bash
# Focus on Tensor Core usage
ncu --metrics \
    hma__type_fp16.sum,\
    hma__fp16_active.avg.pct_of_peak_sustained_elapsed \
    ./bin/examples/gemm_benchmark
```

### Ampere (A100)

```bash
# Check for sparsity utilization
ncu --metrics \
    smsp__inst_executed_op_sp.sum,\
    sm__pipe_tensor_op_hmma_cycles_active.sum \
    ./bin/examples/gemm_benchmark
```

### Hopper (H100)

```bash
# Profile TMA and WGMMA
ncu --metrics \
    l1tex__t_mla_requests.sum,\
    smsp__pipe_wgmma_fmac_active.avg.pct_of_peak_sustained_elapsed \
    ./bin/examples/gemm_benchmark
```

## Performance Benchmarks

Reference performance numbers (A100, FP32):

| Kernel | Size | Time (ms) | TFLOPS | % Peak |
|--------|------|-----------|--------|--------|
| Naive GEMM | 4096 | 85.2 | 3.2 | 16% |
| Tiled GEMM | 4096 | 28.5 | 9.6 | 48% |
| cuBLAS | 4096 | 8.1 | 33.8 | 100% |

## Tips for Accurate Profiling

1. **Warm up the GPU**: Run 2-3 iterations before measuring
2. **Disable GPU clocks**: Use `nvidia-smi -lgc` to lock clocks
3. **Run multiple times**: Take the median, not average
4. **Isolate the GPU**: Close other GPU processes
5. **Use release builds**: Debug builds are 10-100x slower

## Further Reading

- [NVIDIA Nsight Compute Documentation](https://docs.nvidia.com/nsight-compute/)
- [CUDA Best Practices Guide](https://docs.nvidia.com/cuda/cuda-c-best-practices-guide/)
- [Performance Tuning Guide](../PERFORMANCE_TUNING.md)
