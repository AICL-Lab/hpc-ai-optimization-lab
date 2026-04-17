# 📚 HPC-AI-Optimization-Lab 文档中心

欢迎来到 HPC-AI-Optimization-Lab 的完整文档！本文档为 AI 工作负载中使用的 CUDA 内核优化技术提供深入指南。

---

## 🌍 语言 / Language

**简体中文** | [English](../en/README.md)

---

## 🚀 快速导航

### 入门指南

| 资源 | 描述 |
|------|------|
| [项目 README](../../README.zh-CN.md) | 项目概览、构建说明、快速开始 |
| [架构概览](ARCHITECTURE.md) | 设计模式、模块组织 |
| [API 参考](API_REFERENCE.md) | 完整的 API 文档 |
| [示例代码](../../examples/README.md) | 可运行的代码示例 |

### 技术教程

| 文档 | 主题 | 难度 | 预计时间 |
|------|------|------|----------|
| [访存优化](02_memory_optimization.md) | 合并访问、向量化、共享内存 | ⭐⭐ | 2-3 小时 |
| [GEMM 优化](01_gemm_optimization.md) | 7 步矩阵乘法优化 | ⭐⭐⭐⭐ | 4-6 小时 |
| [归约优化](03_reduction_optimization.md) | Warp 洗牌、在线 Softmax、LayerNorm | ⭐⭐⭐ | 3-4 小时 |
| [FlashAttention](04_flash_attention.md) | IO 感知的注意力机制、分块、在线 Softmax | ⭐⭐⭐⭐ | 4-5 小时 |
| [CUDA 13 特性](05_cuda13_features.md) | Hopper 架构：TMA、集群、FP8 | ⭐⭐⭐⭐⭐ | 5-6 小时 |

---

## 📖 学习路径

### 🌱 初学者路径（1-2 周）

**目标**: 理解 CUDA 内存模型和基础优化技术

```
第 1 步：访存优化
        └── 合并访问模式
        └── 向量化加载 (float4)
        └── Grid Stride 循环
        
第 2 步：归约操作
        └── Warp 洗牌原语
        └── 块级归约
        └── 在线 Softmax
        
第 3 步：GEMM 基础
        └── 朴素实现
        └── 共享内存分块 (步骤 1-2)
```

**推荐阅读顺序**:
1. [访存优化](02_memory_optimization.md)
2. [归约优化](03_reduction_optimization.md)
3. [GEMM 优化](01_gemm_optimization.md) (步骤 1-4)

### 🔥 进阶路径（2-4 周）

**目标**: 掌握 Tensor Core 使用和高级技术

```
第 1 步：GEMM 高级
        └── 双缓冲 (步骤 3)
        └── 寄存器分块 (步骤 4)
        └── Tensor Core WMMA (步骤 5)
        
第 2 步：FlashAttention
        └── IO 复杂度分析
        └── 分块策略
        └── 在线 Softmax 集成
```

**推荐阅读顺序**:
1. [GEMM 优化](01_gemm_optimization.md) (步骤 5-7)
2. [FlashAttention](04_flash_attention.md)

### 🚀 专家路径（持续学习）

**目标**: 前沿技术和研究

```
第 1 步：Hopper 架构
        └── TMA (张量内存加速器)
        └── 线程块集群
        └── FP8 量化
        
第 2 步：高级主题
        └── CUTLASS 源码分析
        └── 自定义内核开发
```

