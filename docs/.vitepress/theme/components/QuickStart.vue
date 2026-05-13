<script setup lang="ts">
import { ref } from 'vue'

const steps = [
  {
    number: 1,
    title: 'Clone Repository',
    code: 'git clone https://github.com/LessUp/hpc-ai-optimization-lab.git\ncd hpc-ai-optimization-lab',
    copied: false
  },
  {
    number: 2,
    title: 'Build Project',
    code: 'cmake --preset default\ncmake --build --preset default',
    copied: false
  },
  {
    number: 3,
    title: 'Run Example',
    code: './build/examples/elementwise/relu_example',
    copied: false
  }
]

const copyStates = ref([false, false, false])

const copyCode = async (index: number, code: string) => {
  try {
    await navigator.clipboard.writeText(code)
    copyStates.value[index] = true
    setTimeout(() => {
      copyStates.value[index] = false
    }, 2000)
  } catch (err) {
    console.error('Failed to copy:', err)
  }
}
</script>

<template>
  <div class="quick-start">
    <h2 class="section-title">
      <span class="gradient-text">Quick Start</span>
      <span class="subtitle">Get up and running in 3 simple steps</span>
    </h2>

    <div class="steps-container">
      <div
        v-for="(step, index) in steps"
        :key="step.number"
        class="step-card"
      >
        <div class="step-header">
          <span class="step-number">{{ step.number }}</span>
          <span class="step-title">{{ step.title }}</span>
        </div>

        <div class="code-block">
          <pre><code>{{ step.code }}</code></pre>
          <button
            class="copy-button"
            @click="copyCode(index, step.code)"
            :class="{ copied: copyStates[index] }"
          >
            <span v-if="copyStates[index]">✓ Copied!</span>
            <span v-else>Copy</span>
          </button>
        </div>
      </div>
    </div>

    <div class="requirements">
      <span class="req-label">Requirements:</span>
      <span class="req-item">CUDA 12.4+</span>
      <span class="req-item">CMake 3.24+</span>
      <span class="req-item">C++20</span>
    </div>
  </div>
</template>

<style scoped>
.quick-start {
  margin: 48px 0;
  padding: 32px;
  background: var(--vp-c-bg);
  border-radius: 16px;
  border: 1px solid var(--vp-c-divider);
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

.steps-container {
  display: grid;
  grid-template-columns: repeat(auto-fit, minmax(280px, 1fr));
  gap: 24px;
}

.step-card {
  padding: 20px;
  background: var(--vp-c-bg-soft);
  border-radius: 12px;
  border: 1px solid var(--vp-c-divider);
  transition: all 0.3s ease;
}

.step-card:hover {
  border-color: var(--vp-c-brand-1);
  box-shadow: 0 4px 12px rgba(118, 185, 0, 0.1);
}

.step-header {
  display: flex;
  align-items: center;
  gap: 12px;
  margin-bottom: 16px;
}

.step-number {
  display: flex;
  align-items: center;
  justify-content: center;
  width: 32px;
  height: 32px;
  background: var(--vp-c-brand-1);
  color: white;
  border-radius: 50%;
  font-weight: 700;
  font-size: 0.9375rem;
}

.step-title {
  font-weight: 600;
  font-size: 1rem;
  color: var(--vp-c-text-1);
}

.code-block {
  position: relative;
  background: var(--vp-c-bg);
  border-radius: 8px;
  padding: 16px;
  font-family: ui-monospace, SFMono-Regular, SF Mono, Menlo, Consolas, monospace;
  font-size: 0.8125rem;
  line-height: 1.6;
}

.code-block pre {
  margin: 0;
  padding: 0;
}

.code-block code {
  color: var(--vp-c-text-1);
}

.copy-button {
  position: absolute;
  top: 8px;
  right: 8px;
  padding: 4px 12px;
  background: var(--vp-c-bg-soft);
  border: 1px solid var(--vp-c-divider);
  border-radius: 6px;
  font-size: 0.75rem;
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

.requirements {
  display: flex;
  align-items: center;
  justify-content: center;
  gap: 16px;
  margin-top: 24px;
  padding-top: 24px;
  border-top: 1px solid var(--vp-c-divider);
  flex-wrap: wrap;
}

.req-label {
  font-size: 0.8125rem;
  font-weight: 600;
  color: var(--vp-c-text-2);
}

.req-item {
  display: inline-flex;
  padding: 4px 12px;
  background: var(--vp-c-bg-soft);
  border-radius: 999px;
  font-size: 0.75rem;
  font-weight: 500;
  color: var(--vp-c-text-1);
  border: 1px solid var(--vp-c-divider);
}

@media (max-width: 768px) {
  .steps-container {
    grid-template-columns: 1fr;
  }

  .requirements {
    flex-direction: column;
    gap: 12px;
  }
}
</style>
