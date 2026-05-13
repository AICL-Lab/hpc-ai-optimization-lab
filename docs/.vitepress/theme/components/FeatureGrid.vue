<script setup lang="ts">
const features = [
  {
    icon: '📈',
    title: 'Progressive Learning',
    description: '7-step GEMM optimization from naive to Tensor Core, each step with clear performance gains and explanations.',
    tags: ['GEMM', '140× Speedup']
  },
  {
    icon: '🎯',
    title: 'Production Quality',
    description: 'All kernels tested and benchmarked, following NVIDIA best practices. Clean API design for learning and production.',
    tags: ['Tested', 'Benchmarked']
  },
  {
    icon: '⚡',
    title: 'Modern GPU Features',
    description: 'CUDA 12.4+, Hopper architecture (TMA, Clusters), FP8/INT8 quantization, FlashAttention 2.',
    tags: ['CUDA 12.4', 'Hopper']
  },
  {
    icon: '🌍',
    title: 'Dual Language Docs',
    description: 'Complete English and Chinese documentation with installation guides, tutorials, and API references.',
    tags: ['EN', '中文']
  },
  {
    icon: '🔬',
    title: 'Core Scenarios',
    description: 'From basic elementwise/reduction to complex GEMM/FlashAttention, covering AI inference critical operators.',
    tags: ['Attention', 'Quantization']
  },
  {
    icon: '🐍',
    title: 'Python Bindings',
    description: 'Clean Python API via nanobind, integrate directly into PyTorch workflows. C++ performance + Python ease.',
    tags: ['Python', 'PyTorch']
  }
]
</script>

<template>
  <div class="feature-grid-section">
    <h2 class="section-title">
      <span class="gradient-text">Key Features</span>
      <span class="subtitle">What makes this project unique</span>
    </h2>

    <div class="feature-grid">
      <div
        v-for="feature in features"
        :key="feature.title"
        class="feature-card"
      >
        <div class="feature-icon">{{ feature.icon }}</div>
        <h3 class="feature-title">{{ feature.title }}</h3>
        <p class="feature-description">{{ feature.description }}</p>
        <div class="feature-tags">
          <span
            v-for="tag in feature.tags"
            :key="tag"
            class="feature-tag"
          >
            {{ tag }}
          </span>
        </div>
      </div>
    </div>
  </div>
</template>

<style scoped>
.feature-grid-section {
  margin: 48px 0;
}

.section-title {
  text-align: center;
  margin-bottom: 32px;
}

.section-title .gradient-text {
  display: block;
  font-size: 1.75rem;
  font-weight: 700;
  background: linear-gradient(120deg, var(--vp-c-brand-1), #3b82f6);
  -webkit-background-clip: text;
  background-clip: text;
  -webkit-text-fill-color: transparent;
}

.section-title .subtitle {
  display: block;
  font-size: 0.9375rem;
  color: var(--vp-c-text-2);
  font-weight: 400;
  margin-top: 8px;
}

.feature-grid {
  display: grid;
  grid-template-columns: repeat(auto-fit, minmax(320px, 1fr));
  gap: 24px;
}

.feature-card {
  padding: 24px;
  background: var(--vp-c-bg);
  border: 1px solid var(--vp-c-divider);
  border-radius: 16px;
  transition: all 0.3s ease;
}

.feature-card:hover {
  border-color: var(--vp-c-brand-1);
  box-shadow: 0 8px 24px rgba(118, 185, 0, 0.1);
  transform: translateY(-4px);
}

.feature-icon {
  font-size: 2.5rem;
  margin-bottom: 16px;
}

.feature-title {
  font-size: 1.125rem;
  font-weight: 700;
  color: var(--vp-c-text-1);
  margin: 0 0 12px 0;
}

.feature-description {
  font-size: 0.9375rem;
  color: var(--vp-c-text-2);
  line-height: 1.6;
  margin: 0 0 16px 0;
}

.feature-tags {
  display: flex;
  flex-wrap: wrap;
  gap: 8px;
}

.feature-tag {
  display: inline-flex;
  padding: 4px 12px;
  background: var(--vp-c-brand-soft);
  color: var(--vp-c-brand-1);
  border-radius: 999px;
  font-size: 0.75rem;
  font-weight: 600;
}

@media (max-width: 768px) {
  .feature-grid {
    grid-template-columns: 1fr;
  }
}
</style>
