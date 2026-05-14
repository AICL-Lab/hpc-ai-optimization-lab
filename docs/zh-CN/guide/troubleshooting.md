# 故障排除指南

本指南涵盖了构建和使用 HPC-AI-Optimization-Lab 时的常见问题和解决方案。

---

## 目录

- [编译问题](#编译问题)
- [运行时问题](#运行时问题)
- [性能问题](#性能问题)
- [Python 绑定问题](#python-绑定问题)
- [CUDA 错误](#cuda-错误)

---

## 编译问题

### CMake 配置错误

#### "Could not find CUDA"

**症状：**
```
CMake Error: Could not find CUDA
```

**解决方案：**
1. 验证 CUDA Toolkit 已安装：
   ```bash
   nvcc --version  # 应显示 CUDA 12.4+
   ```
2. 显式设置 CUDA 路径：
   ```bash
   export CUDA_HOME=/usr/local/cuda
   cmake -S . -B build -DCUDA_TOOLKIT_ROOT_DIR=$CUDA_HOME
   ```
3. 检查 PATH 是否包含 CUDA：
   ```bash
   echo $PATH  # 应包含 /usr/local/cuda/bin
   ```

---

#### "CMake version too old"

**症状：**
```
CMake Error: CMake was unable to find a build program corresponding to "Unix Makefiles"
```

**解决方案：**
1. 安装 CMake 3.24+：
   ```bash
   # Ubuntu/Debian
   wget https://github.com/Kitware/CMake/releases/download/v3.28.0/cmake-3.28.0-linux-x86_64.sh
   chmod +x cmake-*.sh
   sudo ./cmake-*.sh --prefix=/usr/local
   ```
2. 或使用 pip：
   ```bash
   pip install cmake --upgrade
   ```

---

#### "Compiler doesn't support C++20"

**症状：**
```
error: unrecognized command line option '-std=c++20'
```

**解决方案：**
1. 升级 GCC 到 11+：
   ```bash
   # Ubuntu 22.04+
   sudo apt install g++-11
   export CXX=g++-11
   ```
2. 或使用 Clang 14+：
   ```bash
   sudo apt install clang-14
   export CXX=clang++-14
   ```

---

### 编译错误

#### "Tensor Core requires SM 7.0+"

**症状：**
```
error: identifier "wmma::load_matrix_sync" is undefined
```

**解决方案：**
1. 显式指定 GPU 架构：
   ```bash
   cmake -S . -B build -DCMAKE_CUDA_ARCHITECTURES="80;90"
   ```
2. 或检查您的 GPU：
   ```bash
   nvidia-smi --query-gpu=compute_cap --format=csv
   ```

---

#### "Shared memory size exceeds limit"

**症状：**
```
error: shared memory array size exceeds maximum
```

**解决方案：**
1. 减小内核配置中的 tile 大小
2. 使用动态共享内存：
   ```cpp
   extern __shared__ float smem[];
   ```
3. 检查 GPU 共享内存限制：
   ```bash
   nvidia-smi -q -d MEMORY | grep "Shared"
   ```

---

## 运行时问题

### "Invalid device ordinal"

**症状：**
```
CUDA error: invalid device ordinal
```

**解决方案：**
1. 检查可用的 GPU：
   ```bash
   nvidia-smi -L
   ```
2. 设置可见设备：
   ```bash
   export CUDA_VISIBLE_DEVICES=0
   ```

---

### "Out of memory"

**症状：**
```
CUDA error: out of memory
```

**解决方案：**
1. 减小 batch/tensor 大小
2. 检查 GPU 内存：
   ```bash
   nvidia-smi
   ```
3. 使用内存池（CUDA 11.2+）：
   ```cpp
   cudaMemPool_t pool;
   cudaDeviceGetDefaultMemPool(&pool, 0);
   ```

---

### 内核启动失败

#### "Launch out of resources"

**解决方案：**
1. 减小 block 大小：
   ```cpp
   // 不用 1024 线程
   dim3 block(256);  // 使用更小的 block
   ```
2. 检查寄存器使用：
   ```bash
   nvcc --ptxas-options=-v kernel.cu
   ```

---

## 性能问题

### Tensor Core 内核性能低

**症状：** TFLOPS 远低于预期

**解决方案：**
1. 确保维度是 16 的倍数：
   ```cpp
   // Tensor Core 要求 M, N, K 能被 16 整除
   int M = ((M + 15) / 16) * 16;  // 填充到 16 的倍数
   ```
2. 验证 FP16 输入：
   ```cpp
   // Tensor Core 需要 __half 输入
   hpc::Tensor<__half> A(M * K);  // 不是 float
   ```
3. 检查占用率：
   ```bash
   nvprof --metrics achieved_occupancy ./program
   ```

---

### Bank Conflicts

**症状：** 共享内存操作意外减速

**解决方案：**
1. 为共享内存数组添加填充：
   ```cpp
   __shared__ float tile[32][33];  // +1 避免 bank conflict
   ```
2. 使用 Nsight Compute 分析：
   ```bash
   ncu --metrics l1tex__data_bank_conflicts_pipe_lsu_mem_shared_op_ld ./program
   ```

---

## Python 绑定问题

### "No module named 'hpc_ai_opt'"

**解决方案：**
1. 使用 Python 绑定构建：
   ```bash
   cmake -S . -B build -DBUILD_PYTHON_BINDINGS=ON
   cmake --build build
   ```
2. 设置 PYTHONPATH：
   ```bash
   export PYTHONPATH="$(pwd)/build/python:${PYTHONPATH}"
   ```
3. 验证：
   ```python
   import hpc_ai_opt
   print(hpc_ai_opt.__doc__)
   ```

---

### "PyTorch tensors must be on CUDA"

**症状：**
```
ValueError: Tensors must be on CUDA device
```

**解决方案：**
```python
# 错误
x = torch.randn(1024)  # CPU tensor

# 正确
x = torch.randn(1024, device="cuda")  # GPU tensor
```

---

### NaN 或错误结果

**解决方案：**
1. 检查 tensor dtypes 是否匹配：
   ```python
   x = torch.randn(1024, device="cuda", dtype=torch.float32)
   y = torch.empty_like(x)  # 相同的 dtype 和 device
   ```
2. 验证维度：
   ```python
   # FlashAttention 要求 head_dim=64
   config = {
       'head_dim': 64,  # 必须是 64
       ...
   }
   ```

---

## CUDA 错误

### 错误代码参考

| 错误代码 | 描述 | 常见原因 |
|----------|------|----------|
| 1 | 无效值 | 参数错误 |
| 2 | 内存不足 | GPU 内存耗尽 |
| 8 | 无效设备序号 | 错误的 GPU ID |
| 9 | 无效内核镜像 | 架构不匹配 |
| 30 | 未知错误 | 通常是驱动问题 |

---

### "CUDA driver version is insufficient"

**解决方案：**
1. 检查驱动版本：
   ```bash
   nvidia-smi  # 查看 "Driver Version"
   ```
2. 更新驱动：
   ```bash
   # Ubuntu
   sudo apt install nvidia-driver-535
   ```
3. 匹配 CUDA 版本：
   | CUDA | 最低驱动 |
   |------|----------|
   | 12.4 | 550.54+ |
   | 12.3 | 545.23+ |
   | 12.2 | 535.54+ |

---

### "CUDA capability not supported"

**解决方案：**
1. 检查 GPU 架构：
   ```bash
   nvidia-smi --query-gpu=compute_cap --format=csv
   ```
2. 为正确的架构构建：
   ```bash
   # A100 (SM 8.0)
   cmake -S . -B build -DCMAKE_CUDA_ARCHITECTURES="80"
   
   # H100 (SM 9.0)
   cmake -S . -B build -DCMAKE_CUDA_ARCHITECTURES="90"
   ```

---

## 调试技巧

### 启用 CUDA 错误检查

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

// 内核启动后使用
kernel<<<grid, block>>>(args);
CUDA_CHECK(cudaGetLastError());
CUDA_CHECK(cudaDeviceSynchronize());
```

---

### 使用 Compute Sanitizer

```bash
# 检查内存错误
compute-sanitizer ./build/tests/gemm/test_gemm

# 检查竞态条件
compute-sanitizer --tool racecheck ./program

# 检查内存泄漏
compute-sanitizer --tool memcheck ./program
```

---

### 使用 Nsight Compute 进行分析

```bash
# 详细内核分析
ncu --set full -o profile ./program

# 关注特定指标
ncu --metrics gpu__time_duration.sum ./program

# 比较内核
ncu --set basic ./program
```

---

### 使用 Nsight Systems 查看时间线

```bash
# 系统级分析
nsys profile -o timeline ./program

# 查看结果
nsys-ui timeline.nsys-rep
```

---

## 获取帮助

如果您的问题未在此处涵盖：

1. **搜索现有问题**：[GitHub Issues](https://github.com/AICL-Lab/hpc-ai-optimization-lab/issues)
2. **查阅文档**：[文档](https://lessup.github.io/hpc-ai-optimization-lab/)
3. **在讨论中提问**：[GitHub Discussions](https://github.com/AICL-Lab/hpc-ai-optimization-lab/discussions)
4. **报告 bug**：使用 [Bug 报告模板](https://github.com/AICL-Lab/hpc-ai-optimization-lab/blob/master/.github/ISSUE_TEMPLATE/bug_report.md)

报告时请包含：
- 操作系统和版本
- CUDA 版本（`nvcc --version`）
- GPU 型号和驱动（`nvidia-smi`）
- CMake 配置输出
- 完整的错误信息
- 最小复现代码

---

## 常见问题

### 问：我可以在没有 GPU 的情况下使用吗？

答：不可以。本库需要具有 Compute Capability 7.0+ 的 NVIDIA GPU。所有内核都在 GPU 上执行。

### 问：为什么我的内核比预期的慢？

答：常见原因：
- GPU 架构错误（为您的 GPU 编译）
- 非最佳维度（为 Tensor Core 填充到 16 的倍数）
- 低占用率（减少寄存器使用）
- Bank conflicts（添加填充）

### 问：这在 Windows 上可用吗？

答：可以，需要 Visual Studio 2022+ 和 CUDA 12.4+。使用 CMake GUI 或 Developer Command Prompt。

### 问：我可以与 PyTorch 一起使用吗？

答：可以！构建 Python 绑定并直接传递 PyTorch CUDA tensors：
```python
import torch
import hpc_ai_opt

x = torch.randn(1024, device="cuda")
y = torch.empty_like(x)
hpc_ai_opt.elementwise.relu(x, y)
```

---

还有问题？[提交一个 issue](https://github.com/AICL-Lab/hpc-ai-optimization-lab/issues/new)，我们会帮助您！
