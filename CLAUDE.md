# CLAUDE.md

Read `AGENTS.md` first. This file only adds Claude-specific operating guidance for this repository.

## Claude usage on this project

- Use `/opsx:explore` when the scope is unclear or when repository drift needs investigation before edits.
- Keep cleanup work inside one bounded OpenSpec change at a time.
- Prefer a long `autopilot` session for sequential stabilization work; do not default to `/fleet`.
- Use `/review` before finishing deletion-heavy, workflow-heavy, or public-facing changes.
- Prefer `gh` for repository metadata and workflow management.
- If a task needs up-to-date external information, use research deliberately instead of turning the whole session into open-ended browsing.

## What not to do

- Do not duplicate `AGENTS.md` in Claude-specific prose.
- Do not preserve low-value docs just because they already exist.
- Do not create parallel active changes for the same cleanup scope.
