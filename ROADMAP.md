# Project Roadmap | 项目收尾方向

This repository is in a **finishing-and-hardening** phase. The goal is to leave the current version clean, credible, and easy to understand rather than to keep expanding the feature surface.

---

## Current focus | 当前重点

- Align repository workflow, specs, and change management around OpenSpec.
- Reduce low-value or duplicated documentation and keep the active docs surface high-signal.
- Simplify CI, Pages, and engineering automation so checks are trustworthy and low-noise.
- Improve GitHub-facing presentation so README, Pages, and About metadata tell the same story.
- Sweep repository-level bugs and inconsistencies that reduce build, docs, or workflow trust.

---

## Not a close-out priority | 当前不作为收尾优先项

- Large new kernel families or major API expansion
- Broad plugin bundles or heavy default MCP integrations
- Long speculative version tables and roadmap promises without bounded implementation work
- Public promises about future releases that are not already backed by active OpenSpec changes

---

## How follow-up work is tracked | 后续工作如何跟踪

- Non-trivial work goes through `openspec/changes/<change>/`.
- Historical decisions live in `openspec/archive/`.
- Any residual follow-up should be captured as bounded tasks or changes, not as speculative version plans.

---

## Stabilization completion criteria | 稳定化完成标准

The current cleanup program is considered complete when:

- active repository workflow and documentation all point to OpenSpec as the same source of truth
- GitHub-facing surfaces (README, Pages, About metadata) tell a consistent project story
- retained CI and Pages workflows are low-noise and fail honestly
- remaining follow-up work fits into a small bounded backlog instead of sprawling roadmap promises

---

## Remaining bounded backlog | 剩余 bounded backlog

- polish the Pages landing experience so it matches the cleaned README and GitHub About copy even more closely
- do one final repository-wide review pass for lingering stale claims, duplicate docs, or workflow edge cases
- complete and archive the active `stabilize-project-for-archive` OpenSpec change once its remaining tasks are closed
