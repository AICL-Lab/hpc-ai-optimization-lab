import { defineConfig } from 'vitepress'
import { head, search } from './configs/index.mts'
import enConfig from './configs/en.mts'
import zhCNConfig from './configs/zh-CN.mts'
import { SITE_CONFIG, BASE_URL, PAGES_URL } from './configs/site-config'

const SITE_URL = PAGES_URL

// https://vitepress.dev/reference/site-config
export default defineConfig({
  // Site Metadata
  lang: 'en-US',
  title: SITE_CONFIG.title,
  titleTemplate: `:title | ${SITE_CONFIG.title}`,
  description: SITE_CONFIG.description,
  
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
      { icon: 'github', link: SITE_CONFIG.repo.url }
    ],

    // Edit Link
    editLink: {
      pattern: `${SITE_CONFIG.repo.url}/edit/master/docs/:path`,
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
})
