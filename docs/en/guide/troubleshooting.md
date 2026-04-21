# Troubleshooting Guide | 故障排除指南

This guide covers common issues and solutions when building and using HPC-AI-Optimization-Lab.

---

## Table of Contents

- [Build Issues](#build-issues)
- [Runtime Issues](#runtime-issues)
- [Performance Issues](#performance-issues)
- [Python Binding Issues](#python-binding-issues)
- [CUDA Errors](#cuda-errors)

---

## Build Issues

### CMake Configuration Errors

#### "Could not find CUDA"

**Symptoms:**
```
CMake Error: Could not find CUDA
```

**Solutions:**
1. Verify CUDA Toolkit is installed:
   ```bash
   nvcc --version  # Should show CUDA 12.4+
   ```
2. Set CUDA path explicitly:
   ```bash
   export CUDA_HOME=/usr/local/cuda
   cmake -S . -B build -DCUDA_TOOLKIT_ROOT_DIR=$CUDA_HOME
   ```
3. Check PATH includes CUDA:
   ```bash
   echo $PATH  # Should include /usr/local/cuda/bin
   ```

---

#### "CMake version too old"

**Symptoms:**
```
CMake Error: CMake was unable to find a build program corresponding to "Unix Makefiles"
```

**Solutions:**
1. Install CMake 3.24+:
   ```bash
   # Ubuntu/Debian
   wget https://github.com/Kitware/CMake/releases/download/v3.28.0/cmake-3.28.0-linux-x86_64.sh
   chmod +x cmake-*.sh
   sudo ./cmake-*.sh --prefix=/usr/local
   ```
2. Or use pip:
   ```bash
   pip install cmake --upgrade
   ```

---

#### "Compiler doesn't support C++20"

**Symptoms:**
```
error: unrecognized command line option '-std=c++20'
```

**Solutions:**
1. Upgrade GCC to 11+:
   ```bash
   # Ubuntu 22.04+
   sudo apt install g++-11
   export CXX=g++-11
   ```
2. Or use Clang 14+:
   ```bash
   sudo apt install clang-14
   export CXX=clang++-14
   ```

---

### Compilation Errors

#### "Tensor Core requires SM 7.0+"

**Symptoms:**
```
error: identifier "wmma::load_matrix_sync" is undefined
```

**Solutions:**
1. Specify GPU architecture explicitly:
   ```bash
   cmake -S . -B build -DCMAKE_CUDA_ARCHITECTURES="80;90"
   ```
2. Or check your GPU:
   ```bash
   nvidia-smi --query-gpu=compute_cap --format=csv
   ```

---

#### "Shared memory size exceeds limit"

**Symptoms:**
```
error: shared memory array size exceeds maximum
```

**Solutions:**
1. Reduce tile size in kernel configuration
2. Use dynamic shared memory:
   ```cpp
   extern __shared__ float smem[];
   ```
3. Check GPU shared memory limits:
   ```bash
   nvidia-smi -q -d MEMORY | grep "Shared"
   ```

---

## Runtime Issues

### "Invalid device ordinal"

**Symptoms:**
```
CUDA error: invalid device ordinal
```

**Solutions:**
1. Check available GPUs:
   ```bash
   nvidia-smi -L
   ```
2. Set visible devices:
   ```bash
   export CUDA_VISIBLE_DEVICES=0
   ```

---

### "Out of memory"

**Symptoms:**
```
CUDA error: out of memory
```

**Solutions:**
1. Reduce batch/tensor size
2. Check GPU memory:
   ```bash
   nvidia-smi
   ```
3. Use memory pool (CUDA 11.2+):
   ```cpp
   cudaMemPool_t pool;
   cudaDeviceGetDefaultMemPool(&pool, 0);
   ```

---

### Kernel Launch Failures

#### "Launch out of resources"

**Solutions:**
1. Reduce block size:
   ```cpp
   // Instead of 1024 threads
   dim3 block(256);  // Use smaller block
   ```
2. Check register usage:
   ```bash
   nvcc --ptxas-options=-v kernel.cu
   ```

---

## Performance Issues

### Low Performance on Tensor Core Kernels

**Symptoms:** TFLOPS much lower than expected

**Solutions:**
1. Ensure dimensions are multiples of 16:
   ```cpp
   // Tensor Core requires M, N, K divisible by 16
   int M = ((M + 15) / 16) * 16;  // Pad to multiple of 16
   ```
2. Verify FP16 input:
   ```cpp
   // Tensor Core requires __half input
   hpc::Tensor<__half> A(M * K);  // Not float
   ```
3. Check occupancy:
   ```bash
   nvprof --metrics achieved_occupancy ./program
   ```

---

### Bank Conflicts

**Symptoms:** Unexpected slowdown in shared memory operations

**Solutions:**
1. Add padding to shared memory arrays:
   ```cpp
   __shared__ float tile[32][33];  // +1 for bank conflict avoidance
   ```
2. Profile with Nsight Compute:
   ```bash
   ncu --metrics l1tex__data_bank_conflicts_pipe_lsu_mem_shared_op_ld ./program
   ```

---

## Python Binding Issues

### "No module named 'hpc_ai_opt'"

**Solutions:**
1. Build with Python bindings:
   ```bash
   cmake -S . -B build -DBUILD_PYTHON_BINDINGS=ON
   cmake --build build
   ```
2. Set PYTHONPATH:
   ```bash
   export PYTHONPATH="$(pwd)/build/python:${PYTHONPATH}"
   ```
3. Verify:
   ```python
   import hpc_ai_opt
   print(hpc_ai_opt.__doc__)
   ```

---

### "PyTorch tensors must be on CUDA"

**Symptoms:**
```
ValueError: Tensors must be on CUDA device
```

**Solutions:**
```python
# Wrong
x = torch.randn(1024)  # CPU tensor

# Correct
x = torch.randn(1024, device="cuda")  # GPU tensor
```

---

### NaN or Incorrect Results

**Solutions:**
1. Check tensor dtypes match:
   ```python
   x = torch.randn(1024, device="cuda", dtype=torch.float32)
   y = torch.empty_like(x)  # Same dtype and device
   ```
2. Verify dimensions:
   ```python
   # FlashAttention requires head_dim=64
   config = {
       'head_dim': 64,  # Must be 64
       ...
   }
   ```

---

## CUDA Errors

### Error Code Reference

| Error Code | Description | Common Cause |
|------------|-------------|--------------|
| 1 | Invalid value | Bad parameter |
| 2 | Out of memory | GPU memory exhausted |
| 8 | Invalid device ordinal | Wrong GPU ID |
| 9 | Invalid kernel image | Architecture mismatch |
| 30 | Unknown error | Usually driver issue |

---

### "CUDA driver version is insufficient"

**Solutions:**
1. Check driver version:
   ```bash
   nvidia-smi  # Look for "Driver Version"
   ```
2. Update driver:
   ```bash
   # Ubuntu
   sudo apt install nvidia-driver-535
   ```
3. Match CUDA version:
   | CUDA | Min Driver |
   |------|------------|
   | 12.4 | 550.54+ |
   | 12.3 | 545.23+ |
   | 12.2 | 535.54+ |

---

### "CUDA capability not supported"

**Solutions:**
1. Check GPU architecture:
   ```bash
   nvidia-smi --query-gpu=compute_cap --format=csv
   ```
2. Build for correct architecture:
   ```bash
   # For A100 (SM 8.0)
   cmake -S . -B build -DCMAKE_CUDA_ARCHITECTURES="80"
   
   # For H100 (SM 9.0)
   cmake -S . -B build -DCMAKE_CUDA_ARCHITECTURES="90"
   ```

---

## Debugging Tips

### Enable CUDA Error Checking

```cpp
#define CUDA_CHECK(call) \
    do { \
        cudaError_t err = call; \
        if (err != cudaSuccess) { \
            fprintf(stderr, "CUDA error at %s:%d: %s\n", \
                    __FILE__, __LINE__, cudaGetErrorString(err)); \
            exit(EXIT_FAILURE); \
        } \
    } while (0)

// Use after kernel launch
kernel<<<grid, block>>>(args);
CUDA_CHECK(cudaGetLastError());
CUDA_CHECK(cudaDeviceSynchronize());
```

---

### Use Compute Sanitizer

```bash
# Check for memory errors
compute-sanitizer ./build/tests/gemm/test_gemm

# Check for race conditions
compute-sanitizer --tool racecheck ./program

# Check for memory leaks
compute-sanitizer --tool memcheck ./program
```

---

### Use Nsight Compute for Profiling

```bash
# Detailed kernel analysis
ncu --set full -o profile ./program

# Focus on specific metrics
ncu --metrics gpu__time_duration.sum ./program

# Compare kernels
ncu --set basic ./program
```

---

### Use Nsight Systems for Timeline

```bash
# System-wide profiling
nsys profile -o timeline ./program

# View results
nsys-ui timeline.nsys-rep
```

---

## Getting Help

If your issue isn't covered here:

1. **Search existing issues**: [GitHub Issues](https://github.com/LessUp/hpc-ai-optimization-lab/issues)
2. **Check documentation**: [docs/](docs/)
3. **Ask in discussions**: [GitHub Discussions](https://github.com/LessUp/hpc-ai-optimization-lab/discussions)
4. **Report a bug**: Use the [Bug Report Template](.github/ISSUE_TEMPLATE/bug_report.md)

When reporting, please include:
- OS and version
- CUDA version (`nvcc --version`)
- GPU model and driver (`nvidia-smi`)
- CMake configuration output
- Full error message
- Minimal reproduction code

---

## FAQ

### Q: Can I use this without a GPU?

A: No. This library requires an NVIDIA GPU with Compute Capability 7.0+. All kernels execute on the GPU.

### Q: Why is my kernel slower than expected?

A: Common reasons:
- Wrong GPU architecture (compile for your GPU)
- Non-optimal dimensions (pad to multiples of 16 for Tensor Core)
- Low occupancy (reduce register usage)
- Bank conflicts (add padding)

### Q: Does this work on Windows?

A: Yes, with Visual Studio 2022+ and CUDA 12.4+. Use CMake GUI or Developer Command Prompt.

### Q: Can I use this with PyTorch?

A: Yes! Build Python bindings and pass PyTorch CUDA tensors directly:
```python
import torch
import hpc_ai_opt

x = torch.randn(1024, device="cuda")
y = torch.empty_like(x)
hpc_ai_opt.elementwise.relu(x, y)
```

---

Still stuck? [Open an issue](https://github.com/LessUp/hpc-ai-optimization-lab/issues/new) and we'll help!
