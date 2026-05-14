import type { HeadConfig, DefaultTheme } from 'vitepress'
import { PAGES_URL, BASE_URL } from './site-config'

const SITE_URL = PAGES_URL + BASE_URL

// Head Configuration
export function head(): HeadConfig[] {
  return [
    // Character encoding and viewport
    ['meta', { charset: 'utf-8' }],
    ['meta', { name: 'viewport', content: 'width=device-width, initial-scale=1.0' }],
    
    // SEO Meta Tags
    ['meta', { name: 'description', content: 'CUDA kernel optimization lab for GEMM, FlashAttention, quantization, and GPU performance learning.' }],
    ['meta', { name: 'keywords', content: 'CUDA, GPU, kernel optimization, HPC, GEMM, FlashAttention, quantization, Tensor Core, nanobind, AI inference' }],
    ['meta', { name: 'author', content: 'HPC-AI-Optimization-Lab Contributors' }],
    ['meta', { name: 'theme-color', content: '#76B900' }],
    
    // Open Graph
    ['meta', { property: 'og:type', content: 'website' }],
    ['meta', { property: 'og:site_name', content: 'HPC-AI-Optimization-Lab' }],
    ['meta', { property: 'og:title', content: 'HPC-AI-Optimization-Lab | CUDA kernel optimization lab' }],
    ['meta', { property: 'og:description', content: 'CUDA kernel optimization lab for GEMM, FlashAttention, quantization, and GPU performance learning.' }],
    ['meta', { property: 'og:url', content: `${SITE_URL}/` }],
    ['meta', { property: 'og:image', content: `${SITE_URL}/og-image.svg` }],
    ['meta', { property: 'og:locale', content: 'en_US' }],
    ['meta', { property: 'og:locale:alternate', content: 'zh_CN' }],
    
    // Twitter Card
    ['meta', { name: 'twitter:card', content: 'summary_large_image' }],
    ['meta', { name: 'twitter:title', content: 'HPC-AI-Optimization-Lab' }],
    ['meta', { name: 'twitter:description', content: 'CUDA kernel optimization lab for GEMM, FlashAttention, quantization, and GPU performance learning.' }],
    ['meta', { name: 'twitter:image', content: `${SITE_URL}/og-image.svg` }],
    
    // Favicon and Icons
    ['link', { rel: 'icon', type: 'image/svg+xml', href: '/logo.svg' }],
    ['link', { rel: 'apple-touch-icon', sizes: '180x180', href: '/logo-large.svg' }],
    
    // Canonical URL
    ['link', { rel: 'canonical', href: `${SITE_URL}/` }],
    
    // Performance
    ['meta', { 'http-equiv': 'x-ua-compatible', content: 'ie=edge' }],
    
    // Preconnect
    ['link', { rel: 'preconnect', href: 'https://fonts.googleapis.com' }],
    ['link', { rel: 'preconnect', href: 'https://fonts.gstatic.com', crossorigin: '' }]
  ]
}

// Search Configuration
export function search(): DefaultTheme.LocalSearchOptions {
  return {
    provider: 'local',
    options: {
      locales: {
        'zh-CN': {
          translations: {
            button: {
              buttonText: '搜索',
              buttonAriaLabel: '搜索文档'
            },
            modal: {
              noResultsText: '未找到结果',
              resetButtonTitle: '清除',
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
