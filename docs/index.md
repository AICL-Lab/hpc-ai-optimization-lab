---
layout: home
hero:
  name: HPC-AI-Optimization-Lab
  text: Documentation entry
  tagline: Progressive CUDA kernel optimization lab with focused Python bindings, bilingual guides, and architecture notes.
  actions:
    - theme: brand
      text: English docs
      link: /en/
    - theme: alt
      text: 简体中文文档
      link: /zh-CN/
    - theme: alt
      text: GitHub
      link: https://github.com/LessUp/hpc-ai-optimization-lab
---

<style>
.entry-grid,
.status-grid {
  display: grid;
  gap: 16px;
  margin: 24px 0;
}

.entry-grid {
  grid-template-columns: repeat(auto-fit, minmax(220px, 1fr));
}

.status-grid {
  grid-template-columns: repeat(auto-fit, minmax(240px, 1fr));
}

.entry-card,
.status-card {
  display: block;
  padding: 18px;
  border: 1px solid var(--vp-c-divider);
  border-radius: 14px;
  background: var(--vp-c-bg-soft);
}

.entry-card:hover,
.status-card:hover {
  border-color: var(--vp-c-brand-1);
  text-decoration: none;
}

.entry-card strong,
.status-card strong {
  display: block;
  margin-bottom: 6px;
}

.status-card small {
  color: var(--vp-c-text-2);
}
</style>

## Start here

<div class="entry-grid">
  <a class="entry-card" href="/en/">
    <strong>English overview</strong>
    Newcomer landing page, guide routing, architecture, and examples.
  </a>
  <a class="entry-card" href="/zh-CN/">
    <strong>简体中文首页</strong>
    中文总览、学习路径、参考资料与示例入口。
  </a>
  <a class="entry-card" href="/en/guide/">
    <strong>Guide index</strong>
    Installation, quick start, memory, reduction, GEMM, FlashAttention, CUDA 13.
  </a>
  <a class="entry-card" href="/en/api/index">
    <strong>API + architecture</strong>
    Public C++/Python reference plus architecture overview.
  </a>
</div>

## Current shipped surface

<div class="status-grid">
  <div class="status-card">
    <strong>GEMM path</strong>
    Steps 1-5 ship today. Step 6 currently delegates to Step 5 for stability. Step 7 remains planned work.
  </div>
  <div class="status-card">
    <strong>Python bindings</strong>
    Current shipped Python modules: <code>elementwise</code>, <code>reduction</code>, <code>gemm</code>.
  </div>
  <div class="status-card">
    <strong>Validation posture</strong>
    CUDA build and runtime verification require local or self-hosted GPU infrastructure; GitHub-hosted runners do not provide CUDA.
  </div>
  <div class="status-card">
    <strong>Best next step</strong>
    Read the language-specific landing page first, then jump to guides, examples, or API depending on your goal.
  </div>
</div>

## Route by goal

<div class="entry-grid">
  <a class="entry-card" href="/en/guide/quick-start">
    <strong>I want to run something</strong>
    Use quick start, then basic Python or C++ examples.
  </a>
  <a class="entry-card" href="/en/examples/index">
    <strong>I want concrete examples</strong>
    Browse runnable examples before reading deeper theory.
  </a>
  <a class="entry-card" href="/en/api/architecture">
    <strong>I want architecture context</strong>
    Review module layout, current seams, and implementation boundaries.
  </a>
  <a class="entry-card" href="/en/changelog">
    <strong>I want recent changes</strong>
    Read changelog before comparing docs against older clones or screenshots.
  </a>
</div>
