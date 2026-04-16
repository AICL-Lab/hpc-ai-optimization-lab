import { h } from 'vue'
import type { Theme } from 'vitepress'
import DefaultTheme from 'vitepress/theme'
import './custom.css'

// 自定义组件
import HomeHero from './components/HomeHero.vue'
import HomeFeatures from './components/HomeFeatures.vue'
import ApiIndex from './components/ApiIndex.vue'
import ModuleCard from './components/ModuleCard.vue'
import PerformanceChart from './components/PerformanceChart.vue'
import CodeSandbox from './components/CodeSandbox.vue'

export default {
  extends: DefaultTheme,
  Layout() {
    return h(DefaultTheme.Layout, null, {
      // 自定义插槽
      'home-hero-before': () => h(HomeHero),
      'home-features-after': () => h(HomeFeatures),
    })
  },
  enhanceApp({ app }) {
    // 注册全局组件
    app.component('ApiIndex', ApiIndex)
    app.component('ModuleCard', ModuleCard)
    app.component('PerformanceChart', PerformanceChart)
    app.component('CodeSandbox', CodeSandbox)
  }
} satisfies Theme
