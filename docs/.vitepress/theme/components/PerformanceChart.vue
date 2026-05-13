<script setup lang="ts">
import { ref } from 'vue'

const gemmSteps = [
  { step: 1, name: 'Naive', tflops: 0.5, color: 'low' },
  { step: 2, name: 'Shared Mem', tflops: 2.0, color: 'low' },
  { step: 3, name: 'Double Buffer', tflops: 3.5, color: 'low' },
  { step: 4, name: 'Register Tile', tflops: 6.0, color: 'medium' },
  { step: 5, name: 'Tensor Core', tflops: 50.0, color: 'high' },
  { step: 6, name: 'MMA PTX', tflops: 60.0, color: 'high', planned: true },
  { step: 7, name: 'Pipelining', tflops: 70.0, color: 'high', planned: true },
]

const maxTflops = 70

const getBarWidth = (tflops: number) => {
  return `${(tflops / maxTflops) * 100}%`
}

const formatTflops = (tflops: number) => {
  if (tflops >= 50) return `${tflops}+ TFLOPS`
  return `${tflops} TFLOPS`
}

const getSpeedup = (tflops: number) => {
  return `${Math.round(tflops / 0.5)}×`
}
</script>

<template>
  <div class="performance-chart">
    <h2 class="chart-title">
      <span class="gradient-text">GEMM Optimization Journey</span>
      <span class="subtitle">FP32 Matrix Multiplication (4096×4096) on NVIDIA A100</span>
    </h2>

    <div class="chart-container">
      <div
        v-for="item in gemmSteps"
        :key="item.step"
        class="chart-row"
        :class="{ planned: item.planned }"
      >
        <div class="step-label">
          <span class="step-number">Step {{ item.step }}</span>
          <span class="step-name">{{ item.name }}</span>
          <span v-if="item.planned" class="planned-badge">Planned</span>
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
        <span>Baseline (&lt;10 TFLOPS)</span>
      </div>
      <div class="legend-item">
        <span class="legend-color medium"></span>
        <span>Optimized (10-50 TFLOPS)</span>
      </div>
      <div class="legend-item">
        <span class="legend-color high"></span>
        <span>Tensor Core (&gt;50 TFLOPS)</span>
      </div>
    </div>

    <div class="chart-note">
      <strong>Key Insight:</strong> Step 5 (Tensor Core) achieves 100× speedup over naive implementation,
      demonstrating the power of modern GPU specialized compute units.
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
