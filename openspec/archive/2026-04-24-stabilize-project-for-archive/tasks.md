## 1. Baseline and source-of-truth alignment

- [x] 1.1 Audit the current repository state, including uncommitted user changes, and record the cleanup boundaries that must not be violated
- [x] 1.2 Remove or replace active references to the legacy `specs/` workflow so `openspec/` becomes the sole active planning system
- [x] 1.3 Normalize the top-level repository information architecture and decide which low-value assets are deleted versus archived

## 2. OpenSpec and governance restructuring

- [x] 2.1 Rewrite active repository governance around the new close-out goal and remove stale roadmap or completion narratives that no longer match project intent
- [x] 2.2 Consolidate or archive outdated changelog and release-note material so active change history has a small, trustworthy surface
- [x] 2.3 Ensure repository-level contribution and security guidance matches the retained process and repository posture

## 3. Core documentation redesign

- [x] 3.1 Rewrite README and related entry documents to present the project clearly without unverifiable claims or duplicated guidance
- [x] 3.2 Redesign the active documentation surface so it serves the stabilization phase and removes low-signal or redundant pages
- [x] 3.3 Decide which supporting documentation stacks remain active and remove or archive the ones that do not justify their maintenance cost

## 4. AI workflow and project instructions

- [x] 4.1 Rewrite `AGENTS.md` so it reflects the actual OpenSpec-driven workflow and the repository's close-out priorities
- [x] 4.2 Add a project-specific `CLAUDE.md` and align any existing model-specific guidance files with the same workflow
- [x] 4.3 Run `/init` to generate Copilot instructions, then rewrite them into a concise project-specific `.github/copilot-instructions.md`
- [x] 4.4 Document when to use `/opsx:*`, `/review`, subagents, autopilot, and when to avoid `/fleet` for this repository

## 5. Developer tooling profile

- [x] 5.1 Standardize the primary LSP path around `clangd` and repository-generated compilation metadata
- [x] 5.2 Add only the minimal shared editor or tooling configuration that materially improves cross-contributor consistency
- [x] 5.3 Document the repository's minimal MCP and plugin strategy, preferring native tooling and `gh` where possible

## 6. Workflow and automation simplification

- [x] 6.1 Simplify CI so configured checks fail truthfully and remove weak or appearance-driven validation steps
- [x] 6.2 Redesign the Pages workflow around the retained documentation surface and remove placeholder-success behavior
- [x] 6.3 Reduce hooks and engineering configuration to the smallest set of tools that provide clear repository value

## 7. GitHub presentation and repository metadata

- [x] 7.1 Redesign GitHub Pages into a focused landing page that complements rather than mirrors the README
- [x] 7.2 Update GitHub About description, homepage URL, and topics via `gh` so public metadata matches the new project positioning
- [x] 7.3 Align README, Pages, and repository metadata so all public-facing surfaces tell the same story

## 8. Stabilization sweep and close-out backlog

- [x] 8.1 Run a repository-wide bug and consistency sweep covering docs, workflows, tooling, and governance surfaces touched by this change
- [x] 8.2 Fix the identified trust-reducing issues and route any remaining work into the active OpenSpec task list
- [x] 8.3 Produce a compact post-cleanup backlog that defines what, if anything, remains before the repository is considered archive-ready