**推荐阅读顺序**:
1. [CUDA 13 特性](05_cuda13_features.md)
2. 外部资源: [CUTLASS 文档](https://github.com/NVIDIA/cutlass)
3. 外部资源: [FlashAttention-2 论文](https://arxiv.org/abs/2307.08691)

---

## 🛠️ 快速参考

### 优化技术总结

| 技术 | 使用场景 | 预期加速 | 文档 |
|------|----------|----------|------|
| 合并访问 (Coalesced Access) | 所有内核 | 2-10× | [访存优化](02_memory_optimization.md) |
| 向量化 (float4) | 逐元素操作 | 1.5-2× | [访存优化](02_memory_optimization.md) |
| Grid Stride 循环 | 大数据量 | 1.2-1.5× | [访存优化](02_memory_optimization.md) |
| 共享内存 | 数据复用 | 2-5× | [GEMM 优化](01_gemm_optimization.md) |
| Warp 洗牌 | 归约操作 | 5-10× | [归约优化](03_reduction_optimization.md) |
| Tensor Core | 矩阵运算 | 10-20× | [GEMM 优化](01_gemm_optimization.md) |
| TMA | 大批量数据拷贝 | 1.5-2× | [CUDA 13 特性](05_cuda13_features.md) |
| 在线 Softmax | 注意力机制 | 3-5× | [FlashAttention](04_flash_attention.md) |

### 性能分析命令

```bash
# Nsight Compute - 内核级分析
ncu --set full -o profile ./your_app

# Nsight Systems - 系统时间线
nsys profile -o timeline ./your_app

# Roofline 分析
ncu --set roofline -o roofline ./your_app

# 内存吞吐分析
ncu --metrics gpu__time_duration.sum,dram__throughput.avg.pct_of_peak_sustained_elapsed \
    ./your_app
```

### 常见问题与解决方案

| 问题 | 原因 | 解决方案 | 参考 |
|------|------|----------|------|
| 低占用率 (Low occupancy) | 寄存器使用过多 | 减少寄存器使用，使用共享内存 | 访存优化 §4 |
| 存储体冲突 (Bank conflicts) | 共享内存访问模式问题 | 添加填充 (Padding) | 访存优化 §4 |
| 带宽利用率低 | 非合并访问 | 重构内存访问模式 | 访存优化 §1 |
| Tensor Core 利用率低 | 维度不是 16 的倍数 | 将矩阵填充为 16 的倍数 | GEMM 优化 §5 |
| 数值不稳定 | 浮点精度问题 | 使用 Welford 算法 | 归约优化 §5 |

---

## 🏗️ 构建文档

### 前置要求

```bash
# Doxygen - C++/CUDA API 文档
sudo apt-get install doxygen graphviz

# Sphinx - Python API 文档
pip install sphinx sphinx-rtd-theme breathe myst-parser sphinx-copybutton
```

### 生成文档

```bash
# C++/CUDA API 文档
doxygen Doxyfile
# 输出位置: docs/api/html/

# Python API 文档
cd python
sphinx-build -b html . _build/html
# 输出位置: python/_build/html/
```

### 本地预览

```bash
# C++ API 文档
python -m http.server 8000 -d api/html

# Python API 文档
python -m http.server 8001 -d python/_build/html
```

---

## 📚 外部资源

### NVIDIA 官方文档

- [CUDA C++ 编程指南](https://docs.nvidia.com/cuda/cuda-c-programming-guide/)
- [CUDA C++ 最佳实践指南](https://docs.nvidia.com/cuda/cuda-c-best-practices-guide/)
- [CUDA 优化指南](https://docs.nvidia.com/cuda/cuda-optimization-guide/)
- [CUTLASS 文档](https://github.com/NVIDIA/cutlass/tree/main/media/docs)

### 基础论文

| 论文 | 主题 | 链接 |
|------|------|------|
| FlashAttention | IO 感知的注意力机制 | [arXiv:2205.14135](https://arxiv.org/abs/2205.14135) |
| FlashAttention-2 | 并行度改进 | [arXiv:2307.08691](https://arxiv.org/abs/2307.08691) |
| Online Softmax | 单次遍历 Softmax | [arXiv:1805.02867](https://arxiv.org/abs/1805.02867) |

### 推荐博客与教程

- [如何优化 CUDA 矩阵乘法内核](https://siboehm.com/articles/22/CUDA-MMM) - 循序渐进的 GEMM 优化
- [NVIDIA 开发者博客](https://developer.nvidia.com/blog/) - 官方教程
- [Mark Harris 的 CUDA 优化](https://developer.download.nvidia.com/assets/cuda/files/reduction.pdf) - 经典归约优化

---

## 🤝 贡献文档

我们欢迎文档改进！贡献时请遵循：

1. **格式**: 使用带适当标题的 Markdown
2. **代码示例**: 包含可编译、可运行的代码片段
3. **性能数据**: 适当添加基准数据
4. **参考资料**: 引用相关论文和资源

### 文档风格指南

```markdown
# 章节标题

简介段落。

## 子章节

### 代码示例

```cpp
// 始终包含必要的头文件
#include "gemm/gemm.cuh"

int matrix_size = 1024;  // 使用有意义的命名
```

### 性能表格

| 实现 | 时间 (ms) | 带宽 (GB/s) |
|----------------|-----------|------------------|
| 朴素 | 10.0 | 50 |
| 优化 | 2.0 | 250 |
```

---

## 📞 获取帮助

- **文档问题**: 提交带有 `[docs]` 标签的 issue
- **代码问题**: 查看现有 issues 或创建新 issue
- **功能请求**: 使用功能请求模板

---

<div align="center">

**学习愉快！🚀**

如果本文档对您有帮助，请考虑给仓库 ⭐ 点赞！

[文档门户](https://lessup.github.io/hpc-ai-optimization-lab) · 
[报告 Bug](https://github.com/LessUp/hpc-ai-optimization-lab/issues) · 
[请求功能](https://github.com/LessUp/hpc-ai-optimization-lab/issues)

</div>
