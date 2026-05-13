---
layout: home
hero:
  name: HPC-AI-Optimization-Lab
  text: ''
  actions:
    - theme: brand
      text: 简体中文
      link: /zh-CN/
    - theme: alt
      text: English
      link: /en/
---

<script setup>
import { onMounted } from 'vue'
import { useRouter } from 'vitepress'

onMounted(() => {
  const router = useRouter()
  const lang = navigator.language || navigator.userLanguage
  if (lang.startsWith('zh')) {
    router.go('/zh-CN/')
  } else {
    router.go('/en/')
  }
})
</script>
