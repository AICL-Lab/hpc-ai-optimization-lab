import { defineConfig } from 'vitepress'
import { withMermaid } from 'vitepress-plugin-mermaid'
import llmstxt from 'vitepress-plugin-llms'
import { head, search } from './configs/index.mts'
import enConfig from './configs/en.mts'
import zhCNConfig from './configs/zh-CN.mts'

const BASE_URL = '/hpc-ai-optimization-lab/'
const SITE_URL = 'https://lessup.github.io'

// https://vitepress.dev/reference/site-config
export default withMermaid(defineConfig({
  // Site Metadata
  lang: 'en-US',
  title: 'HPC-AI-Optimization-Lab',
  titleTemplate: ':title | HPC-AI-Optimization-Lab',
  description: 'CUDA kernel optimization lab for GEMM, FlashAttention, quantization, and GPU performance learning.',
  
  // Base Configuration
  base: BASE_URL,
  srcDir: '.',
  srcExclude: ['**/README.md', '**/node_modules/**'],
  outDir: './.vitepress/dist',
  cacheDir: './.vitepress/cache',
  
  // Clean URLs
  cleanUrls: true,

  // Last Updated
  lastUpdated: true,
  
  // Appearance (dark/light mode)
  appearance: true,
  
  // Sitemap
  sitemap: {
    hostname: SITE_URL + BASE_URL,
    lastmodDateOnly: true
  },
  
  // Robots
  robots: {
    allowAll: true
  },
  
  // Multi-language Configuration
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
  
  // Head Configuration
  head: head(),
  
  // Theme Configuration
  themeConfig: {
    // Logo
    logo: { src: '/logo.svg', width: 28, height: 28 },
    
    // Site Title
    siteTitle: 'HPC-AI-Optimization-Lab',
    
    // Search
    search: search(),
    
    // Social Links
    socialLinks: [
      { icon: 'github', link: 'https://github.com/LessUp/hpc-ai-optimization-lab' }
    ],
    
    // Edit Link
    editLink: {
      pattern: 'https://github.com/LessUp/hpc-ai-optimization-lab/edit/master/docs/:path',
      text: 'Edit on GitHub'
    },
    
    // Footer
    footer: {
      message: 'Released under the Apache 2.0 License.',
      copyright: 'Copyright © 2024-2026 HPC-AI-Optimization-Lab Contributors'
    },
    
    // Document Footer
    docFooter: {
      prev: 'Previous',
      next: 'Next'
    },
    
    // Outline
    outline: {
      label: 'On this page',
      level: [2, 4]
    },
    
    // Last Updated Text
    lastUpdated: {
      text: 'Updated',
      formatOptions: {
        dateStyle: 'medium',
        timeStyle: 'short'
      }
    },
    
    // Return to Top
    returnToTopLabel: 'Back to top',
    
    // Menu Labels
    sidebarMenuLabel: 'Menu',
    langMenuLabel: 'Languages',
    
    // Dark Mode Switch
    darkModeSwitchLabel: 'Theme',
    lightModeSwitchTitle: 'Switch to light',
    darkModeSwitchTitle: 'Switch to dark',
    
    // External Link Icon
    externalLinkIcon: true
  },
  
  // Markdown Configuration
  markdown: {
    theme: {
      light: 'github-light',
      dark: 'github-dark'
    },
    lineNumbers: true,
    config: async (md) => {
      const { default: footnote } = await import('markdown-it-footnote')
      const { default: taskLists } = await import('markdown-it-task-lists')
      md.use(footnote)
      md.use(taskLists)
    }
  },
  
  // Vite Configuration
  vite: {
    plugins: [
      llmstxt({
        domain: `${SITE_URL}${BASE_URL}`,
        title: 'HPC-AI-Optimization-Lab',
        description: 'CUDA kernel optimization lab for GEMM, FlashAttention, quantization, and GPU performance learning.',
      })
    ],
    server: {
      port: 5173,
      host: true
    },
    build: {
      chunkSizeWarningLimit: 1000
    }
  },
  
	  // Transform Page Data
  transformPageData: (pageData) => {
    pageData.frontmatter.head ??= []

    // Open Graph URL
    pageData.frontmatter.head.push([
      'meta',
      { property: 'og:url', content: `${SITE_URL}${BASE_URL}${pageData.relativePath.replace(/\.md$/, '.html')}` }
    ])

    // Article modified time
    if (pageData.frontmatter.lastUpdated) {
      pageData.frontmatter.head.push([
        'meta',
        { property: 'article:modified_time', content: new Date(pageData.frontmatter.lastUpdated).toISOString() }
      ])
    }
  }
}))
