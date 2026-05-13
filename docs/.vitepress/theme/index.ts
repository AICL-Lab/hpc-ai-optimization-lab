import type { Theme } from 'vitepress'
import DefaultTheme from 'vitepress/theme'
import './custom.css'

// Custom components
import PerformanceChart from './components/PerformanceChart.vue'
import QuickStart from './components/QuickStart.vue'
import CitationBlock from './components/CitationBlock.vue'
import FeatureGrid from './components/FeatureGrid.vue'

export default {
  extends: DefaultTheme,
  enhanceApp({ app }) {
    app.component('PerformanceChart', PerformanceChart)
    app.component('QuickStart', QuickStart)
    app.component('CitationBlock', CitationBlock)
    app.component('FeatureGrid', FeatureGrid)
  }
} as Theme
