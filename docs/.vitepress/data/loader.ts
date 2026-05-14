/**
 * Data Loader for Vue Components
 * Provides localized data from YAML files
 */
import yaml from 'js-yaml'

// Import YAML files as raw strings (Vite feature)
import quickStartRaw from './quick-start.yaml?raw'
import gemmStepsRaw from './gemm-steps.yaml?raw'
import featuresRaw from './features.yaml?raw'
import citationRaw from './citation.yaml?raw'

// Type definitions
type Locale = 'en-US' | 'zh-CN'

interface LocalizedText {
  'en-US': string
  'zh-CN': string
}

interface QuickStartStep {
  number: number
  title: LocalizedText
  code: string
}

interface QuickStartData {
  steps: QuickStartStep[]
  requirements: string[]
  labels: Record<Locale, {
    title: string
    subtitle: string
    requirements: string
    copy: string
    copied: string
  }>
}

interface GemmStep {
  step: number
  name: LocalizedText
  tflops: number
  color: 'low' | 'medium' | 'high'
  planned?: boolean
}

interface GemmStepsData {
  maxTflops: number
  steps: GemmStep[]
  labels: Record<Locale, {
    title: string
    subtitle: string
    planned: string
    keyInsight: string
    legend: { low: string; medium: string; high: string }
  }>
}

interface Feature {
  icon: string
  title: LocalizedText
  description: LocalizedText
  tags: string[]
}

interface FeaturesData {
  features: Feature[]
  labels: Record<Locale, {
    title: string
    subtitle: string
  }>
}

interface CitationData {
  labels: Record<Locale, {
    title: string
    subtitle: string
    copyCitation: string
    copied: string
    formatTabs: string[]
    links: { repo: string; issue: string; docs: string }
  }>
}

// Parse YAML files
const quickStartData = yaml.load(quickStartRaw) as QuickStartData
const gemmStepsData = yaml.load(gemmStepsRaw) as GemmStepsData
const featuresData = yaml.load(featuresRaw) as FeaturesData
const citationData = yaml.load(citationRaw) as CitationData

/**
 * Get localized quick start data
 */
export function getQuickStartData(locale: Locale) {
  return {
    steps: quickStartData.steps.map(step => ({
      number: step.number,
      title: step.title[locale],
      code: step.code
    })),
    requirements: quickStartData.requirements,
    labels: quickStartData.labels[locale]
  }
}

/**
 * Get localized GEMM steps data
 */
export function getGemmStepsData(locale: Locale) {
  return {
    maxTflops: gemmStepsData.maxTflops,
    steps: gemmStepsData.steps.map(step => ({
      step: step.step,
      name: step.name[locale],
      tflops: step.tflops,
      color: step.color,
      planned: step.planned
    })),
    labels: gemmStepsData.labels[locale]
  }
}

/**
 * Get localized features data
 */
export function getFeaturesData(locale: Locale) {
  return {
    features: featuresData.features.map(f => ({
      icon: f.icon,
      title: f.title[locale],
      description: f.description[locale],
      tags: f.tags
    })),
    labels: featuresData.labels[locale]
  }
}

/**
 * Get localized citation data
 */
export function getCitationData(locale: Locale) {
  return {
    labels: citationData.labels[locale]
  }
}

/**
 * Helper function to format TFLOPS display
 */
export function formatTflops(tflops: number): string {
  if (tflops >= 50) return `${tflops}+ TFLOPS`
  return `${tflops} TFLOPS`
}

/**
 * Helper function to calculate speedup
 */
export function getSpeedup(tflops: number): string {
  return `${Math.round(tflops / 0.5)}×`
}

// Export raw data for components that need it
export {
  quickStartData,
  gemmStepsData,
  featuresData,
  citationData
}
