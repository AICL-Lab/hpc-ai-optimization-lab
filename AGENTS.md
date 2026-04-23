# AGENTS.md

Shared repository guidance for AI assistants.

## Repository posture

This project is in **stabilization / close-out mode**. Prefer simplification, consolidation, and trustworthiness over feature expansion. Delete or archive low-value material once a stronger canonical replacement exists.

## Canonical sources of truth

- **Active work**: `openspec/changes/<change>/`
- **Historical decisions**: `openspec/archive/`
- **Shared assistant workflow**: `AGENTS.md`
- **Tool adapters**: `CLAUDE.md`, `.github/copilot-instructions.md`
- **Public project entry**: `README.md`

Do not create overlapping active OpenSpec changes for the same scope.

## Required OpenSpec workflow

For any non-trivial repository change:

1. `/opsx:explore` to investigate and narrow scope
2. `/opsx:propose` to create a bounded change
3. `/opsx:apply` to implement from tasks
4. `/review` before merge
5. `/opsx:archive` after completion

Additional rules:

- Prefer **serial, bounded changes** over concurrent long-lived branches.
- Prefer one long `autopilot` session over `/fleet` unless parallelism is clearly worth the added drift.
- Keep changes small enough to review and archive cleanly.
- Update OpenSpec artifacts before expanding implementation scope.

## Project-specific cleanup rules

- `openspec/` is the only active planning system. Remove or rewrite legacy `specs/` references.
- Keep the repository root high-signal. Generic summaries, duplicated reports, and filler docs should not stay active.
- Public-facing copy must avoid unverifiable claims, stale completion percentages, and speculative roadmap noise.
- GitHub Pages should act as a focused landing page, not a verbatim README mirror.
- Preserve user-authored in-progress edits; do not revert or overwrite unclear work without understanding it first.

## Technical profile

- **Language / runtime**: C++20, CUDA 12.4+, optional Python bindings via nanobind
- **Build**: CMake 3.24+ with presets in `CMakePresets.json`
- **Tests**: GoogleTest + RapidCheck
- **Primary code intelligence**: `clangd` using generated `compile_commands.json`
- **Preferred GitHub interface**: `gh`

Validation baseline:

```bash
cmake --preset default
cmake --build --preset default
ctest --preset default
```

If the configured build tree exposes zero tests or stale results, reconfigure before trusting it.

## Tooling and automation posture

- Keep hooks and automation narrow; every retained check should protect a real recurring failure mode.
- Prefer native tooling, built-in skills, and `gh` before adding MCP servers or plugins.
- Only commit editor or AI-tool config that improves repository-wide consistency. Personal preferences stay local.

## Review guidance

Use `/review` for:

- broad refactors
- workflow changes
- deletion-heavy cleanup
- GitHub-facing copy or policy changes

Use subagents only for independent investigation threads. Do not delegate work that is tightly coupled to an active local edit sequence.
