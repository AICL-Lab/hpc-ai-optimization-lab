## Why

The repository has solid technical assets, but its project surface has drifted away from a reliable closure-ready shape. OpenSpec is only partially adopted, the top-level structure and source-of-truth files are unclear, AI/developer guidance is fragmented, and the current baseline is too noisy to support safe final cleanup work.

This change establishes the canonical foundation for the rest of the finishing program. It creates the repository rules, specs, and guidance needed to simplify later work without reintroducing drift.

## What Changes

- Define the authoritative repository structure, including which files remain at the root and which are removed, archived, or relocated.
- Rebuild canonical OpenSpec main specs under `openspec/specs/` so the repository has a clear spec source of truth again.
- Establish a closure-oriented OpenSpec operating model for future work, including when to use `/opsx:explore`, `/opsx:propose`, `/opsx:apply`, `/review`, and `/opsx:archive`.
- Normalize AI instruction files so `AGENTS.md`, `CLAUDE.md`, and `copilot-instructions.md` become concise, project-specific, and non-duplicative.
- Define the baseline developer tooling posture for this repository, including hooks, LSP/editor expectations, and minimal high-value local automation.
- Aggressively remove low-value summary and drift documents once canonical replacements exist.

## Capabilities

### New Capabilities
- `repo-foundation`: Defines the canonical top-level structure, source-of-truth ownership, and cleanup rules for repository documents and directories.
- `openspec-governance`: Defines how OpenSpec artifacts, main specs, change proposals, implementation, review, and archival work for this repository.
- `assistant-instructions`: Defines the repository-specific instruction stack for AGENTS, Claude, and Copilot guidance without redundant generic content.
- `development-baseline`: Defines the minimum trustworthy local development baseline for hooks, LSP/editor support, and validation entry points.

### Modified Capabilities
- None.

## Impact

- Affects repository root documentation and structure.
- Affects `openspec/`, including new canonical main specs and future change workflow.
- Affects AI/developer guidance files such as `AGENTS.md`, `CLAUDE.md`, and `copilot-instructions.md`.
- Affects local engineering configuration such as hooks and editor/LSP guidance.
- Creates the foundation that later changes will use to simplify CI/Pages and finalize GitHub-facing metadata.
