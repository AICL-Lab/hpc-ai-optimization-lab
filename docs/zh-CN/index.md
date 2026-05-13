---
layout: home

hero:
  name: HPC-AI-Optimization-Lab
  text: 从原理到极致优化
  tagline: 通过渐进式学习掌握 GEMM、FlashAttention、量化等核心 GPU 优化技术，从 0.5 TFLOPS 到 70+ TFLOPS
  image:
    src: /logo-large.svg
    alt: HPC-AI-Optimization-Lab
  actions:
    - theme: brand
      text: 开始学习
      link: /zh-CN/guide/quick-start
    - theme: alt
      text: GEMM 之旅
      link: /zh-CN/guide/gemm
    - theme: alt
      text: GitHub
      link: https://github.com/LessUp/hpc-ai-optimization-lab
---

<script setup>
import PerformanceChart from '../.vitepress/theme/components/PerformanceChart.vue'
import QuickStart from '../.vitepress/theme/components/QuickStart.vue'
import CitationBlock from '../.vitepress/theme/components/CitationBlock.vue'
import FeatureGrid from '../.vitepress/theme/components/FeatureGrid.vue'
</script>

<style>
:root {
  --vp-home-hero-name-color: transparent;
  --vp-home-hero-name-background: -webkit-linear-gradient(120deg, #76B900 30%, #00599C);
  --vp-home-hero-image-background-image: linear-gradient(-45deg, #76B90050 50%, #00599C50 50%);
  --vp-home-hero-image-filter: blur(44px);
}
</style>

<div class="home-content">

<!-- 性能亮点 -->
<div class="performance-banner">
  <span class="metric">🚀 <strong>140×</strong> 性能提升</span>
  <span class="metric">⚡ <strong>70+</strong> TFLOPS</span>
  <span class="metric">🎯 <strong>A100</strong> 验证</span>
</div>

<!-- 快速入门 -->
<QuickStart />

<!-- 性能图表 -->
<PerformanceChart />

<!-- 特性网格 -->
<FeatureGrid />

<!-- 适合谁 -->
## 👥 适合谁？

<div class="audience-grid">
  <div class="audience-card">
    <span class="icon">🎓</span>
    <h3>学生</h3>
    <p>从零开始学习 CUDA 优化，理解 GPU 架构和性能瓶颈，为研究打下坚实基础。</p>
  </div>
  <div class="audience-card">
    <span class="icon">🔬</span>
    <h3>研究者</h3>
    <p>快速验证新的内核优化思路，使用生产级基准进行对比实验。</p>
  </div>
  <div class="audience-card">
    <span class="icon">🏭</span>
    <h3>工程师</h3>
    <p>获得可部署的高性能内核实现，加速 AI 推理应用。</p>
  </div>
</div>

<!-- 学习路径 -->
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
      <li>GEMM Steps 5-7（Tensor Cores）</li>
      <li><a href="/zh-CN/guide/flash-attention">FlashAttention 实现</a></li>
      <li><a href="/zh-CN/guide/profiling">性能调优与 Profiling</a></li>
    </ul>
  </div>
  <div class="path-level advanced">
    <h4>🏆 高级（持续）</h4>
    <ul>
      <li><a href="/zh-CN/guide/cuda13">CUDA 13 Hopper 特性</a></li>
      <li>CUTLASS 深度探索</li>
      <li>研究级内核实现</li>
    </ul>
  </div>
</div>

<!-- 引用 -->
<CitationBlock />

</div>

<style scoped>
.home-content {
  max-width: 1200px;
  margin: 0 auto;
  padding: 0 24px;
}

.performance-banner {
  display: flex;
  justify-content: center;
  gap: 32px;
  padding: 24px;
  background: var(--vp-c-bg-soft);
  border-radius: 16px;
  margin: 48px 0;
  border: 1px solid var(--vp-c-divider);
}

.metric {
  display: flex;
  align-items: center;
  gap: 8px;
  font-size: 1.125rem;
  color: var(--vp-c-text-1);
}

.metric strong {
  color: var(--vp-c-brand-1);
  font-size: 1.5rem;
}

.audience-grid {
  display: grid;
  grid-template-columns: repeat(auto-fit, minmax(280px, 1fr));
  gap: 24px;
  margin: 24px 0;
}

.audience-card {
  padding: 24px;
  background: var(--vp-c-bg-soft);
  border-radius: 12px;
  border: 1px solid var(--vp-c-divider);
  text-align: center;
}

.audience-card .icon {
  font-size: 2.5rem;
  display: block;
  margin-bottom: 12px;
}

.audience-card h3 {
  margin: 0 0 12px;
  font-size: 1.125rem;
}

.audience-card p {
  margin: 0;
  font-size: 0.9375rem;
  color: var(--vp-c-text-2);
  line-height: 1.6;
}

.learning-path {
  display: grid;
  grid-template-columns: repeat(auto-fit, minmax(280px, 1fr));
  gap: 24px;
  margin: 24px 0;
}

.path-level {
  padding: 24px;
  background: var(--vp-c-bg-soft);
  border-radius: 12px;
  border: 1px solid var(--vp-c-divider);
}

.path-level h4 {
  margin: 0 0 16px;
  font-size: 1rem;
}

.path-level ul {
  list-style: none;
  padding: 0;
  margin: 0;
}

.path-level li {
  padding: 8px 0;
  color: var(--vp-c-text-2);
}

.path-level li a {
  color: var(--vp-c-brand-1);
  text-decoration: none;
}

.path-level li a:hover {
  text-decoration: underline;
}

@media (max-width: 768px) {
  .performance-banner {
    flex-direction: column;
    gap: 16px;
    align-items: center;
  }
}
</style>