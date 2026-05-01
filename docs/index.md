---
layout: home

hero:
  name: HPC-AI-Optimization-Lab
  text: 从原理到极致优化的 CUDA Kernel 之旅
  tagline: 通过渐进式学习掌握 GEMM、FlashAttention、量化等核心 GPU 优化技术，从 0.5 TFLOPS 到 70+ TFLOPS
  image:
    src: /logo-large.svg
    alt: HPC-AI-Optimization-Lab
  actions:
    - theme: brand
      text: 开始学习
      link: /en/guide/quick-start
    - theme: alt
      text: 查看性能数据
      link: /en/guide/gemm#performance-comparison-summary
    - theme: alt
      text: GitHub
      link: https://github.com/LessUp/hpc-ai-optimization-lab

features:
  - icon: 📈
    title: 渐进式优化路径
    details: 7 步 GEMM 优化从简单到极致，每一步都有清晰的性能提升和原理讲解。实测从 0.5 TFLOPS 提升到 70+ TFLOPS，加速比达 140×。
  
  - icon: 🎯
    title: 生产级代码质量
    details: 所有 kernel 都经过测试和基准验证，遵循 NVIDIA 最佳实践。代码覆盖率全面，API 设计简洁一致，适合学习和生产使用。
  
  - icon: ⚡
    title: 现代 GPU 特性
    details: 支持 CUDA 12.4+、Hopper 架构新特性（TMA、Clusters）、FP8/INT8 量化、FlashAttention 2 等前沿技术。
  
  - icon: 🌍
    title: 双语文档体系
    details: 完整的中英文文档，包含安装指南、优化教程、API 参考和故障排除。适合全球开发者学习和贡献。
  
  - icon: 🔬
    title: 覆盖核心场景
    details: 从基础的 elementwise/reduction 到复杂的 GEMM/FlashAttention，再到前沿的量化与 CUDA 13 特性，全方位覆盖 AI 推理关键算子。
  
  - icon: 🐍
    title: Python 绑定支持
    details: 提供简洁的 Python API，支持直接在 PyTorch 工作流中使用。C++ 性能 + Python 易用性的完美结合。
---

<style>
:root {
  --vp-home-hero-name-color: transparent;
  --vp-home-hero-name-background: -webkit-linear-gradient(120deg, #76B900 30%, #00599C);
  --vp-home-hero-image-background-image: linear-gradient(-45deg, #76B90050 50%, #00599C50 50%);
  --vp-home-hero-image-filter: blur(44px);
}

.VPHero .image-bg {
  transition: all 0.5s ease;
}

.VPHero:hover .image-bg {
  transform: scale(1.1);
}
</style>

## 🎯 适合谁

<div class="vp-doc" style="padding: 2rem 0;">

| 角色 | 你将获得 |
|-----|---------|
| **学生** | 从零开始学习 CUDA 优化，理解 GPU 架构和性能瓶颈，为研究打下坚实基础 |
| **研究者** | 快速验证新的 kernel 优化思路，使用生产级基准进行对比实验 |
| **工程师** | 获得可部署的高性能 kernel 实现，加速 AI 推理应用 |

</div>

## 📊 性能亮点

<div class="vp-doc" style="padding: 2rem 0;">

### GEMM 优化之路 (FP32, 4096×4096, NVIDIA A100)

| 步骤 | 优化技术 | TFLOPS | 加速比 | 状态 |
|:----:|---------|:------:|:------:|:----:|
| 1 | Naive | 0.5 | 1× | ✅ |
| 2 | Shared Memory Tiling | 2.0 | 4× | ✅ |
| 3 | Double Buffering | 3.5 | 7× | ✅ |
| 4 | Register Tiling | 6.0 | 12× | ✅ |
| 5 | **Tensor Core (WMMA)** | **50+** | **100×** | ✅ |
| 6 | Tensor Core (MMA PTX) | ~60 | ~120× | 🚧 |
| 7 | Software Pipelining | ~70 | ~140× | 🚧 |

> 💡 **为什么这很重要**：Step 5 实现了 100× 性能提升，展示了现代 AI 硬件专用计算单元的强大威力。

</div>

## 🚀 快速开始

<div class="vp-doc" style="padding: 2rem 0;">

```bash
# 克隆仓库
git clone https://github.com/LessUp/hpc-ai-optimization-lab.git
cd hpc-ai-optimization-lab

# 构建项目
cmake --preset default
cmake --build --preset default

# 运行你的第一个 kernel
./examples/elementwise/relu_example
```

</div>

## 📚 学习路径

<div class="vp-doc" style="padding: 2rem 0;">

### 🌱 初级（1-2 周）
- [安装配置](/en/guide/installation)
- [快速开始](/en/guide/quick-start)
- [内存优化模式](/en/guide/memory)
- [归约算法](/en/guide/reduction)
- GEMM Steps 1-4

### 🚀 中级（2-4 周）
- GEMM Steps 5-7（Tensor Cores）
- [FlashAttention 实现](/en/guide/flash-attention)
- [性能调优与 Profiling](/en/guide/profiling)

### 🏆 高级（持续）
- [CUDA 13 Hopper 特性](/en/guide/cuda13)
- CUTLASS 深度探索
- 研究级 kernel 实现

</div>

## 🔗 快速链接

<div class="vp-doc" style="padding: 2rem 0;">

| 文档 | English | 中文 |
|------|---------|------|
| 安装指南 | [Installation](/en/guide/installation) | [安装指南](/zh-CN/guide/installation) |
| 快速入门 | [Quick Start](/en/guide/quick-start) | [快速入门](/zh-CN/guide/quick-start) |
| GEMM 教程 | [7-Step Journey](/en/guide/gemm) | [GEMM 优化](/zh-CN/guide/gemm) |
| API 参考 | [C++/Python](/en/api/index) | [API 参考](/zh-CN/api/index) |

</div>

---

<div class="vp-doc" style="text-align: center; padding: 3rem 0;">

**⭐ 如果这个项目对你有帮助，请给个 Star！**

[![GitHub stars](https://img.shields.io/github/stars/LessUp/hpc-ai-optimization-lab?style=social)](https://github.com/LessUp/hpc-ai-optimization-lab/stargazers)

</div>
