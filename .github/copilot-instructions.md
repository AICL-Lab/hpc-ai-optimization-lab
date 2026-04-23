# Copilot Instructions

Read `AGENTS.md` first. These instructions adapt the shared repository workflow to GitHub Copilot CLI and related Copilot surfaces.

## Default Copilot flow

- Use `/plan` or `/opsx:explore` for broad cleanup and ambiguity.
- Use `/opsx:propose` for any non-trivial change before implementation.
- Use `/opsx:apply` to execute a bounded task list.
- Use `/review` before merge or before concluding a deletion-heavy cleanup.
- Use `/opsx:archive` after the change is complete.

## Copilot-specific guidance

- Prefer `autopilot` for long sequential stabilization work.
- Avoid `/fleet` unless the work truly decomposes into independent parallel tracks.
- Use `/remote` only when you need GitHub/web control of the same bounded session, not as a way to create process drift.
- Use `/research` for current external information; do not use it for codebase questions that local files or GitHub context already answer.
- Use `/lsp` with the repository-level `.github/lsp.json` profile for code intelligence.
- Prefer `gh` and built-in GitHub capabilities before adding MCP servers or plugins.

## Repository priorities

- Treat `openspec/` as the active planning system.
- Keep the repo root and active docs surface high-signal.
- Rewrite or remove stale claims instead of layering new copy on top of them.
- Keep assistant guidance aligned across `AGENTS.md`, `CLAUDE.md`, and this file.
