# HPC-AI-Optimization-Lab

[English](README.md) | 简体中文

一个面向 AI 工作负载的 CUDA 算子实验仓库，按模块组织了 elementwise、reduction、GEMM、convolution、attention、quantization，以及实验性的较新 CUDA 路径。

## 仓库内容

- `src/common/`：共享 CUDA 工具，如 tensor 封装、计时器、launch 辅助和归约原语
- `src/01_elementwise/` 到 `src/07_cuda13_features/`：按主题划分的 CUDA 模块
- `tests/`：基于 GoogleTest + RapidCheck 的测试
- `examples/`：当前提供的 CUDA 与 Python 示例
- `python/`：nanobind 绑定和 benchmark 脚本
- `docs/`：优化说明与 Python 绑定文档

## 支持矩阵

### 当前稳定/重点验证范围

- `src/01_elementwise` 到 `src/06_quantization` 下的核心 CUDA kernel
- 基于 CMake 的原生构建与基于 CTest 的验证
- 面向 elementwise、reduction、gemm 子集的轻量 Python 绑定

### 当前实验性 / fallback 范围

以下模块目前更偏教学或兼容性占位，不应视为成熟生产实现：

- `src/04_convolution/conv_winograd.cu`：当前回退到已验证的 implicit GEMM 卷积路径
- `src/07_cuda13_features/tma.cu`：当前是普通 kernel copy fallback
- `src/07_cuda13_features/cluster.cu`：当前是可移植的 block reduction fallback
- `src/07_cuda13_features/fp8_gemm.cu`：当前展示的是缩放 float 行为，而不是真正的 Hopper FP8 kernel

## 构建 C++/CUDA 项目

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j$(nproc)
ctest --test-dir build --output-on-failure
```

## 构建 Python 绑定

当前 Python 扩展名为 `hpc_ai_opt`，暴露 `elementwise`、`reduction`、`gemm` 等底层子模块。

```bash
cmake -S . -B build -DBUILD_PYTHON_BINDINGS=ON
cmake --build build
export PYTHONPATH="$(pwd)/build/python:${PYTHONPATH}"
python -c "import hpc_ai_opt; print(hpc_ai_opt.__doc__)"
python examples/python/basic_usage.py
```

当前绑定是有意保持轻量的：
- 直接接收 CUDA tensor
- 输出 tensor 由调用方分配
- 一些 kernel 需要显式 shape 参数
- wrapper 会做基础参数校验，然后以异步方式发起 CUDA 调用

## 构建示例

```bash
cmake -S . -B build -DBUILD_EXAMPLES=ON
cmake --build build --target relu_example gemm_benchmark
```

## 当前 Python API 形态

```python
import torch
import hpc_ai_opt

x = torch.randn(1024, 1024, device="cuda", dtype=torch.float32)
y = torch.empty_like(x)

hpc_ai_opt.elementwise.relu(x, y)
```

## 环境要求

- CUDA Toolkit 12.4+
- CMake 3.24+
- 支持 C++20 的编译器
- 支持 CUDA 的 NVIDIA GPU
- 若运行 Python 示例，需要带 CUDA 的 PyTorch

说明：
- 当前 Docker 开发环境基于 CUDA 12.4.1。
- `src/07_cuda13_features/` 下的实验模块不代表仓库已经完整覆盖 Hopper/Blackwell 特性。
- 当前 `flash_attention` 实现仅正式支持 `float` 且 `head_dim == 64`。

## CI 与验证范围

默认 GitHub Actions 工作流目前是轻量检查，主要验证：
- 格式
- 仓库/文档一致性检查
- 文档构建

它**不会**在 GitHub-hosted runner 上提供完整的原生 CUDA build + test 覆盖。
若要验证原生 CUDA 功能，请在具备 CUDA 工具链和 GPU 的本地机器上执行上面的 CMake + CTest 流程。

## 文档

- `docs/README.md`
- `docs/python/index.rst`
- `docs/01_gemm_optimization.md`
- `docs/04_flash_attention.md`
