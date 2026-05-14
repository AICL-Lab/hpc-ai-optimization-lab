<script setup lang="ts">
import { computed } from 'vue'
import { useData } from 'vitepress'
import { getGemmStepsData, formatTflops, getSpeedup } from '../../data/loader'

const { lang } = useData()
const locale = computed(() => lang.value as 'en-US' | 'zh-CN')
const data = computed(() => getGemmStepsData(locale.value))

const getBarWidth = (tflops: number) => {
  return `${(tflops / data.value.maxTflops) * 100}%`
}
</script>

<template>
  <div class="performance-chart">
    <h2 class="chart-title">
      <span class="gradient-text">{{ data.labels.title }}</span>
      <span class="subtitle">{{ data.labels.subtitle }}</span>
    </h2>

    <div class="chart-container">
      <div
        v-for="item in data.steps"
        :key="item.step"
        class="chart-row"
        :class="{ planned: item.planned }"
      >
        <div class="step-label">
          <span class="step-number">Step {{ item.step }}</span>
          <span class="step-name">{{ item.name }}</span>
          <span v-if="item.planned" class="planned-badge">{{ data.labels.planned }}</span>
        </div>

        <div class="bar-container">
          <div
            class="bar"
            :class="item.color"
            :style="{ width: getBarWidth(item.tflops) }"
          >
            <span class="bar-value">{{ formatTflops(item.tflops) }}</span>
          </div>
        </div>

        <div class="metrics">
          <span class="speedup">{{ getSpeedup(item.tflops) }}</span>
        </div>
      </div>
    </div>

    <div class="chart-legend">
      <div class="legend-item">
        <span class="legend-color low"></span>
        <span>{{ data.labels.legend.low }}</span>
      </div>
      <div class="legend-item">
        <span class="legend-color medium"></span>
        <span>{{ data.labels.legend.medium }}</span>
      </div>
      <div class="legend-item">
        <span class="legend-color high"></span>
        <span>{{ data.labels.legend.high }}</span>
      </div>
    </div>

    <div class="chart-note">
      <strong>Key Insight:</strong> {{ data.labels.keyInsight }}
    </div>
  </div>
</template>

<style scoped>
.performance-chart {
  margin: 48px 0;
  padding: 32px;
  background: var(--vp-c-bg-soft);
  border-radius: 16px;
  border: 1px solid var(--vp-c-divider);
}

.chart-title {
  text-align: center;
  margin-bottom: 32px;
}

.chart-title .gradient-text {
  display: block;
  font-size: 1.75rem;
  font-weight: 700;
  background: linear-gradient(120deg, var(--vp-c-brand-1), #3b82f6);
  -webkit-background-clip: text;
  background-clip: text;
  -webkit-text-fill-color: transparent;
}

.chart-title .subtitle {
  display: block;
  font-size: 0.9375rem;
  color: var(--vp-c-text-2);
  font-weight: 400;
  margin-top: 8px;
}

.chart-container {
  display: flex;
  flex-direction: column;
  gap: 12px;
}

.chart-row {
  display: grid;
  grid-template-columns: 160px 1fr 80px;
  align-items: center;
  gap: 16px;
  padding: 12px 16px;
  background: var(--vp-c-bg);
  border-radius: 8px;
  transition: all 0.3s ease;
}

.chart-row:hover {
  box-shadow: 0 4px 12px rgba(0, 0, 0, 0.05);
}

.chart-row.planned {
  opacity: 0.7;
}

.step-label {
  display: flex;
  flex-direction: column;
  gap: 2px;
}

.step-number {
  font-size: 0.75rem;
  font-weight: 600;
  color: var(--vp-c-text-3);
  text-transform: uppercase;
  letter-spacing: 0.5px;
}

.step-name {
  font-size: 0.9375rem;
  font-weight: 600;
  color: var(--vp-c-text-1);
}

.planned-badge {
  display: inline-block;
  font-size: 0.625rem;
  padding: 2px 6px;
  background: var(--vp-c-brand-soft);
  color: var(--vp-c-brand-1);
  border-radius: 4px;
  text-transform: uppercase;
  font-weight: 600;
  width: fit-content;
  margin-top: 2px;
}

.bar-container {
  height: 32px;
  background: var(--vp-c-bg-alt);
  border-radius: 6px;
  overflow: hidden;
}

.bar {
  height: 100%;
  display: flex;
  align-items: center;
  padding: 0 12px;
  border-radius: 6px;
  transition: width 0.8s ease-out;
  animation: barGrow 1s ease-out;
}

@keyframes barGrow {
  from { width: 0; }
}

.bar.low {
  background: linear-gradient(90deg, #ef4444, #f87171);
}

.bar.medium {
  background: linear-gradient(90deg, #f59e0b, #fbbf24);
}

.bar.high {
  background: linear-gradient(90deg, #10b981, #34d399);
}

.bar-value {
  font-size: 0.8125rem;
  font-weight: 700;
  color: white;
  white-space: nowrap;
}

.metrics {
  text-align: right;
}

.speedup {
  font-size: 1rem;
  font-weight: 700;
  font-family: ui-monospace, SFMono-Regular, SF Mono, Menlo, Consolas, monospace;
  color: var(--vp-c-brand-1);
}

.chart-legend {
  display: flex;
  justify-content: center;
  gap: 24px;
  margin-top: 24px;
  padding-top: 24px;
  border-top: 1px solid var(--vp-c-divider);
}

.legend-item {
  display: flex;
  align-items: center;
  gap: 8px;
  font-size: 0.8125rem;
  color: var(--vp-c-text-2);
}

.legend-color {
  width: 16px;
  height: 16px;
  border-radius: 4px;
}

.legend-color.low { background: #ef4444; }
.legend-color.medium { background: #f59e0b; }
.legend-color.high { background: #10b981; }

.chart-note {
  margin-top: 24px;
  padding: 16px;
  background: var(--vp-c-brand-soft);
  border-radius: 8px;
  font-size: 0.875rem;
  color: var(--vp-c-text-2);
  line-height: 1.6;
}

.chart-note strong {
  color: var(--vp-c-brand-1);
}

@media (max-width: 768px) {
  .chart-row {
    grid-template-columns: 1fr;
    gap: 8px;
  }

  .bar-container {
    order: 2;
  }

  .metrics {
    order: 1;
    text-align: left;
  }

  .chart-legend {
    flex-direction: column;
    gap: 12px;
  }
}
</style>
