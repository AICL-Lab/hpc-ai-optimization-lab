import type { DefaultTheme } from 'vitepress'
import { SITE_CONFIG } from './site-config'

const zhCNConfig: DefaultTheme.Config = {
  nav: [
    { text: '首页', link: '/zh-CN/' },
    {
      text: '指南',
      items: [
        { text: '入门指南', link: '/zh-CN/guide/' },
        { text: '访存优化', link: '/zh-CN/guide/memory' },
        { text: '归约', link: '/zh-CN/guide/reduction' },
        { text: 'GEMM', link: '/zh-CN/guide/gemm' },
        { text: 'FlashAttention', link: '/zh-CN/guide/flash-attention' },
        { text: 'CUDA 13', link: '/zh-CN/guide/cuda13' }
      ]
    },
    {
      text: '参考',
      items: [
        { text: 'API 参考', link: '/zh-CN/api/index' },
        { text: '架构概览', link: '/zh-CN/api/architecture' },
        { text: '更新日志', link: '/zh-CN/changelog' }
      ]
    },
    {
      text: '项目',
      items: [
        { text: 'GitHub', link: SITE_CONFIG.repo.url },
        { text: 'Releases', link: SITE_CONFIG.repo.releases }
      ]
    }
  ],

  sidebar: {
    '/zh-CN/': [
      {
        text: '入门',
        collapsed: false,
        items: [
          { text: '项目概览', link: '/zh-CN/' },
          { text: '指南索引', link: '/zh-CN/guide/' },
          { text: '快速开始', link: '/zh-CN/guide/quick-start' },
          { text: '安装指南', link: '/zh-CN/guide/installation' }
        ]
      },
      {
        text: '核心主题',
        collapsed: false,
        items: [
          { text: '访存优化', link: '/zh-CN/guide/memory' },
          { text: '归约', link: '/zh-CN/guide/reduction' },
          { text: 'GEMM', link: '/zh-CN/guide/gemm' },
          { text: 'FlashAttention', link: '/zh-CN/guide/flash-attention' },
          { text: 'CUDA 13', link: '/zh-CN/guide/cuda13' }
        ]
      },
      {
        text: '进阶主题',
        collapsed: true,
        items: [
          { text: '性能调优', link: '/zh-CN/guide/performance-tuning' },
          { text: '性能分析', link: '/zh-CN/guide/profiling' },
          { text: '最佳实践', link: '/zh-CN/guide/best-practices' },
          { text: '故障排除', link: '/zh-CN/guide/troubleshooting' }
        ]
      },
      {
        text: '参考',
        collapsed: true,
        items: [
          { text: 'API 参考', link: '/zh-CN/api/index' },
          { text: '架构概览', link: '/zh-CN/api/architecture' },
          { text: '更新日志', link: '/zh-CN/changelog' }
        ]
      }
    ]
  },

  // Footer
  docFooter: {
    prev: '上一页',
    next: '下一页'
  },
  
  // Return to top
  returnToTopLabel: '回到顶部',
  
  // Menu
  sidebarMenuLabel: '菜单',
  langMenuLabel: '选择语言',
  
  // Outline
  outline: {
    label: '本页内容'
  },
  
  // Last updated
  lastUpdated: {
    text: '更新于'
  },
  
  // Dark mode
  darkModeSwitchLabel: '主题'
}

export default zhCNConfig
