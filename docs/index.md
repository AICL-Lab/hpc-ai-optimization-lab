---
layout: home
hero:
  name: HPC-AI-Optimization-Lab
  text: CUDA Kernel Optimization Lab
  tagline: From Naive to Tensor Core — Master GPU Performance Optimization
---

<script setup>
import { onMounted } from 'vue'

onMounted(() => {
  const browserLang = navigator.language || navigator.userLanguage
  const targetPath = browserLang.startsWith('zh') ? '/zh-CN/' : '/en/'
  
  if (typeof window !== 'undefined') {
    const currentPath = window.location.pathname
    if (currentPath.endsWith('/hpc-ai-optimization-lab/') || currentPath.endsWith('/hpc-ai-optimization-lab')) {
      window.location.replace(targetPath)
    }
  }
})
</script>

<div style="text-align: center; padding: 2rem;">
  <p style="color: var(--vp-c-text-2);">
    Redirecting to your preferred language...
  </p>
  <p>
    <a href="/en/">English</a> | <a href="/zh-CN/">简体中文</a>
  </p>
</div>
