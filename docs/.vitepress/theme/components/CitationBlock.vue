<script setup lang="ts">
import { ref } from 'vue'

const bibtex = `@software{hpc_ai_optimization_lab,
  title = {HPC-AI-Optimization-Lab: CUDA Kernel Optimization Learning Platform},
  author = {LessUp},
  year = {2024},
  url = {https://github.com/LessUp/hpc-ai-optimization-lab},
  version = {0.3.0},
  note = {GEMM, FlashAttention, Quantization implementations}
}`

const apa = `LessUp. (2024). HPC-AI-Optimization-Lab: CUDA Kernel Optimization Learning Platform (Version 0.3.0). https://github.com/LessUp/hpc-ai-optimization-lab`

const formats = [
  { name: 'BibTeX', code: bibtex },
  { name: 'APA', code: apa }
]

const activeFormat = ref(0)
const copied = ref(false)

const copyCitation = async () => {
  try {
    await navigator.clipboard.writeText(formats[activeFormat.value].code)
    copied.value = true
    setTimeout(() => {
      copied.value = false
    }, 2000)
  } catch (err) {
    console.error('Failed to copy:', err)
  }
}
</script>

<template>
  <div class="citation-block">
    <h2 class="section-title">
      <span class="gradient-text">Citation</span>
      <span class="subtitle">If you use this project in your research, please cite</span>
    </h2>

    <div class="citation-container">
      <div class="format-tabs">
        <button
          v-for="(format, index) in formats"
          :key="format.name"
          class="format-tab"
          :class="{ active: activeFormat === index }"
          @click="activeFormat = index"
        >
          {{ format.name }}
        </button>
      </div>

      <div class="citation-content">
        <pre><code>{{ formats[activeFormat].code }}</code></pre>
        <button
          class="copy-button"
          @click="copyCitation"
          :class="{ copied: copied }"
        >
          <span v-if="copied">✓ Copied!</span>
          <span v-else>Copy Citation</span>
        </button>
      </div>
    </div>

    <div class="citation-links">
      <a href="https://github.com/LessUp/hpc-ai-optimization-lab" class="citation-link">
        <span class="icon">📦</span>
        <span>GitHub Repository</span>
      </a>
      <a href="https://github.com/LessUp/hpc-ai-optimization-lab/issues" class="citation-link">
        <span class="icon">🐛</span>
        <span>Report Issue</span>
      </a>
      <a href="https://lessup.github.io/hpc-ai-optimization-lab/" class="citation-link">
        <span class="icon">📚</span>
        <span>Documentation</span>
      </a>
    </div>
  </div>
</template>

<style scoped>
.citation-block {
  margin: 48px 0;
  padding: 32px;
  background: var(--vp-c-bg-soft);
  border-radius: 16px;
  border: 1px solid var(--vp-c-divider);
}

.section-title {
  text-align: center;
  margin-bottom: 24px;
}

.section-title .gradient-text {
  display: block;
  font-size: 1.5rem;
  font-weight: 700;
  background: linear-gradient(120deg, var(--vp-c-brand-1), #3b82f6);
  -webkit-background-clip: text;
  background-clip: text;
  -webkit-text-fill-color: transparent;
}

.section-title .subtitle {
  display: block;
  font-size: 0.875rem;
  color: var(--vp-c-text-2);
  font-weight: 400;
  margin-top: 8px;
}

.citation-container {
  max-width: 700px;
  margin: 0 auto;
}

.format-tabs {
  display: flex;
  gap: 8px;
  margin-bottom: 16px;
}

.format-tab {
  padding: 8px 16px;
  background: var(--vp-c-bg);
  border: 1px solid var(--vp-c-divider);
  border-radius: 8px;
  font-size: 0.875rem;
  font-weight: 500;
  color: var(--vp-c-text-2);
  cursor: pointer;
  transition: all 0.2s ease;
}

.format-tab:hover {
  color: var(--vp-c-text-1);
  border-color: var(--vp-c-brand-1);
}

.format-tab.active {
  background: var(--vp-c-brand-soft);
  color: var(--vp-c-brand-1);
  border-color: var(--vp-c-brand-1);
}

.citation-content {
  position: relative;
  background: var(--vp-c-bg);
  border-radius: 12px;
  padding: 20px;
  border: 1px solid var(--vp-c-divider);
}

.citation-content pre {
  margin: 0;
  padding: 0;
  overflow-x: auto;
}

.citation-content code {
  font-family: ui-monospace, SFMono-Regular, SF Mono, Menlo, Consolas, monospace;
  font-size: 0.8125rem;
  line-height: 1.6;
  color: var(--vp-c-text-1);
}

.copy-button {
  position: absolute;
  top: 12px;
  right: 12px;
  padding: 6px 16px;
  background: var(--vp-c-bg-soft);
  border: 1px solid var(--vp-c-divider);
  border-radius: 6px;
  font-size: 0.8125rem;
  font-weight: 500;
  color: var(--vp-c-text-2);
  cursor: pointer;
  transition: all 0.2s ease;
}

.copy-button:hover {
  background: var(--vp-c-brand-soft);
  color: var(--vp-c-brand-1);
  border-color: var(--vp-c-brand-1);
}

.copy-button.copied {
  background: var(--vp-c-brand-1);
  color: white;
  border-color: var(--vp-c-brand-1);
}

.citation-links {
  display: flex;
  justify-content: center;
  gap: 24px;
  margin-top: 24px;
  padding-top: 24px;
  border-top: 1px solid var(--vp-c-divider);
  flex-wrap: wrap;
}

.citation-link {
  display: flex;
  align-items: center;
  gap: 8px;
  padding: 8px 16px;
  background: var(--vp-c-bg);
  border: 1px solid var(--vp-c-divider);
  border-radius: 8px;
  font-size: 0.875rem;
  font-weight: 500;
  color: var(--vp-c-text-2);
  text-decoration: none;
  transition: all 0.2s ease;
}

.citation-link:hover {
  color: var(--vp-c-brand-1);
  border-color: var(--vp-c-brand-1);
}

.citation-link .icon {
  font-size: 1rem;
}

@media (max-width: 768px) {
  .citation-links {
    flex-direction: column;
    align-items: center;
  }

  .format-tabs {
    justify-content: center;
  }
}
</style>
