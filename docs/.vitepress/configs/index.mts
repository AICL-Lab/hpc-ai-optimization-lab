import type { HeadConfig, DefaultTheme } from 'vitepress'

// Head 配置
export function head(): HeadConfig[] {
  return [
    // 字符编码和视口
    ['meta', { charset: 'utf-8' }],
    ['meta', { name: 'viewport', content: 'width=device-width, initial-scale=1.0' }],
    
    // SEO
    ['meta', { name: 'description', content: 'A comprehensive CUDA kernel optimization laboratory for AI workloads' }],
    ['meta', { name: 'keywords', content: 'CUDA, GPU, optimization, HPC, AI, machine learning, Tensor Core, FlashAttention' }],
    ['meta', { name: 'author', content: 'HPC-AI-Optimization-Lab Contributors' }],
    ['meta', { name: 'theme-color', content: '#0891b2' }],
    
    // Open Graph
    ['meta', { property: 'og:type', content: 'website' }],
    ['meta', { property: 'og:site_name', content: 'HPC-AI-Optimization-Lab' }],
    ['meta', { property: 'og:title', content: 'HPC-AI-Optimization-Lab Documentation' }],
    ['meta', { property: 'og:description', content: 'A comprehensive CUDA kernel optimization laboratory for AI workloads' }],
    ['meta', { property: 'og:url', content: 'https://lessup.github.io/hpc-ai-optimization-lab/' }],
    ['meta', { property: 'og:image', content: 'https://lessup.github.io/hpc-ai-optimization-lab/og-image.png' }],
    
    // Twitter Card
    ['meta', { name: 'twitter:card', content: 'summary_large_image' }],
    ['meta', { name: 'twitter:title', content: 'HPC-AI-Optimization-Lab' }],
    ['meta', { name: 'twitter:description', content: 'CUDA kernel optimization laboratory for AI workloads' }],
    
    // 预连接到 CDN
    ['link', { rel: 'preconnect', href: 'https://fonts.googleapis.com' }],
    ['link', { rel: 'preconnect', href: 'https://fonts.gstatic.com', crossorigin: '' }],
    
    // Favicon
    ['link', { rel: 'icon', type: 'image/svg+xml', href: '/logo.svg' }],
    ['link', { rel: 'apple-touch-icon', sizes: '180x180', href: '/apple-touch-icon.png' }],
    
    // Canonical
    ['link', { rel: 'canonical', href: 'https://lessup.github.io/hpc-ai-optimization-lab/' }],
    
    // Performance hints
    ['meta', { name: 'render-color', content: '#081120' }],
    ['meta', { 'http-equiv': 'x-ua-compatible', content: 'ie=edge' }]
  ]
}

// 导航配置
export function nav(): DefaultTheme.NavItem[] {
  return [
    { text: 'Home', link: '/en/' },
    {
      text: 'Guide',
      items: [
        { text: 'Getting Started', link: '/en/guide/' },
        { text: 'Memory Optimization', link: '/en/guide/memory' },
        { text: 'GEMM Optimization', link: '/en/guide/gemm' },
        { text: 'FlashAttention', link: '/en/guide/flash-attention' }
      ]
    },
    {
      text: 'API',
      items: [
        { text: 'C++ / CUDA', link: '/cpp-api/index.html', target: '_blank' },
        { text: 'Python', link: '/python-api/index.html', target: '_blank' }
      ]
    },
    { text: 'v0.3.0', link: '/en/changelog' }
  ]
}

// 侧边栏配置
export function sidebar(): DefaultTheme.Sidebar {
  return {
    '/en/': [
      {
        text: 'Introduction',
        collapsed: false,
        items: [
          { text: 'What is HPC-AI-Lab?', link: '/en/' },
          { text: 'Quick Start', link: '/en/guide/quick-start' },
          { text: 'Installation', link: '/en/guide/installation' }
        ]
      },
      {
        text: 'Core Guides',
        collapsed: false,
        items: [
          { text: 'Memory Optimization', link: '/en/guide/memory' },
          { text: 'Reduction Optimization', link: '/en/guide/reduction' },
          { text: 'GEMM Optimization', link: '/en/guide/gemm' },
          { text: 'FlashAttention', link: '/en/guide/flash-attention' },
          { text: 'CUDA 13 Features', link: '/en/guide/cuda13' }
        ]
      }
    ],
    '/zh-CN/': [
      {
        text: '介绍',
        collapsed: false,
        items: [
          { text: '什么是 HPC-AI-Lab？', link: '/zh-CN/' },
          { text: '快速开始', link: '/zh-CN/guide/quick-start' },
          { text: '安装', link: '/zh-CN/guide/installation' }
        ]
      },
      {
        text: '核心教程',
        collapsed: false,
        items: [
          { text: '访存优化', link: '/zh-CN/guide/memory' },
          { text: '归约优化', link: '/zh-CN/guide/reduction' },
          { text: 'GEMM 优化', link: '/zh-CN/guide/gemm' },
          { text: 'FlashAttention', link: '/zh-CN/guide/flash-attention' },
          { text: 'CUDA 13 特性', link: '/zh-CN/guide/cuda13' }
        ]
      }
    ]
  }
}

// 搜索配置
export function search(): DefaultTheme.LocalSearchOptions {
  return {
    provider: 'local',
    options: {
      locales: {
        'zh-CN': {
          translations: {
            button: {
              buttonText: '搜索文档',
              buttonAriaLabel: '搜索文档'
            },
            modal: {
              noResultsText: '无法找到相关结果',
              resetButtonTitle: '清除查询条件',
              footer: {
                selectText: '选择',
                navigateText: '切换',
                closeText: '关闭'
              }
            }
          }
        }
      },
      miniSearch: {
        searchOptions: {
          boost: { title: 4, text: 2, titles: 1 }
        }
      }
    }
  }
}
