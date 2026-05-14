/**
 * Sync changelog from root CHANGELOG.md to docs
 *
 * This script reads the root CHANGELOG.md and creates localized versions
 * for the VitePress documentation site.
 */

import { readFileSync, writeFileSync, existsSync, mkdirSync } from 'fs'
import { join, dirname } from 'path'
import { fileURLToPath } from 'url'

const __filename = fileURLToPath(import.meta.url)
const __dirname = dirname(__filename)

const rootChangelog = join(__dirname, '..', '..', 'CHANGELOG.md')
const enChangelog = join(__dirname, '..', 'en', 'changelog.md')
const zhChangelog = join(__dirname, '..', 'zh-CN', 'changelog.md')

function syncChangelog() {
  if (!existsSync(rootChangelog)) {
    console.error('❌ Root CHANGELOG.md not found')
    process.exit(1)
  }

  const content = readFileSync(rootChangelog, 'utf-8')

  // Transform for VitePress format
  const transformContent = (content, lang) => {
    // Remove the first "# Changelog" heading (we'll add our own frontmatter)
    let transformed = content.replace(/^# Changelog\n\n/, '')

    // Add VitePress frontmatter
    const frontmatter = `---
title: ${lang === 'en' ? 'Changelog' : '更新日志'}
description: ${lang === 'en' ? 'Release history and changes for HPC-AI-Optimization-Lab' : 'HPC-AI-Optimization-Lab 发布历史与变更记录'}
---

# ${lang === 'en' ? 'Changelog' : '更新日志'}

`
    return frontmatter + transformed
  }

  // Write English version
  const enContent = transformContent(content, 'en')
  writeFileSync(enChangelog, enContent, 'utf-8')
  console.log('✓ Synced changelog to docs/en/changelog.md')

  // Write Chinese version
  const zhContent = transformContent(content, 'zh-CN')
  writeFileSync(zhChangelog, zhContent, 'utf-8')
  console.log('✓ Synced changelog to docs/zh-CN/changelog.md')

  console.log('✓ Changelog sync complete')
}

syncChangelog()
