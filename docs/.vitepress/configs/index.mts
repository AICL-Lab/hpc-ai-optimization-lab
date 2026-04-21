import type { HeadConfig, DefaultTheme } from 'vitepress'

const SITE_URL = 'https://lessup.github.io/hpc-ai-optimization-lab'

// Head 配置
export function head(): HeadConfig[] {
  return [
    // 字符编码和视口
    ['meta', { charset: 'utf-8' }],
    ['meta', { name: 'viewport', content: 'width=device-width, initial-scale=1.0' }],
    
    // SEO Meta Tags
    ['meta', { name: 'description', content: 'A comprehensive CUDA kernel optimization laboratory for AI workloads. Learn GPU programming from memory coalescing to Tensor Core optimization with GEMM, FlashAttention, and CUDA 13 features.' }],
    ['meta', { name: 'keywords', content: 'CUDA, GPU, optimization, HPC, AI, machine learning, Tensor Core, FlashAttention, GEMM, kernel optimization, NVIDIA, deep learning, parallel computing, CUDA programming' }],
    ['meta', { name: 'author', content: 'HPC-AI-Optimization-Lab Contributors' }],
    ['meta', { name: 'theme-color', content: '#0891b2' }],
    ['meta', { name: 'google-site-verification', content: '' }], // Add your Google Search Console verification
    
    // Open Graph
    ['meta', { property: 'og:type', content: 'website' }],
    ['meta', { property: 'og:site_name', content: 'HPC-AI-Optimization-Lab' }],
    ['meta', { property: 'og:title', content: 'HPC-AI-Optimization-Lab | CUDA Kernel Optimization' }],
    ['meta', { property: 'og:description', content: 'A comprehensive CUDA kernel optimization laboratory for AI workloads. From naive implementations to Tensor Core mastery.' }],
    ['meta', { property: 'og:url', content: `${SITE_URL}/` }],
    ['meta', { property: 'og:image', content: `${SITE_URL}/og-image.png` }],
    ['meta', { property: 'og:locale', content: 'en_US' }],
    ['meta', { property: 'og:locale:alternate', content: 'zh_CN' }],
    
    // Twitter Card
    ['meta', { name: 'twitter:card', content: 'summary_large_image' }],
    ['meta', { name: 'twitter:site', content: '@LessUp' }],
    ['meta', { name: 'twitter:title', content: 'HPC-AI-Optimization-Lab' }],
    ['meta', { name: 'twitter:description', content: 'CUDA kernel optimization laboratory: GEMM, FlashAttention, Tensor Core optimization' }],
    ['meta', { name: 'twitter:image', content: `${SITE_URL}/og-image.png` }],
    
    // Favicon and Icons
    ['link', { rel: 'icon', type: 'image/svg+xml', href: '/logo.svg' }],
    ['link', { rel: 'icon', type: 'image/png', sizes: '32x32', href: '/favicon-32x32.png' }],
    ['link', { rel: 'icon', type: 'image/png', sizes: '16x16', href: '/favicon-16x16.png' }],
    ['link', { rel: 'apple-touch-icon', sizes: '180x180', href: '/apple-touch-icon.png' }],
    ['link', { rel: 'manifest', href: '/site.webmanifest' }],
    
    // Canonical URL
    ['link', { rel: 'canonical', href: `${SITE_URL}/` }],
    
    // Performance and Security
    ['meta', { 'http-equiv': 'x-ua-compatible', content: 'ie=edge' }],
    ['meta', { name: 'format-detection', content: 'telephone=no' }]
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
        { text: 'FlashAttention', link: '/en/guide/flash-attention' },
        { text: 'CUDA 13 Features', link: '/en/guide/cuda13' }
      ]
    },
    {
      text: 'API',
      items: [
        { text: 'C++ / CUDA', link: '/cpp-api/index.html', target: '_blank' },
        { text: 'Python', link: '/python-api/index.html', target: '_blank' },
        { text: 'Architecture', link: '/en/api/architecture' }
      ]
    },
    { 
      text: 'v0.3.0',
      items: [
        { text: 'Changelog', link: '/en/changelog' },
        { text: 'GitHub Releases', link: 'https://github.com/LessUp/hpc-ai-optimization-lab/releases' }
      ]
    }
  ]
}

// 侧边栏配置
export function sidebar(): DefaultTheme.Sidebar {
  return {
    '/en/': [
      {
        text: 'Getting Started',
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
      },
      {
        text: 'Advanced',
        collapsed: true,
        items: [
          { text: 'Performance Tuning', link: '/en/guide/performance-tuning' },
          { text: 'Profiling', link: '/en/guide/profiling' },
          { text: 'Best Practices', link: '/en/guide/best-practices' },
          { text: 'Troubleshooting', link: '/en/guide/troubleshooting' }
        ]
      }
    ],
    '/zh-CN/': [
      {
        text: '入门',
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
      },
      {
        text: '高级',
        collapsed: true,
        items: [
          { text: '性能调优', link: '/zh-CN/guide/performance-tuning' },
          { text: '性能分析', link: '/zh-CN/guide/profiling' },
          { text: '最佳实践', link: '/zh-CN/guide/best-practices' },
          { text: '故障排除', link: '/zh-CN/guide/troubleshooting' }
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
