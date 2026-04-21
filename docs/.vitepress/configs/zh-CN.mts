import type { DefaultTheme, LocaleSpecificConfig } from 'vitepress'

export default {
  label: '简体中文',
  lang: 'zh-CN',
  description: '面向 AI 工作负载的综合 CUDA 内核优化实验室',
  
  themeConfig: {
    nav: [
      { text: '首页', link: '/zh-CN/' },
      {
        text: '教程',
        items: [
          { text: 'GEMM 优化', link: '/zh-CN/guide/gemm' },
          { text: '访存优化', link: '/zh-CN/guide/memory' },
          { text: '归约优化', link: '/zh-CN/guide/reduction' },
          { text: 'FlashAttention', link: '/zh-CN/guide/flash-attention' },
          { text: 'CUDA 13 特性', link: '/zh-CN/guide/cuda13' }
        ]
      },
      {
        text: 'API 参考',
        items: [
          { text: 'C++ / CUDA API', link: '/cpp-api/index.html', target: '_blank' },
          { text: 'Python API', link: '/python-api/index.html', target: '_blank' }
        ]
      },
      { 
        text: 'v0.3.0',
        items: [
          { text: '更新日志', link: '/zh-CN/changelog' },
          { text: 'GitHub Releases', link: 'https://github.com/LessUp/hpc-ai-optimization-lab/releases' }
        ]
      }
    ],
    
    sidebar: {
      '/zh-CN/guide/': [
        {
          text: '入门',
          collapsed: false,
          items: [
            { text: '介绍', link: '/zh-CN/guide/' },
            { text: '安装', link: '/zh-CN/guide/installation' },
            { text: '快速开始', link: '/zh-CN/guide/quick-start' }
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
      ],
      '/zh-CN/api/': [
        {
          text: 'API 参考',
          collapsed: false,
          items: [
            { text: '概览', link: '/zh-CN/api/' },
            { text: 'C++ / CUDA', link: '/cpp-api/index.html', target: '_blank' },
            { text: 'Python', link: '/python-api/index.html', target: '_blank' },
            { text: '架构', link: '/zh-CN/api/architecture' }
          ]
        },
        {
          text: '模块',
          collapsed: true,
          items: [
            { text: '逐元素操作', link: '/zh-CN/api/elementwise' },
            { text: '归约', link: '/zh-CN/api/reduction' },
            { text: 'GEMM', link: '/zh-CN/api/gemm' },
            { text: '卷积', link: '/zh-CN/api/convolution' },
            { text: '注意力', link: '/zh-CN/api/attention' },
            { text: '量化', link: '/zh-CN/api/quantization' },
            { text: 'CUDA 13', link: '/zh-CN/api/cuda13' }
          ]
        }
      ],
      '/zh-CN/examples/': [
        {
          text: '示例',
          collapsed: false,
          items: [
            { text: '概览', link: '/zh-CN/examples/' },
            { text: '逐元素操作', link: '/zh-CN/examples/elementwise' },
            { text: '矩阵乘法', link: '/zh-CN/examples/gemm' },
            { text: '注意力', link: '/zh-CN/examples/attention' },
            { text: 'Python 绑定', link: '/zh-CN/examples/python' }
          ]
        }
      ]
    },
    
    outline: {
      label: '本页目录'
    },
    
    docFooter: {
      prev: '上一页',
      next: '下一页'
    },
    
    lastUpdated: {
      text: '最后更新于'
    },
    
    editLink: {
      pattern: 'https://github.com/LessUp/hpc-ai-optimization-lab/edit/main/docs/:path',
      text: '在 GitHub 上编辑此页'
    },
    
    footer: {
      message: '基于 Apache 2.0 许可发布',
      copyright: '版权所有 © 2024-2026 HPC-AI-Optimization-Lab 贡献者'
    },
    
    langMenuLabel: '切换语言',
    returnToTopLabel: '回到顶部',
    sidebarMenuLabel: '菜单',
    darkModeSwitchLabel: '外观',
    lightModeSwitchTitle: '切换到浅色主题',
    darkModeSwitchTitle: '切换到深色主题'
  }
} as LocaleSpecificConfig<DefaultTheme.Config>
