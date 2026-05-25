# Changelog

This file is the single authoritative changelog for **HPC-AI-Optimization-Lab**.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/) and kept intentionally concise for the repository's stabilization phase.

## [Unreleased]

### Changed

- Removed the repository's OpenSpec and tool-specific AI workflow scaffolding to return the project to a simpler contributor model.
- Consolidated changelog history back to this root file and removed duplicated changelog pages, sync scripts, and side directories.
- Refocused GitHub Pages on guides, examples, and architecture instead of mirroring repository change history.
- Removed the experimental `src/cuda13` module, its examples, and its tests so the shipped CUDA surface matches the maintained modules only.
- Removed the experimental Winograd convolution branch and kept convolution centered on the validated implicit GEMM implementation.
- Dropped the VitePress LLM/Mermaid plugin bundle and tightened Pages navigation and reference docs around the retained core surface.

## [0.3.0] - 2026-04-16

### Changed

- Promoted the bilingual documentation surface and refreshed the repository presentation.
- Consolidated the public documentation and release presentation for the 0.3.0 stabilization milestone.

## [0.2.0] - 2025-01-15

### Added

- Basic open-source governance files and GitHub templates.
- Initial CI, formatting, and documentation-tooling baseline.

## [0.1.0] - 2024-01-01

### Added

- Initial CUDA kernel library structure across common utilities, elementwise ops, reduction, GEMM, convolution, attention, quantization, and experimental CUDA 13 paths.
- Initial testing, examples, and optional Python binding surface.

[Unreleased]: https://github.com/LessUp/hpc-ai-optimization-lab/compare/v0.3.0...HEAD
[0.3.0]: https://github.com/LessUp/hpc-ai-optimization-lab/compare/v0.2.0...v0.3.0
[0.2.0]: https://github.com/LessUp/hpc-ai-optimization-lab/compare/v0.1.0...v0.2.0
[0.1.0]: https://github.com/LessUp/hpc-ai-optimization-lab/releases/tag/v0.1.0
