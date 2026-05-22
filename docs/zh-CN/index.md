---
layout: home

hero:
  name: HPC-AI-Optimization-Lab
  text: 渐进式 CUDA 内核实验室
  tagline: 围绕已交付实现与可验证文档，学习 GEMM、FlashAttention、量化与 CUDA 优化方法。
  image:
    src: /logo-large.svg
    alt: HPC-AI-Optimization-Lab
  actions:
    - theme: brand
      text: 指南索引
      link: /zh-CN/guide/
    - theme: alt
      text: API 参考
      link: /zh-CN/api/index
    - theme: alt
      text: 示例
      link: /zh-CN/examples/index
---

<script setup>
import PerformanceChart from '../.vitepress/theme/components/PerformanceChart.vue'
import QuickStart from '../.vitepress/theme/components/QuickStart.vue'
import CitationBlock from '../.vitepress/theme/components/CitationBlock.vue'
import FeatureGrid from '../.vitepress/theme/components/FeatureGrid.vue'
import ArchitectureDiagram from '../.vitepress/theme/components/ArchitectureDiagram.vue'
import MetricsDashboard from '../.vitepress/theme/components/MetricsDashboard.vue'
</script>

<style>
:root {
  --vp-home-hero-name-color: transparent;
  --vp-home-hero-name-background: -webkit-linear-gradient(120deg, #1a365d 30%, #38b2ac);
  --vp-home-hero-image-background-image: linear-gradient(-45deg, #1a365d50 50%, #38b2ac50 50%);
  --vp-home-hero-image-filter: blur(44px);
}

.home-content {
  display: grid;
  gap: 24px;
}

.route-grid {
  display: grid;
  gap: 16px;
  grid-template-columns: repeat(auto-fit, minmax(220px, 1fr));
}

.route-card {
  display: block;
  padding: 18px;
  border-radius: 14px;
  border: 1px solid var(--vp-c-divider);
  background: var(--vp-c-bg-soft);
}

.route-card:hover {
  border-color: var(--vp-c-brand-1);
  text-decoration: none;
}

.route-card strong {
  display: block;
  margin-bottom: 6px;
}

.status-note {
  padding: 16px 18px;
  border-left: 4px solid var(--vp-c-brand-1);
  border-radius: 12px;
  background: var(--vp-c-bg-soft);
  color: var(--vp-c-text-2);
}

@media (max-width: 768px) {
  .route-grid {
    grid-template-columns: 1fr;
  }
}
</style>

<div class="home-content">

<!-- 状态横幅 -->
<div class="performance-banner">
  <span class="metric">✅ <strong>Steps 1-5</strong> 已交付</span>
  <span class="metric">🐍 <strong>3</strong> 个 Python 模块已开放</span>
  <span class="metric">🧪 <strong>1</strong> 个基准套件已接通</span>
</div>

<!-- 指标面板 -->
<MetricsDashboard lang="zh-CN" />

<!-- 系统架构 -->
<ArchitectureDiagram lang="zh-CN" />

<!-- 快速开始 -->
<QuickStart />

<!-- 性能图表 -->
<PerformanceChart />

<!-- 特性网格 -->
<FeatureGrid />

## 🧭 下一步去哪里

<div class="route-grid">
  <a class="route-card" href="/zh-CN/guide/quick-start">
    <strong>先跑起来</strong>
    从安装与快速开始入手，再进入更深入的优化主题。
  </a>
  <a class="route-card" href="/zh-CN/guide/gemm">
    <strong>追踪 GEMM 演进路径</strong>
    从朴素实现一路看到 Tensor Core WMMA 的实测优化过程。
  </a>
  <a class="route-card" href="/zh-CN/api/architecture">
    <strong>阅读架构上下文</strong>
    了解示例、Python 绑定、CUDA 内核与公共基础设施如何连接。
  </a>
  <a class="route-card" href="/zh-CN/examples/index">
    <strong>先看可运行示例</strong>
    在进入理论章节前，先掌握真实 API 形状与调用方式。
  </a>
</div>

<div class="status-note">
  公共文档只描述当前已交付行为。Step 6 目前为稳定性委托到 Step 5，Step 7 仍属计划项；Python 绑定当前仅覆盖 <code>elementwise</code>、<code>reduction</code> 与 <code>gemm</code>。
</div>

## 👥 适合谁？

<div class="audience-grid">
  <div class="audience-card">
    <span class="icon">🎓</span>
    <h3>学生</h3>
    <p>从第一性原理学习 CUDA 优化，并把文档内容映射到真实可运行内核。</p>
  </div>
  <div class="audience-card">
    <span class="icon">🔬</span>
    <h3>研究者</h3>
    <p>围绕可测量基线与架构说明快速验证新的内核优化思路。</p>
  </div>
  <div class="audience-card">
    <span class="icon">🏭</span>
    <h3>工程师</h3>
    <p>先理解 CUDA 与 Python 集成 seam，再把内核迁移到生产系统。</p>
  </div>
</div>

## 📚 学习路径

<div class="learning-path">
  <div class="path-level beginner">
    <h4>🌱 初级（1-2 周）</h4>
    <ul>
      <li><a href="/zh-CN/guide/installation">安装配置</a></li>
      <li><a href="/zh-CN/guide/quick-start">快速开始</a></li>
      <li><a href="/zh-CN/guide/memory">访存优化模式</a></li>
      <li><a href="/zh-CN/guide/reduction">归约算法</a></li>
      <li>GEMM Steps 1-4</li>
    </ul>
  </div>
  <div class="path-level intermediate">
    <h4>🚀 中级（2-4 周）</h4>
    <ul>
      <li>GEMM Step 5（Tensor Core WMMA）</li>
      <li><a href="/zh-CN/guide/flash-attention">FlashAttention 实现</a></li>
      <li><a href="/zh-CN/guide/profiling">性能调优与 Profiling</a></li>
    </ul>
  </div>
  <div class="path-level advanced">
    <h4>🏆 高级（持续）</h4>
    <ul>
      <li><a href="/zh-CN/guide/cuda13">CUDA 13 Hopper 特性</a></li>
      <li>结合实现状态说明阅读 Step 6 / Step 7 规划</li>
      <li>准备本地 GPU 实验时同步参考架构页与 API 参考</li>
    </ul>
  </div>
</div>

<CitationBlock />

</div>
