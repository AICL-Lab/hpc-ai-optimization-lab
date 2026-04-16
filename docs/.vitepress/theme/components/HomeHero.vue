<template>
  <div class="home-hero">
    <div class="hero-content">
      <div class="badge">
        <span class="badge-dot"></span>
        <span>v0.3.0 Released</span>
      </div>
      
      <h1 class="title">
        <span class="title-line">CUDA Kernel</span>
        <span class="title-line highlight">Optimization Lab</span>
      </h1>
      
      <p class="description">
        A comprehensive educational laboratory for high-performance GPU kernels. 
        From memory coalescing to Tensor Cores, from naive implementations to 
        production-ready FlashAttention.
      </p>
      
      <div class="actions">
        <a href="/en/guide/" class="action primary">
          <span>Get Started</span>
          <span class="arrow">→</span>
        </a>
        <a href="/en/guide/gemm" class="action secondary">
          <span>Explore Guides</span>
        </a>
      </div>
      
      <div class="stats">
        <div class="stat">
          <span class="stat-value">7</span>
          <span class="stat-label">Modules</span>
        </div>
        <div class="stat-divider"></div>
        <div class="stat">
          <span class="stat-value">50+</span>
          <span class="stat-label">TFLOPS</span>
        </div>
        <div class="stat-divider"></div>
        <div class="stat">
          <span class="stat-value">2</span>
          <span class="stat-label">Languages</span>
        </div>
      </div>
    </div>
    
    <div class="hero-visual">
      <div class="code-preview">
        <div class="code-header">
          <span class="dot red"></span>
          <span class="dot yellow"></span>
          <span class="dot green"></span>
          <span class="filename">gemm_tensor_core.cu</span>
        </div>
        <pre class="code-body"><code>// Tensor Core GEMM - 140x Speedup!
__global__ void gemm_tensor_core(...) {
  // WMMA fragments
  wmma::fragment&lt;...&gt; a, b, c;
  
  // Load to shared memory
  wmma::load_matrix_sync(a, A, ...);
  wmma::load_matrix_sync(b, B, ...);
  
  // Tensor Core MMA
  wmma::mma_sync(c, a, b, c);
  
  // Store result
  wmma::store_matrix_sync(C, c, ...);
}</code></pre>
      </div>
    </div>
  </div>
</template>

<style scoped>
.home-hero {
  display: grid;
  grid-template-columns: 1fr 1fr;
  gap: 64px;
  align-items: center;
  padding: 64px 24px;
  max-width: 1200px;
  margin: 0 auto;
}

.hero-content {
  display: flex;
  flex-direction: column;
  gap: 24px;
}

.badge {
  display: inline-flex;
  align-items: center;
  gap: 8px;
  padding: 6px 14px;
  background: rgba(34, 211, 238, 0.1);
  border: 1px solid rgba(34, 211, 238, 0.3);
  border-radius: 100px;
  font-size: 0.875rem;
  font-weight: 500;
  color: var(--vp-c-brand-1);
  width: fit-content;
}

.badge-dot {
  width: 8px;
  height: 8px;
  background: #10b981;
  border-radius: 50%;
  animation: pulse 2s infinite;
}

@keyframes pulse {
  0%, 100% { opacity: 1; }
  50% { opacity: 0.5; }
}

.title {
  display: flex;
  flex-direction: column;
  gap: 4px;
  margin: 0;
  font-size: 3.5rem;
  font-weight: 800;
  line-height: 1.1;
}

.title-line {
  color: var(--vp-c-text-1);
}

.title-line.highlight {
  background: linear-gradient(135deg, #22d3ee, #0891b2);
  -webkit-background-clip: text;
  -webkit-text-fill-color: transparent;
}

.description {
  font-size: 1.125rem;
  line-height: 1.7;
  color: var(--vp-c-text-2);
  max-width: 500px;
}

.actions {
  display: flex;
  gap: 12px;
}

.action {
  display: inline-flex;
  align-items: center;
  gap: 8px;
  padding: 14px 28px;
  border-radius: 10px;
  font-weight: 600;
  text-decoration: none;
  transition: all 0.3s;
}

.action.primary {
  background: linear-gradient(135deg, #22d3ee, #06b6d4);
  color: #0b0f19;
}

.action.primary:hover {
  transform: translateY(-2px);
  box-shadow: 0 8px 25px rgba(34, 211, 238, 0.4);
}

.action.secondary {
  background: var(--vp-c-bg-elv);
  border: 1px solid var(--vp-c-border);
  color: var(--vp-c-text-1);
}

.action.secondary:hover {
  border-color: var(--vp-c-brand-1);
}

.arrow {
  transition: transform 0.3s;
}

.action:hover .arrow {
  transform: translateX(4px);
}

.stats {
  display: flex;
  align-items: center;
  gap: 24px;
  margin-top: 16px;
  padding-top: 24px;
  border-top: 1px solid var(--vp-c-divider);
}

.stat {
  display: flex;
  flex-direction: column;
}

.stat-value {
  font-size: 1.75rem;
  font-weight: 800;
  color: var(--vp-c-brand-1);
}

.stat-label {
  font-size: 0.875rem;
  color: var(--vp-c-text-3);
}

.stat-divider {
  width: 1px;
  height: 32px;
  background: var(--vp-c-divider);
}

.hero-visual {
  display: flex;
  align-items: center;
  justify-content: center;
}

.code-preview {
  background: #0d1117;
  border: 1px solid var(--vp-c-border);
  border-radius: 12px;
  overflow: hidden;
  box-shadow: 0 25px 50px -12px rgba(0, 0, 0, 0.5);
  max-width: 100%;
}

.code-header {
  display: flex;
  align-items: center;
  gap: 8px;
  padding: 12px 16px;
  background: rgba(255, 255, 255, 0.02);
  border-bottom: 1px solid var(--vp-c-border);
}

.dot {
  width: 12px;
  height: 12px;
  border-radius: 50%;
}

.dot.red { background: #ff5f56; }
.dot.yellow { background: #ffbd2e; }
.dot.green { background: #27c93f; }

.filename {
  margin-left: 8px;
  font-size: 0.875rem;
  color: var(--vp-c-text-3);
  font-family: var(--vp-font-family-mono);
}

.code-body {
  padding: 20px;
  margin: 0;
  font-family: var(--vp-font-family-mono);
  font-size: 0.8125rem;
  line-height: 1.7;
  color: #e6edf3;
  overflow-x: auto;
}

@media (max-width: 960px) {
  .home-hero {
    grid-template-columns: 1fr;
    text-align: center;
    padding: 40px 24px;
  }
  
  .title {
    font-size: 2.5rem;
  }
  
  .description {
    margin: 0 auto;
  }
  
  .actions {
    justify-content: center;
  }
  
  .stats {
    justify-content: center;
  }
  
  .hero-visual {
    order: -1;
  }
  
  .code-preview {
    max-width: 100%;
  }
}
</style>
