import type { Theme } from 'vitepress'
import DefaultTheme from 'vitepress/theme'
import './custom.css'

// Custom components
import PerformanceChart from './components/PerformanceChart.vue'
import QuickStart from './components/QuickStart.vue'
import CitationBlock from './components/CitationBlock.vue'
import FeatureGrid from './components/FeatureGrid.vue'
import ArchitectureDiagram from './components/ArchitectureDiagram.vue'
import MetricsDashboard from './components/MetricsDashboard.vue'

export default {
  extends: DefaultTheme,
  enhanceApp({ app }) {
    app.component('PerformanceChart', PerformanceChart)
    app.component('QuickStart', QuickStart)
    app.component('CitationBlock', CitationBlock)
    app.component('FeatureGrid', FeatureGrid)
    app.component('ArchitectureDiagram', ArchitectureDiagram)
    app.component('MetricsDashboard', MetricsDashboard)
  }
} as Theme
