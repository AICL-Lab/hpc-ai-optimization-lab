## Why

The repository has drifted away from a single, trustworthy project model: OpenSpec adoption is incomplete, documentation and workflows are noisy and inconsistent, and GitHub-facing presentation does not match the project's close-out goal. This change is needed now to aggressively normalize the repository into a low-noise, archive-ready state before any further maintenance work compounds the drift.

## What Changes

- Re-establish OpenSpec as the canonical planning and execution system for repository-wide changes.
- Normalize repository structure and remove legacy `specs/` drift, duplicate governance sources, and low-value documentation.
- Redesign core project documents so they are specific to this project's stabilization phase rather than generic open-source boilerplate.
- Simplify CI, Pages, hooks, and engineering configuration to a smaller set of trustworthy quality gates.
- Redesign GitHub Pages, repository About metadata, and topic strategy so the public presentation clearly explains the project and points to the right landing page.
- Add project-specific AI collaboration guidance for AGENTS, Claude, Copilot, and related tool usage, including low-noise review and autopilot practices.
- Standardize local development guidance around a primary LSP strategy and minimal MCP/plugin footprint.
- Perform a repository-wide bug and consistency sweep for issues directly affecting build trust, documentation trust, and repository governance.

## Capabilities

### New Capabilities
- `repository-governance`: Define archive-ready repository structure, source-of-truth documents, and deletion/archive rules for low-value assets.
- `documentation-rationalization`: Define how README, core governance docs, changelog strategy, and supporting documentation are rewritten, reduced, or archived.
- `automation-rationalization`: Define trustworthy CI, Pages, hooks, and engineering automation behavior with low-noise failure semantics.
- `github-presentation`: Define the public-facing GitHub About, homepage, topics, and Pages landing-page requirements.
- `ai-development-workflow`: Define project-specific OpenSpec-driven collaboration rules for AGENTS, Claude, Copilot, review flows, autopilot usage, and long-session development.
- `developer-tooling-profile`: Define the primary LSP, editor/tooling expectations, and minimal MCP/plugin strategy for this repository.
- `stabilization-sweep`: Define how repository-wide bug, inconsistency, and drift cleanup is identified and completed before close-out.

### Modified Capabilities
- None currently defined in `openspec/specs/`; this change establishes the initial active capability set for the current repository model.

## Impact

- Affected areas include `openspec/`, root governance documents, `docs/`, `.github/workflows/`, Git hooks/tooling configs, GitHub repository metadata, and cross-cutting repository conventions.
- This change is repository-wide and may remove or archive outdated files, rewrite public-facing content, and tighten automation behavior.
- No new runtime dependency is planned by default; the focus is simplification, normalization, and trustworthiness.
