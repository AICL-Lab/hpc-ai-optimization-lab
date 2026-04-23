## 1. Baseline audit

- [x] 1.1 Inventory the dirty worktree and classify overlapping edits as user-intended work, generated drift, or unresolved changes that require caution.
- [x] 1.2 Audit root-level documents, summary files, and structural leftovers to identify what should remain canonical, be relocated, be archived, or be deleted.
- [x] 1.3 Rebuild and document the minimum trustworthy configure/build/test baseline, including any current gaps such as missing registered tests.

## 2. Repository foundation normalization

- [x] 2.1 Define and apply the canonical top-level repository structure and source-of-truth ownership for root docs, `docs/`, `openspec/`, `changelog/`, and engineering configuration.
- [x] 2.2 Aggressively remove or relocate low-value duplicate summaries and drift documents once canonical replacements or destinations exist.
- [x] 2.3 Recreate canonical main capability specs under `openspec/specs/` from the new foundation requirements.

## 3. OpenSpec workflow governance

- [x] 3.1 Update repository governance documentation so future work follows the bounded `explore -> propose -> apply -> review -> archive` lifecycle.
- [x] 3.2 Encode the closure-stage branching and merge discipline, including the preference for serial bounded changes and long-running `autopilot` over default `/fleet` usage.

## 4. Assistant instruction alignment

- [x] 4.1 Refactor `AGENTS.md` into the shared canonical assistant workflow layer for this repository.
- [x] 4.2 Create a concise root `CLAUDE.md` that adapts the shared workflow for Claude-specific usage without duplicating generic guidance.
- [x] 4.3 Generate and tailor `copilot-instructions.md` so Copilot follows the same repository-specific OpenSpec and cleanup expectations.
- [x] 4.4 Evaluate `QWEN.md` and either fold unique project knowledge into canonical instruction files or archive/remove it.

## 5. Development baseline and local tooling

- [x] 5.1 Right-size hooks and engineering configuration so each retained automation step protects a real recurring failure mode in this repository.
- [x] 5.2 Document the minimal high-value LSP/editor baseline around `clangd` and `compile_commands.json`, with lightweight guidance for CMake and Python only where justified.
- [x] 5.3 Document the repository’s MCP, plugin, and Copilot tradeoff posture so future sessions favor skills and built-in GitHub tooling unless heavier integrations show concrete recurring value.

## 6. Foundation validation and handoff

- [x] 6.1 Verify that the normalized foundation docs, specs, and guidance files are internally consistent and point to the same source-of-truth workflow.
- [x] 6.2 Prepare the next bounded OpenSpec change for delivery-surface simplification using the cleaned foundation as its baseline.
