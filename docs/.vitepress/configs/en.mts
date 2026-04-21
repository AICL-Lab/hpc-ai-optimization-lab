import type { DefaultTheme, LocaleSpecificConfig } from 'vitepress'

export default {
  label: 'English',
  lang: 'en-US',
  description: 'A comprehensive CUDA kernel optimization laboratory for AI workloads',

  themeConfig: {
    nav: [
      { text: 'Home', link: '/en/' },
      {
        text: 'Guide',
        items: [
          { text: 'GEMM Optimization', link: '/en/guide/gemm' },
          { text: 'Memory Optimization', link: '/en/guide/memory' },
          { text: 'Reduction Optimization', link: '/en/guide/reduction' },
          { text: 'FlashAttention', link: '/en/guide/flash-attention' },
          { text: 'CUDA 13 Features', link: '/en/guide/cuda13' }
        ]
      },
      {
        text: 'API Reference',
        items: [
          { text: 'C++ / CUDA API', link: '/cpp-api/index.html', target: '_blank' },
          { text: 'Python API', link: '/python-api/index.html', target: '_blank' }
        ]
      },
      {
        text: 'v0.3.0',
        items: [
          { text: 'Changelog', link: '/en/changelog' },
          { text: 'GitHub Releases', link: 'https://github.com/LessUp/hpc-ai-optimization-lab/releases' }
        ]
      }
    ],

    sidebar: {
      '/en/guide/': [
        {
          text: 'Getting Started',
          collapsed: false,
          items: [
            { text: 'Introduction', link: '/en/guide/' },
            { text: 'Installation', link: '/en/guide/installation' },
            { text: 'Quick Start', link: '/en/guide/quick-start' }
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
      '/en/api/': [
        {
          text: 'API Reference',
          collapsed: false,
          items: [
            { text: 'Overview', link: '/en/api/' },
            { text: 'C++ / CUDA', link: '/cpp-api/index.html', target: '_blank' },
            { text: 'Python', link: '/python-api/index.html', target: '_blank' },
            { text: 'Architecture', link: '/en/api/architecture' }
          ]
        },
        {
          text: 'Modules',
          collapsed: true,
          items: [
            { text: 'Elementwise', link: '/en/api/elementwise' },
            { text: 'Reduction', link: '/en/api/reduction' },
            { text: 'GEMM', link: '/en/api/gemm' },
            { text: 'Convolution', link: '/en/api/convolution' },
            { text: 'Attention', link: '/en/api/attention' },
            { text: 'Quantization', link: '/en/api/quantization' },
            { text: 'CUDA 13', link: '/en/api/cuda13' }
          ]
        }
      ],
      '/en/examples/': [
        {
          text: 'Examples',
          collapsed: false,
          items: [
            { text: 'Overview', link: '/en/examples/' },
            { text: 'Elementwise Operations', link: '/en/examples/elementwise' },
            { text: 'Matrix Multiplication', link: '/en/examples/gemm' },
            { text: 'Attention', link: '/en/examples/attention' },
            { text: 'Python Bindings', link: '/en/examples/python' }
          ]
        }
      ]
    },

    outline: {
      label: 'On this page'
    },

    docFooter: {
      prev: 'Previous page',
      next: 'Next page'
    },

    lastUpdated: {
      text: 'Last updated'
    },

    editLink: {
      pattern: 'https://github.com/LessUp/hpc-ai-optimization-lab/edit/main/docs/:path',
      text: 'Edit this page on GitHub'
    },

    footer: {
      message: 'Released under the Apache 2.0 License.',
      copyright: 'Copyright © 2024-2026 HPC-AI-Optimization-Lab Contributors'
    },

    langMenuLabel: 'Change language',
    returnToTopLabel: 'Return to top',
    sidebarMenuLabel: 'Menu',
    darkModeSwitchLabel: 'Appearance',
    lightModeSwitchTitle: 'Switch to light theme',
    darkModeSwitchTitle: 'Switch to dark theme'
  }
} as LocaleSpecificConfig<DefaultTheme.Config>
