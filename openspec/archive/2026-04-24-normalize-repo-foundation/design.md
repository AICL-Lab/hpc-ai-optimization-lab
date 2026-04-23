## Context

The repository already contains substantial implementation work, but its non-code surface is inconsistent and partially migrated. OpenSpec is present but not yet acting as the canonical system of record, multiple root-level documents appear to be temporary or low-value summaries, and the current assistant/tooling guidance is fragmented across files with overlapping intent.

The repository is also in a dirty state today, so foundational cleanup must be careful about preserving user-authored edits while still enabling aggressive removal of low-value drift. This change is intentionally limited to foundation work so that later changes can simplify delivery workflows and public GitHub surfaces without mixing concerns.

## Goals / Non-Goals

**Goals:**
- Define the authoritative top-level repository structure and source-of-truth ownership.
- Re-establish OpenSpec as the canonical planning and specification system under `openspec/`.
- Create a concise, non-duplicative AI instruction stack across `AGENTS.md`, `CLAUDE.md`, and `copilot-instructions.md`.
- Establish the minimum trustworthy local development baseline for validation, hooks, and LSP/editor support.
- Encode a closure-oriented workflow that supports serial, bounded, low-drift changes.

**Non-Goals:**
- Redesign GitHub Pages or rewrite the public marketing narrative in full.
- Rationalize CI/Pages workflows beyond the minimum needed to support foundation work.
- Curate GitHub About metadata, homepage URL, or repository topics.
- Introduce new large dependencies, broad plugin ecosystems, or complex MCP topologies.

## Decisions

### Decision: Split the program into serial bounded OpenSpec changes
This cleanup program SHALL be executed as a small sequence of bounded changes rather than one umbrella change.

**Why:** The repository is already noisy and partially modified. Smaller changes reduce drift, make `/review` more meaningful, and align better with long-running `autopilot` sessions than a single oversized proposal.

**Alternatives considered:**
- One large “finalize everything” change: rejected because it would make specs vague and implementation too easy to derail.

### Decision: Use aggressive cleanup, but only after canonical replacement exists
Low-value documents, summaries, and overlapping guidance SHALL be deleted once a stronger canonical replacement exists. Archival SHALL be reserved for historically useful material.

**Why:** The user’s goal is fast closure, not preservation of every intermediate artifact. Keeping redundant files would continue the ambiguity the cleanup is meant to remove.

**Alternatives considered:**
- Archive-first for most material: rejected because it preserves too much noise and weakens source-of-truth clarity.

### Decision: Rebuild capability specs in OpenSpec before broad implementation
The repository SHALL regain canonical main specs through `openspec/specs/`, and future cleanup work SHALL flow through explicit OpenSpec capabilities and changes.

**Why:** The deleted legacy `/specs/*` tree and empty `openspec/specs/` directory leave the repository without a durable canonical requirements layer. Re-establishing that layer is the safest way to prevent further architecture drift.

**Alternatives considered:**
- Continue with ad hoc repo cleanup without canonical specs: rejected because it would repeat the drift pattern already present.

### Decision: Layer assistant instructions instead of duplicating them
`AGENTS.md` SHALL carry shared repository workflow and architectural rules. `CLAUDE.md` and `copilot-instructions.md` SHALL be thinner tool adapters that reuse the same canonical project guidance rather than restating everything.

**Why:** The repo needs project-specific guidance, but duplicated assistant docs become stale quickly and create conflicting instructions.

**Alternatives considered:**
- Put full guidance into every tool-specific file: rejected because it multiplies maintenance burden.
- Keep only one tool-specific file: rejected because the repository is used across multiple assistants.

### Decision: Prefer minimal, high-value local tooling
The baseline LSP/editor posture SHALL center on `clangd` backed by `compile_commands.json`, with minimal additional tooling. MCP/plugin additions SHALL only be introduced if they deliver repeated concrete value.

**Why:** This repository is nearing closure. Heavy tooling expansion would add maintenance cost without enough long-term payoff.

**Alternatives considered:**
- Add multiple MCP servers and editor-specific integrations up front: rejected because it increases complexity and context cost.

## Risks / Trade-offs

- **Dirty worktree overlap** → Audit and classify existing edits before sweeping deletions or file moves.
- **Over-scoping the foundation change** → Keep delivery-surface and GitHub-marketing work in later changes.
- **Specs becoming too abstract** → Write requirements against concrete repository behaviors and file ownership rules.
- **Assistant guidance drift returning later** → Make `AGENTS.md` the shared canonical layer and keep tool-specific files thin.

## Migration Plan

1. Audit the current worktree and identify overlapping in-progress edits.
2. Create the foundation capability specs and apply-ready task list.
3. Implement repository structure normalization and OpenSpec canonicalization first.
4. Apply assistant-instruction and development-baseline changes after source-of-truth ownership is clear.
5. Use `/review` before merging the change.
6. Archive the change after canonical specs are synced and tasks are complete.

## Open Questions

- Which currently modified docs/theme files are intentional user work versus generated or low-value drift?
- Whether any part of `QWEN.md` contains unique project knowledge worth folding into canonical assistant guidance.
- Whether any currently documented validation path can be trusted without reconfiguring the build tree first.
