<script setup lang="ts">
import { computed } from 'vue'
import { useData } from 'vitepress'
import { getFeaturesData } from '../../data/loader'

const { lang } = useData()
const locale = computed(() => lang.value as 'en-US' | 'zh-CN')
const data = computed(() => getFeaturesData(locale.value))
</script>

<template>
  <div class="feature-grid-section">
    <h2 class="section-title">
      <span class="gradient-text">{{ data.labels.title }}</span>
      <span class="subtitle">{{ data.labels.subtitle }}</span>
    </h2>

    <div class="feature-grid">
      <div
        v-for="feature in data.features"
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
