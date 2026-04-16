---
layout: home

hero:
  name: HPC-AI-Optimization-Lab
  text: CUDA 内核优化实验室
  tagline: 面向 AI 工作负载的综合 GPU 高性能内核教育实验室。从内存合并访问到张量核心，从朴素实现到生产级 FlashAttention。
  image:
    src: /logo-large.svg
    alt: HPC-AI-Optimization-Lab Logo
  actions:
    - theme: brand
      text: 开始使用
      link: /zh-CN/guide/
    - theme: alt
      text: 在 GitHub 上查看
      link: https://github.com/LessUp/hpc-ai-optimization-lab

features:
  - icon: 🔢
    title: 逐元素操作
    details: ReLU、Sigmoid、转置。学习合并访问、向量化和存储体冲突避免。GPU 编程的基础。
  - icon: 📊
    title: 归约操作
    details: Softmax、LayerNorm、RMSNorm。掌握线程束洗牌、块级归约和在线算法。注意力机制的核心。
  - icon: 🧮
    title: GEMM 优化
    details: 7步矩阵乘法优化之旅。从朴素实现（0.5 TFLOPS）到张量核心（70+ TFLOPS）。140倍加速潜力。
  - icon: 🖼️
    title: 卷积
    details: 隐式 GEMM 卷积实现。理解卷积如何映射到矩阵乘法以获得最佳性能。
  - icon: 🎯
    title: FlashAttention
    details: 基于 IO 感知的注意力机制与在线 Softmax。将 HBM 流量从 O(N²) 降至 O(N)。最先进的 Transformer 优化。
  - icon: 🚀
    title: CUDA 13 特性
    details: 实验性 TMA、线程块集群、FP8 GEMM。前沿的 Hopper 架构特性与兼容实现。
---

<script setup>
import HomeHero from '/.vitepress/theme/components/HomeHero.vue'
import HomeFeatures from '/.vitepress/theme/components/HomeFeatures.vue'
</script>

<div style="text-align: center; margin: 2rem 0;">
  <a href="/en/" style="color: var(--vp-c-text-3); text-decoration: none; font-size: 0.9rem;">
    🌐 English Documentation →
  </a>
</div>

## 🏠 欢迎来到 HPC-AI-Optimization-Lab

本文档站点为现代 AI 工作负载中使用的 CUDA 内核优化技术提供全面指南。无论您是学习 GPU 编程的初学者，还是优化生产内核的经验丰富的开发者，您都能在这里找到宝贵的见解。

## 🎓 学习路径

### 初学者（第 1-2 周）
从 GPU 内存和并行归约的基础开始：

1. **[访存优化](/zh-CN/guide/memory)** - 理解合并访问、向量化（float4）、共享内存模式和存储体冲突避免。
2. **[归约优化](/zh-CN/guide/reduction)** - 学习线程束洗牌、块级归约、在线 Softmax 和 Welford 算法。
3. **GEMM 步骤 1-4** - 通过朴素实现到共享内存分块建立坚实基础。

### 进阶（第 2-4 周）
掌握张量核心使用和注意力机制：

1. **GEMM 步骤 5-7** - 实现张量核心 WMMA/MMA、寄存器分块和软件流水线。
2. **[FlashAttention](/zh-CN/guide/flash-attention)** - 理解 IO 感知设计、分块策略和在线 Softmax 集成。

### 专家（持续学习）
探索前沿特性和研究：

1. **[CUDA 13 特性](/zh-CN/guide/cuda13)** - 试验 TMA、线程块集群和 FP8 量化。
2. **CUTLASS 深入研究** - 研究生产内核的参考实现。

## 📊 性能基准

| 实现 | 加速比 | TFLOPS (FP16) | 关键技术 |
|-----|--------|--------------|---------|
| 朴素 GEMM | 1x | ~0.5 | 基线 |
| 共享内存分块 | 4x | ~2.0 | 分块大小优化 |
| 双缓冲 | 7x | ~3.5 | 内存延迟隐藏 |
| 寄存器分块 | 12x | ~6.0 | 减少共享内存流量 |
| 张量核心 WMMA | 100x | ~50 | 硬件加速 |
| 张量核心 MMA PTX | 120x | ~60 | 细粒度控制 |
| 软件流水线 | 140x | ~70+ | 多阶段重叠 |

## 🛠️ 快速开始

```bash
# 克隆仓库
git clone https://github.com/LessUp/hpc-ai-optimization-lab.git
cd hpc-ai-optimization-lab

# 使用 CMake 构建
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j$(nproc)

# 运行测试
ctest --test-dir build --output-on-failure

# 构建 Python 绑定
cmake -S . -B build -DBUILD_PYTHON_BINDINGS=ON
cmake --build build
export PYTHONPATH="$(pwd)/build/python:$PYTHONPATH"
```

## 📚 文档结构

- **教程**: 涵盖每个优化主题的深入指南
- **API 参考**: 自动生成的 C++/CUDA 和 Python API 文档
- **示例**: 每个模块的实用代码示例

<HomeFeatures />

## 🤝 贡献

我们欢迎贡献！请参阅我们的 [GitHub 仓库](https://github.com/LessUp/hpc-ai-optimization-lab) 了解提交问题和拉取请求的指南。

## 📄 许可证

本项目采用 Apache 2.0 许可证。

---

<div style="text-align: center; margin-top: 4rem; padding: 2rem; background: var(--vp-c-bg-elv); border-radius: 16px;">
  <h3 style="margin-top: 0;">开始您的优化之旅</h3>
  <p>从朴素内核到张量核心精通——今天提升您的 CUDA 技能。</p>
  <a href="/zh-CN/guide/" style="display: inline-flex; align-items: center; gap: 8px; padding: 14px 28px; background: linear-gradient(135deg, var(--vp-c-brand-1), var(--vp-c-brand-2)); color: #0b0f19; font-weight: 600; border-radius: 10px; text-decoration: none;">
    开始使用 →
  </a>
</div>
