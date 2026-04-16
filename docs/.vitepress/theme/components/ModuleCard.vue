<template>
  <a :href="href" class="module-card">
    <div class="module-header">
      <span class="module-number">{{ number }}</span>
      <span :class="['module-status', status]">{{ statusText }}</span>
    </div>
    <h3 class="module-title">{{ title }}</h3>
    <p class="module-desc">{{ description }}</p>
    <div class="module-topics">
      <span v-for="topic in topics" :key="topic" class="topic-tag">{{ topic }}</span>
    </div>
  </a>
</template>

<script setup lang="ts">
defineProps<{
  number: string
  title: string
  description: string
  href: string
  status: 'stable' | 'experimental'
  statusText: string
  topics: string[]
}>()
</script>

<style scoped>
.module-card {
  display: block;
  background: var(--vp-c-bg-elv);
  border: 1px solid var(--vp-c-border);
  border-radius: 16px;
  padding: 24px;
  text-decoration: none;
  transition: all 0.3s ease;
}

.module-card:hover {
  transform: translateY(-4px);
  border-color: var(--vp-c-brand-1);
  box-shadow: 0 20px 40px -15px rgba(0, 0, 0, 0.3);
}

.module-header {
  display: flex;
  justify-content: space-between;
  align-items: center;
  margin-bottom: 16px;
}

.module-number {
  font-size: 0.875rem;
  font-weight: 700;
  color: var(--vp-c-brand-1);
  font-family: var(--vp-font-family-mono);
}

.module-status {
  font-size: 0.6875rem;
  font-weight: 600;
  padding: 4px 10px;
  border-radius: 100px;
  text-transform: uppercase;
  letter-spacing: 0.5px;
}

.module-status.stable {
  background: rgba(16, 185, 129, 0.15);
  color: #10b981;
}

.module-status.experimental {
  background: rgba(245, 158, 11, 0.15);
  color: #f59e0b;
}

.module-title {
  font-size: 1.25rem;
  font-weight: 700;
  color: var(--vp-c-text-1);
  margin: 0 0 12px;
}

.module-desc {
  font-size: 0.9375rem;
  color: var(--vp-c-text-3);
  line-height: 1.6;
  margin: 0 0 20px;
}

.module-topics {
  display: flex;
  flex-wrap: wrap;
  gap: 8px;
}

.topic-tag {
  font-size: 0.75rem;
  color: var(--vp-c-text-2);
  background: var(--vp-c-bg-soft);
  padding: 4px 12px;
  border-radius: 100px;
  border: 1px solid var(--vp-c-border);
}
</style>
