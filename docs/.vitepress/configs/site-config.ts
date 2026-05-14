/**
 * Site-wide configuration
 * Single source of truth for repository URLs, version, and other site metadata
 */

export const SITE_CONFIG = {
  repo: {
    owner: 'AICL-Lab',
    name: 'hpc-ai-optimization-lab',
    get url() {
      return `https://github.com/${this.owner}/${this.name}`
    },
    get pages() {
      return `https://${this.owner.toLowerCase()}.github.io/${this.name}`
    },
    get issues() {
      return `${this.url}/issues`
    },
    get releases() {
      return `${this.url}/releases`
    }
  },
  version: '0.3.0',
  title: 'HPC-AI-Optimization-Lab',
  description:
    'CUDA kernel optimization lab for GEMM, FlashAttention, quantization, and GPU performance learning.'
} as const

// Convenience exports for common use
export const REPO_URL = SITE_CONFIG.repo.url
export const PAGES_URL = SITE_CONFIG.repo.pages
export const BASE_URL = `/${SITE_CONFIG.repo.name}/`
