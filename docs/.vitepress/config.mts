import { defineConfig } from 'vitepress'
import { head, nav, sidebar, search } from './configs/index.mts'
import enConfig from './configs/en.mts'
import zhCNConfig from './configs/zh-CN.mts'

// https://vitepress.dev/reference/site-config
export default defineConfig({
  // 站点元数据
  lang: 'en-US',
  title: 'HPC-AI-Optimization-Lab',
  titleTemplate: ':title | HPC-AI-Optimization-Lab',
  description: 'A comprehensive CUDA kernel optimization laboratory for AI workloads',
  
  // 基础配置
  srcDir: '.',
  srcExclude: ['**/README.md', '**/node_modules/**'],
  outDir: './.vitepress/dist',
  cacheDir: './.vitepress/cache',
  
  // 清理 URL
  cleanUrls: true,
  
  // 最后更新时间
  lastUpdated: true,
  
  // 多语言配置
  locales: {
    root: {
      label: 'English',
      lang: 'en-US',
      link: '/en/',
      ...enConfig
    },
    'zh-CN': {
      label: '简体中文',
      lang: 'zh-CN',
      link: '/zh-CN/',
      ...zhCNConfig
    }
  },
  
  // Head 配置
  head: head(),
  
  // 主题配置
  themeConfig: {
    // Logo
    logo: { src: '/logo.svg', width: 24, height: 24 },
    
    // 站点标题
    siteTitle: 'HPC-AI-Lab',
    
    // 导航栏
    nav: nav(),
    
    // 侧边栏
    sidebar: sidebar(),
    
    // 搜索配置
    search: search(),
    
    // 社交链接
    socialLinks: [
      { icon: 'github', link: 'https://github.com/LessUp/hpc-ai-optimization-lab' }
    ],
    
    // 编辑链接
    editLink: {
      pattern: 'https://github.com/LessUp/hpc-ai-optimization-lab/edit/main/docs/:path',
      text: 'Edit this page on GitHub'
    },
    
    // 页脚
    footer: {
      message: 'Released under the Apache 2.0 License.',
      copyright: 'Copyright © 2024-2026 HPC-AI-Optimization-Lab Contributors'
    },
    
    // 广告
    carbonAds: undefined,
    
    // 文档页脚
    docFooter: {
      prev: 'Previous page',
      next: 'Next page'
    },
    
    // 大纲
    outline: {
      label: 'On this page',
      level: [2, 4]
    },
    
    // 最后更新时间文本
    lastUpdated: {
      text: 'Last updated',
      formatOptions: {
        dateStyle: 'full',
        timeStyle: 'medium'
      }
    },
    
    // 返回顶部
    returnToTopLabel: 'Return to top',
    
    // 菜单
    sidebarMenuLabel: 'Menu',
    langMenuLabel: 'Change language',
    
    // 暗色模式
    darkModeSwitchLabel: 'Appearance',
    lightModeSwitchTitle: 'Switch to light theme',
    darkModeSwitchTitle: 'Switch to dark theme',
    
    // 外部链接图标
    externalLinkIcon: true
  },
  
  // Markdown 配置
  markdown: {
    theme: {
      light: 'github-light',
      dark: 'github-dark'
    },
    lineNumbers: true,
    config: async (md) => {
      // 自定义 Markdown 插件
      const { default: footnote } = await import('markdown-it-footnote')
      const { default: taskLists } = await import('markdown-it-task-lists')
      md.use(footnote)
      md.use(taskLists)
    }
  },
  
  // Vite 配置
  vite: {
    server: {
      port: 5173,
      host: true
    },
    build: {
      chunkSizeWarningLimit: 1000,
      rollupOptions: {
        output: {
          manualChunks: {
            'group-zh': [
              './configs/zh-CN.mts'
            ]
          }
        }
      }
    },
    ssr: {
      noExternal: [
        '@vitepress-demo-preview/component',
        '@vitepress-demo-preview/plugin'
      ]
    },
    resolve: {
      alias: {
        '@': '/.vitepress'
      }
    }
  },
  
  // 缓存重写
  transformPageData: (pageData) => {
    // 添加自定义页面数据
    pageData.frontmatter.head ??= []
    pageData.frontmatter.head.push([
      'meta',
      { property: 'og:url', content: `https://lessup.github.io/hpc-ai-optimization-lab${pageData.relativePath}` }
    ])
  }
})
