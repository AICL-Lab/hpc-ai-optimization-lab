import type { DefaultTheme } from 'vitepress'
import { SITE_CONFIG } from './site-config'

const enConfig: DefaultTheme.Config = {
  nav: [
    { text: 'Home', link: '/en/' },
    {
      text: 'Guides',
      items: [
        { text: 'Getting Started', link: '/en/guide/' },
        { text: 'Memory Optimization', link: '/en/guide/memory' },
        { text: 'Reduction', link: '/en/guide/reduction' },
        { text: 'GEMM', link: '/en/guide/gemm' },
        { text: 'FlashAttention', link: '/en/guide/flash-attention' }
      ]
    },
    {
      text: 'Reference',
      items: [
        { text: 'API Reference', link: '/en/api/index' },
        { text: 'Architecture', link: '/en/api/architecture' }
      ]
    },
    {
      text: 'Project',
      items: [
        { text: 'GitHub', link: SITE_CONFIG.repo.url },
        { text: 'Releases', link: SITE_CONFIG.repo.releases }
      ]
    }
  ],

  sidebar: {
    '/en/': [
      {
        text: 'Getting Started',
        collapsed: false,
        items: [
          { text: 'Overview', link: '/en/' },
          { text: 'Guide Index', link: '/en/guide/' },
          { text: 'Quick Start', link: '/en/guide/quick-start' },
          { text: 'Installation', link: '/en/guide/installation' }
        ]
      },
      {
        text: 'Core Topics',
        collapsed: false,
        items: [
          { text: 'Memory Optimization', link: '/en/guide/memory' },
          { text: 'Reduction', link: '/en/guide/reduction' },
          { text: 'GEMM', link: '/en/guide/gemm' },
          { text: 'FlashAttention', link: '/en/guide/flash-attention' }
        ]
      },
      {
        text: 'Advanced Notes',
        collapsed: true,
        items: [
          { text: 'Performance Tuning', link: '/en/guide/performance-tuning' },
          { text: 'Profiling', link: '/en/guide/profiling' },
          { text: 'Best Practices', link: '/en/guide/best-practices' },
          { text: 'Troubleshooting', link: '/en/guide/troubleshooting' }
        ]
      },
      {
        text: 'Reference',
        collapsed: true,
        items: [
          { text: 'API Reference', link: '/en/api/index' },
          { text: 'Architecture', link: '/en/api/architecture' }
        ]
      }
    ]
  },

  // Footer
  docFooter: {
    prev: 'Previous',
    next: 'Next'
  },
  
  // Return to top
  returnToTopLabel: 'Back to top',
  
  // Menu
  sidebarMenuLabel: 'Menu',
  langMenuLabel: 'Languages',
  
  // Outline
  outline: {
    label: 'On this page'
  },
  
  // Last updated
  lastUpdated: {
    text: 'Updated'
  },
  
  // Dark mode
  darkModeSwitchLabel: 'Theme'
}

export default enConfig
