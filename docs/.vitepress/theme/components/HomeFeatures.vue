<template>
  <div class="home-features">
    <div class="features-header">
      <h2 class="features-title">Seven Kernel Modules</h2>
      <p class="features-desc">
        From basic elementwise operations to cutting-edge CUDA 13 features, 
        each module builds upon the previous to create a comprehensive learning journey.
      </p>
    </div>
    
    <div class="features-grid">
      <div class="feature-card" v-for="(feature, index) in features" :key="index">
        <div class="feature-icon">{{ feature.icon }}</div>
        <h3 class="feature-title">{{ feature.title }}</h3>
        <p class="feature-desc">{{ feature.desc }}</p>
        <div class="feature-meta">
          <span :class="['status', feature.status]">{{ feature.statusText }}</span>
          <span class="difficulty">{{ feature.difficulty }}</span>
        </div>
      </div>
    </div>
    
    <div class="features-cta">
      <a href="/en/guide/" class="cta-button">
        Start Learning
        <span class="arrow">→</span>
      </a>
      <a href="/cpp-api/" class="cta-button secondary" target="_blank">
        Explore API
      </a>
    </div>
  </div>
</template>

<script setup lang="ts">
const features = [
  {
    icon: '🔢',
    title: 'Elementwise',
    desc: 'ReLU, Sigmoid, Transpose. Learn coalesced access, vectorization, and bank conflict avoidance.',
    status: 'stable',
    statusText: 'Stable',
    difficulty: 'Beginner'
  },
  {
    icon: '📊',
    title: 'Reduction',
    desc: 'Softmax, LayerNorm, RMSNorm. Master warp shuffle, block reduction, and online algorithms.',
    status: 'stable',
    statusText: 'Stable',
    difficulty: 'Beginner'
  },
  {
    icon: '🧮',
    title: 'GEMM',
    desc: '7-step matrix multiplication optimization. From naive to Tensor Core, benchmarked against cuBLAS.',
    status: 'stable',
    statusText: 'Stable',
    difficulty: 'Intermediate'
  },
  {
    icon: '🖼️',
    title: 'Convolution',
    desc: 'Implicit GEMM convolution. Winograd implementation with GEMM fallback.',
    status: 'stable',
    statusText: 'Stable',
    difficulty: 'Intermediate'
  },
  {
    icon: '🎯',
    title: 'Attention',
    desc: 'FlashAttention with online softmax. IO-aware design for efficient transformer attention.',
    status: 'stable',
    statusText: 'Stable',
    difficulty: 'Advanced'
  },
  {
    icon: '⚡',
    title: 'Quantization',
    desc: 'INT8 per-row quantization, FP8 scaling utilities. Low-precision inference kernels.',
    status: 'stable',
    statusText: 'Stable',
    difficulty: 'Intermediate'
  },
  {
    icon: '🚀',
    title: 'CUDA 13',
    desc: 'TMA, Thread Block Clusters, FP8 GEMM. Experimental Hopper architecture features.',
    status: 'experimental',
    statusText: 'Experimental',
    difficulty: 'Advanced'
  }
]
</script>

<style scoped>
.home-features {
  padding: 80px 24px;
  max-width: 1200px;
  margin: 0 auto;
}

.features-header {
  text-align: center;
  margin-bottom: 48px;
}

.features-title {
  font-size: 2.25rem;
  font-weight: 800;
  color: var(--vp-c-text-1);
  margin: 0 0 16px;
}

.features-desc {
  font-size: 1.125rem;
  color: var(--vp-c-text-2);
  max-width: 640px;
  margin: 0 auto;
  line-height: 1.7;
}

.features-grid {
  display: grid;
  grid-template-columns: repeat(auto-fit, minmax(300px, 1fr));
  gap: 24px;
  margin-bottom: 48px;
}

.feature-card {
  background: var(--vp-c-bg-elv);
  border: 1px solid var(--vp-c-border);
  border-radius: 16px;
  padding: 28px;
  transition: all 0.3s ease;
}

.feature-card:hover {
  transform: translateY(-4px);
  border-color: var(--vp-c-brand-1);
  box-shadow: 0 20px 40px -15px rgba(0, 0, 0, 0.3);
}

.feature-icon {
  font-size: 2.5rem;
  margin-bottom: 16px;
}

.feature-title {
  font-size: 1.25rem;
  font-weight: 700;
  color: var(--vp-c-text-1);
  margin: 0 0 12px;
}

.feature-desc {
  font-size: 0.9375rem;
  color: var(--vp-c-text-3);
  line-height: 1.6;
  margin: 0 0 20px;
}

.feature-meta {
  display: flex;
  gap: 12px;
}

.status {
  font-size: 0.75rem;
  font-weight: 600;
  padding: 4px 10px;
  border-radius: 100px;
  text-transform: uppercase;
  letter-spacing: 0.5px;
}

.status.stable {
  background: rgba(16, 185, 129, 0.15);
  color: #10b981;
}

.status.experimental {
  background: rgba(245, 158, 11, 0.15);
  color: #f59e0b;
}

.difficulty {
  font-size: 0.75rem;
  color: var(--vp-c-text-3);
  padding: 4px 10px;
  background: var(--vp-c-bg-soft);
  border-radius: 100px;
}

.features-cta {
  display: flex;
  justify-content: center;
  gap: 16px;
}

.cta-button {
  display: inline-flex;
  align-items: center;
  gap: 8px;
  padding: 16px 32px;
  background: linear-gradient(135deg, var(--vp-c-brand-1), var(--vp-c-brand-2));
  color: #0b0f19;
  font-weight: 600;
  border-radius: 10px;
  text-decoration: none;
  transition: all 0.3s;
}

.cta-button:hover {
  transform: translateY(-2px);
  box-shadow: 0 8px 25px rgba(34, 211, 238, 0.4);
}

.cta-button.secondary {
  background: var(--vp-c-bg-elv);
  color: var(--vp-c-text-1);
  border: 1px solid var(--vp-c-border);
}

.cta-button.secondary:hover {
  border-color: var(--vp-c-brand-1);
}

.arrow {
  transition: transform 0.3s;
}

.cta-button:hover .arrow {
  transform: translateX(4px);
}

@media (max-width: 768px) {
  .features-grid {
    grid-template-columns: 1fr;
  }
  
  .features-title {
    font-size: 1.75rem;
  }
  
  .features-cta {
    flex-direction: column;
    align-items: center;
  }
}
</style>
