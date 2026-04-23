## Context

The repository currently mixes several competing sources of truth: archived OpenSpec artifacts, a partially removed legacy `specs/` tree, generic governance documents, an overgrown docs stack, and workflows that optimize for apparent completeness rather than trustworthy signal. The project owner wants to close out the current version quickly, make the repository presentable and internally coherent, and then shift into a low-maintenance state rather than continue expanding scope.

This change is cross-cutting. It touches repository structure, governance documents, GitHub metadata, Pages, workflows, AI collaboration instructions, and local developer tooling guidance. It must also preserve valuable in-progress user edits already present in the worktree while aggressively deleting or archiving low-value assets.

## Goals / Non-Goals

**Goals:**
- Make `openspec/` the only active planning and requirements system for repository-wide work.
- Establish a small, explicit set of canonical project documents and directory responsibilities.
- Reduce documentation, workflow, and tooling noise so repository behavior is trustworthy and maintainable.
- Align README, Pages, GitHub About, topics, and homepage around a single public positioning.
- Define a project-specific AI-assisted development workflow that fits long, low-maintenance stabilization work.
- Standardize the primary local tooling path around `clangd` and generated compilation metadata.
- Complete a repository-wide stabilization sweep and leave behind a compact, sequential backlog.

**Non-Goals:**
- Adding new CUDA kernel features or expanding module scope.
- Supporting every editor, IDE, AI client, or plugin with bespoke repository configuration.
- Preserving all historical roadmap, changelog, or documentation narrative if it no longer serves the project.
- Building a large multi-service contributor platform or high-ceremony process.

## Decisions

### 1. Use one umbrella OpenSpec change for repository stabilization

**Decision:** All work in this effort is scoped under `stabilize-project-for-archive` rather than split across many parallel changes.

**Rationale:** The repository drift is systemic, not isolated. A single umbrella change provides one backlog, one design, one set of capabilities, and one execution order suitable for long autopilot sessions.

**Alternatives considered:**
- **Multiple smaller changes:** clearer isolation, but it would reintroduce fragmentation and coordination overhead.
- **Direct repo edits without OpenSpec:** faster short-term, but contradicts the owner's goal of restoring OpenSpec discipline.

### 2. Make OpenSpec the sole active source of planning truth

**Decision:** Active planning, requirements, and execution MUST live under `openspec/`; legacy `specs/` content and references are removed or archived.

**Rationale:** The current dual-system state causes drift and ambiguous ownership. Eliminating the old path simplifies both human and AI workflows.

**Alternatives considered:**
- **Keep both `specs/` and `openspec/`:** rejected because it preserves ambiguity.
- **Mirror content between both systems:** rejected because it doubles maintenance and invites inconsistency.

### 3. Prefer aggressive deletion/archive over soft preservation

**Decision:** Low-value, generic, duplicated, or outdated assets are deleted or archived by default; only project-specific, high-signal artifacts remain in the active surface area.

**Rationale:** The repository is entering a close-out phase. Reducing active surface area lowers maintenance cost and improves clarity faster than incremental patching.

**Alternatives considered:**
- **Patch in place:** lower immediate disruption, but keeps low-quality assets alive.
- **Preserve all history in active paths:** rejected because it keeps navigation noisy and makes trust worse.

### 4. Rebuild the public-facing narrative around a landing-page model

**Decision:** GitHub Pages becomes a focused landing page for newcomers, while README remains the repository entry document and both are aligned with GitHub About metadata.

**Rationale:** The current Pages setup over-optimizes for documentation technology and under-optimizes for clear project positioning. A landing-page-first approach improves discoverability without requiring a sprawling docs portal.

**Alternatives considered:**
- **Use README as-is for Pages:** low effort, but weak public presentation.
- **Keep a broad documentation portal:** rejected unless it can be maintained with minimal complexity.

### 5. Simplify automation to trustworthy quality gates

**Decision:** CI and Pages workflows are reduced to checks that fail when they should fail, without placeholder artifacts, silent lint passes, or brittle text-coupled consistency rules.

**Rationale:** The owner explicitly wants fewer meaningless workflows. Trustworthy failure semantics matter more than a long checklist of nominal jobs.

**Alternatives considered:**
- **Keep current workflow breadth:** rejected because it produces noise and success-shaped behavior.
- **Remove most automation entirely:** rejected because the repo still needs a minimal quality baseline.

### 6. Standardize on clangd-centered local tooling

**Decision:** `clangd` plus generated `compile_commands.json` is the primary LSP path. Repository config should only include project-specific support such as `.clangd`, CMake presets, or editor hints when they materially improve this path.

**Rationale:** `clangd` is mature for C++/CUDA-adjacent workflows, broadly consumed by editor clients, and independent of any single AI model vendor.

**Alternatives considered:**
- **Per-tool LSP configuration:** rejected because LSP is editor/client infrastructure, not model-specific infrastructure.
- **Commit large editor-specific setups:** rejected unless strictly necessary for project consistency.

### 7. Prefer native GitHub and low-context tooling for repository governance

**Decision:** Repository metadata and workflow governance should prefer `gh`, native GitHub configuration, and narrowly scoped assistance over heavy always-on MCP usage.

**Rationale:** GitHub repository management is central to this effort, but high-context integrations are unnecessary for many of the tasks. This keeps the project lean and predictable.

**Alternatives considered:**
- **Broad MCP adoption:** more capability on paper, but higher context overhead and more maintenance.
- **No automation for GitHub settings:** rejected because About/homepage/topics are part of the stated objective.

## Risks / Trade-offs

- **[Risk] Aggressive deletion removes content the owner still values** → Mitigation: prefer archive/move when historical value exists; keep the new canonical surface intentionally small.
- **[Risk] Existing uncommitted user changes overlap with cleanup targets** → Mitigation: inspect worktree carefully before edits and avoid reverting user-authored deltas.
- **[Risk] Simplifying workflows drops a check that was catching regressions** → Mitigation: preserve only checks with clear signal and observable maintenance value.
- **[Risk] Public-facing messaging becomes too minimal** → Mitigation: design README, Pages, and About as complementary layers with distinct audiences.
- **[Risk] Tooling recommendations become too editor-specific** → Mitigation: define only shared repository-level expectations and leave personal editor preferences out of version control.
- **[Risk] Scope remains too broad for a single pass** → Mitigation: enforce a strict execution order and leave a concise backlog instead of trying to perfect every optional area.

## Migration Plan

1. Create the new OpenSpec capability specs and a sequential task list.
2. Normalize source-of-truth documents and remove legacy planning drift.
3. Rewrite the active governance and AI workflow documents.
4. Simplify workflows, hooks, and engineering configuration around the retained toolchain.
5. Redesign README, Pages, and GitHub metadata into a coherent public presentation.
6. Run a stabilization sweep for remaining inconsistencies and fix the ones directly tied to repository trust.
7. Leave only a compact backlog for any residual follow-up work.

Rollback is repository-state based rather than runtime-based: if a cleanup step proves too destructive, restore the specific removed artifact from git history before merge rather than preserving low-value files by default.

## Open Questions

- Whether the final active docs surface should retain Doxygen/Sphinx alongside the landing-page-oriented Pages site, or collapse further into a smaller static surface.
- Whether any current historical changelog or roadmap content deserves archival preservation rather than full removal.
- Whether a lightweight committed editor config (for example `.clangd` or `.vscode/extensions.json`) improves enough developer consistency to justify its maintenance cost.
