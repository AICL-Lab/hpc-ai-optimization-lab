---
layout: home

hero:
  name: HPC-AI-Optimization-Lab
  text: Progressive CUDA kernel lab
  tagline: Learn GEMM, FlashAttention, quantization, and CUDA optimization from shipped implementations and measured documentation.
  image:
    src: /logo-large.svg
    alt: HPC-AI-Optimization-Lab
  actions:
    - theme: brand
      text: Guide Index
      link: /en/guide/
    - theme: alt
      text: API Reference
      link: /en/api/index
    - theme: alt
      text: Examples
      link: /en/examples/index
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

<!-- Status Banner -->
<div class="performance-banner">
  <span class="metric">✅ <strong>Steps 1-5</strong> shipped</span>
  <span class="metric">🐍 <strong>3</strong> Python modules shipped</span>
  <span class="metric">🧪 <strong>1</strong> benchmark suite wired</span>
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

## 🧭 Where to go next

<div class="route-grid">
  <a class="route-card" href="/en/guide/quick-start">
    <strong>Run something first</strong>
    Start with installation and quick start before deeper optimization topics.
  </a>
  <a class="route-card" href="/en/guide/gemm">
    <strong>Trace the GEMM journey</strong>
    Follow the measured path from naive kernels through Tensor Core WMMA.
  </a>
  <a class="route-card" href="/en/api/architecture">
    <strong>Read architecture context</strong>
    See how examples, Python bindings, kernels, and common infrastructure connect.
  </a>
  <a class="route-card" href="/en/examples/index">
    <strong>Browse runnable examples</strong>
    Jump to concrete API shapes before theory-heavy guides.
  </a>
</div>

<div class="status-note">
  Public docs track shipped behavior. Step 6 currently delegates to Step 5 for stability, Step 7 ships as the maintained software-pipelined path, and Python bindings cover <code>elementwise</code>, <code>reduction</code>, and <code>gemm</code> only.
</div>

## 👥 Who is this for?

<div class="audience-grid">
  <div class="audience-card">
    <span class="icon">🎓</span>
    <h3>Students</h3>
    <p>Learn CUDA optimization from first principles and connect guide material to runnable kernels.</p>
  </div>
  <div class="audience-card">
    <span class="icon">🔬</span>
    <h3>Researchers</h3>
    <p>Prototype kernel ideas against a repository that exposes measured baselines and architecture notes.</p>
  </div>
  <div class="audience-card">
    <span class="icon">🏭</span>
    <h3>Engineers</h3>
    <p>Study concrete CUDA and Python integration seams before adapting kernels into production systems.</p>
  </div>
</div>

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
      <li>GEMM Step 5 (Tensor Core WMMA)</li>
      <li><a href="/en/guide/flash-attention">FlashAttention Implementation</a></li>
      <li><a href="/en/guide/profiling">Profiling & Tuning</a></li>
    </ul>
  </div>
  <div class="path-level advanced">
    <h4>🏆 Advanced (ongoing)</h4>
    <ul>
      <li>Compare shipped kernels with the CUTLASS baseline and public benchmarks</li>
      <li>Review Step 6 fallback notes alongside the shipped Step 7 path</li>
      <li>Use architecture + API reference when preparing local GPU experiments</li>
    </ul>
  </div>
</div>

<CitationBlock />

</div>
