---
layout: home

hero:
  name: HPC-AI-Optimization-Lab
  text: From Principles to Peak Optimization
  tagline: Master CUDA kernel optimization through progressive GEMM, FlashAttention, and quantization implementations — from 0.5 TFLOPS to 70+ TFLOPS
  image:
    src: /logo-large.svg
    alt: HPC-AI-Optimization-Lab
  actions:
    - theme: brand
      text: Start Learning
      link: /en/guide/quick-start
    - theme: alt
      text: GEMM Journey
      link: /en/guide/gemm
    - theme: alt
      text: GitHub
      link: https://github.com/LessUp/hpc-ai-optimization-lab
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
</style>

<div class="home-content">

<!-- Performance Banner -->
<div class="performance-banner">
  <span class="metric">🚀 <strong>140×</strong> Speedup</span>
  <span class="metric">⚡ <strong>70+</strong> TFLOPS</span>
  <span class="metric">🎯 <strong>A100</strong> Verified</span>
</div>

<!-- Metrics Dashboard -->
<MetricsDashboard />

<!-- Architecture Diagram -->
<ArchitectureDiagram />

<!-- Quick Start -->
<QuickStart />

<!-- Performance Chart -->
<PerformanceChart />

<!-- Feature Grid -->
<FeatureGrid />

<!-- Who is this for -->
## 👥 Who is this for?

<div class="audience-grid">
  <div class="audience-card">
    <span class="icon">🎓</span>
    <h3>Students</h3>
    <p>Learn CUDA optimization from first principles, understand GPU architecture and performance bottlenecks.</p>
  </div>
  <div class="audience-card">
    <span class="icon">🔬</span>
    <h3>Researchers</h3>
    <p>Quickly prototype new kernel optimization ideas, use production-grade baselines for comparison experiments.</p>
  </div>
  <div class="audience-card">
    <span class="icon">🏭</span>
    <h3>Engineers</h3>
    <p>Get production-ready kernel implementations for AI inference acceleration.</p>
  </div>
</div>

<!-- Learning Path -->
## 📚 Learning Path

<div class="learning-path">
  <div class="path-level beginner">
    <h4>🌱 Beginner (1-2 weeks)</h4>
    <ul>
      <li><a href="/en/guide/installation">Installation & Setup</a></li>
      <li><a href="/en/guide/quick-start">Quick Start</a></li>
      <li><a href="/en/guide/memory">Memory Optimization Patterns</a></li>
      <li><a href="/en/guide/reduction">Reduction Algorithms</a></li>
      <li>GEMM Steps 1-4</li>
    </ul>
  </div>
  <div class="path-level intermediate">
    <h4>🚀 Intermediate (2-4 weeks)</h4>
    <ul>
      <li>GEMM Steps 5-7 (Tensor Cores)</li>
      <li><a href="/en/guide/flash-attention">FlashAttention Implementation</a></li>
      <li><a href="/en/guide/profiling">Profiling & Tuning</a></li>
    </ul>
  </div>
  <div class="path-level advanced">
    <h4>🏆 Advanced (ongoing)</h4>
    <ul>
      <li><a href="/en/guide/cuda13">CUDA 13 Hopper Features</a></li>
      <li>CUTLASS Deep Dive</li>
      <li>Research Implementations</li>
    </ul>
  </div>
</div>

<!-- Citation -->
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
