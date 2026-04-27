# Active Changes

This directory contains active OpenSpec changes during development.

## Status

**Currently**: No active changes in progress. The project is in stabilization/close-out mode.

## Workflow

For any non-trivial repository change, follow the OpenSpec workflow:

1. **Explore**: `/opsx:explore` - Investigate and narrow scope
2. **Propose**: `/opsx:propose` - Create a bounded change
3. **Apply**: `/opsx:apply` - Implement from tasks
4. **Review**: `/review` - Before merge
5. **Archive**: `/opsx:archive` - After completion

## Archive

Completed changes are archived in `../archive/` with date-prefixed directories.

## Guidelines

- Prefer serial, bounded changes over concurrent long-lived branches
- Keep changes small enough to review and archive cleanly
- Update OpenSpec artifacts before expanding implementation scope

For more details, see `AGENTS.md` and `openspec/specs/`.
