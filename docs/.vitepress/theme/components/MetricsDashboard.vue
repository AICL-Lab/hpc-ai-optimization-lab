<script setup lang="ts">
import { ref, onMounted, computed } from 'vue'

interface Metric {
  value: number
  suffix?: string
  prefix?: string
  label: string
  labelZh: string
  color: string
}

const props = defineProps<{
  lang?: 'en' | 'zh-CN'
}>()

const lang = computed(() => props.lang || 'en')

const metrics: Metric[] = [
  {
    value: 140,
    suffix: '×',
    label: 'Speedup',
    labelZh: '加速比',
    color: '#2f855a'
  },
  {
    value: 70,
    suffix: '+',
    label: 'TFLOPS',
    labelZh: 'TFLOPS',
    color: '#38b2ac'
  },
  {
    value: 8,
    suffix: '-bit',
    label: 'FP8 Support',
    labelZh: 'FP8 支持',
    color: '#3b82f6'
  },
  {
    value: 100,
    suffix: '%',
    label: 'A100 Verified',
    labelZh: 'A100 验证',
    color: '#8b5cf6'
  }
]

const animatedValues = ref<number[]>(metrics.map(() => 0))
const isVisible = ref(false)

const animateCounters = () => {
  const duration = 1500
  const startTime = performance.now()

  const animate = (currentTime: number) => {
    const elapsed = currentTime - startTime
    const progress = Math.min(elapsed / duration, 1)

    // Easing function
    const easeOutQuart = 1 - Math.pow(1 - progress, 4)

    metrics.forEach((metric, index) => {
      animatedValues.value[index] = Math.round(metric.value * easeOutQuart)
    })

    if (progress < 1) {
      requestAnimationFrame(animate)
    }
  }

  requestAnimationFrame(animate)
}

onMounted(() => {
  // Use IntersectionObserver to start animation when visible
  const observer = new IntersectionObserver(
    (entries) => {
      entries.forEach((entry) => {
        if (entry.isIntersecting && !isVisible.value) {
          isVisible.value = true
          animateCounters()
          observer.disconnect()
        }
      })
    },
    { threshold: 0.3 }
  )

  const element = document.querySelector('.metrics-dashboard')
  if (element) {
    observer.observe(element)
  }
})

const getLabel = (metric: Metric) => lang.value === 'zh-CN' ? metric.labelZh : metric.label

const formatValue = (value: number, metric: Metric) => {
  return `${metric.prefix || ''}${value}${metric.suffix || ''}`
}
</script>

<template>
  <div class="metrics-dashboard">
    <div class="metrics-grid">
      <div
        v-for="(metric, index) in metrics"
        :key="index"
        class="metric-card"
        :style="{ '--metric-color': metric.color }"
      >
        <div class="metric-value">
          <span class="value-number">{{ formatValue(animatedValues[index], metric) }}</span>
        </div>
        <div class="metric-label">{{ getLabel(metric) }}</div>
        <div class="metric-bar">
          <div
            class="metric-bar-fill"
            :style="{
              width: `${(animatedValues[index] / metric.value) * 100}%`,
              backgroundColor: metric.color
            }"
          ></div>
        </div>
      </div>
    </div>

    <div class="metrics-note">
      <span v-if="lang === 'zh-CN'">
        基于 FP32 GEMM 优化旅程，在 NVIDIA A100 上实测
      </span>
      <span v-else>
        Based on FP32 GEMM optimization journey, measured on NVIDIA A100
      </span>
    </div>
  </div>
</template>

<style scoped>
.metrics-dashboard {
  margin: 32px 0;
  padding: 24px;
  background: var(--vp-c-bg-soft);
  border-radius: 16px;
  border: 1px solid var(--vp-c-divider);
}

.metrics-grid {
  display: grid;
  grid-template-columns: repeat(4, 1fr);
  gap: 24px;
}

@media (max-width: 768px) {
  .metrics-grid {
    grid-template-columns: repeat(2, 1fr);
    gap: 16px;
  }
}

@media (max-width: 480px) {
  .metrics-grid {
    grid-template-columns: 1fr;
  }
}

.metric-card {
  text-align: center;
  padding: 20px;
  background: var(--vp-c-bg);
  border-radius: 12px;
  border: 1px solid var(--vp-c-divider);
  transition: all 0.3s ease;
}

.metric-card:hover {
  transform: translateY(-4px);
  box-shadow: var(--vp-shadow-3);
  border-color: var(--metric-color);
}

.metric-value {
  margin-bottom: 8px;
}

.value-number {
  font-size: 2.5rem;
  font-weight: 700;
  color: var(--metric-color);
  font-variant-numeric: tabular-nums;
  line-height: 1;
}

.metric-label {
  font-size: 0.875rem;
  font-weight: 500;
  color: var(--vp-c-text-2);
  margin-bottom: 12px;
}

.metric-bar {
  height: 4px;
  background: var(--vp-c-divider);
  border-radius: 2px;
  overflow: hidden;
}

.metric-bar-fill {
  height: 100%;
  border-radius: 2px;
  transition: width 1.5s ease-out;
}

.metrics-note {
  margin-top: 24px;
  padding-top: 16px;
  border-top: 1px solid var(--vp-c-divider);
  text-align: center;
  font-size: 0.8125rem;
  color: var(--vp-c-text-3);
}
</style>
