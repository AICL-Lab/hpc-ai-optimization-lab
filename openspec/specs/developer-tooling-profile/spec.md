---
Status: Active
Phase: Implemented
Last Updated: 2026-05-01
---

## Requirements

### Requirement: The repository defines a primary LSP path
The repository SHALL document `clangd` with generated compilation metadata as the primary LSP path for local development.

#### Scenario: A contributor asks how to enable code intelligence
- **WHEN** local editor or AI-assisted tooling setup is documented
- **THEN** the documented default points to `clangd` and the repository explains how compilation metadata is produced and consumed

### Requirement: Tooling guidance distinguishes shared repo config from personal preference
Repository-committed tooling configuration SHALL be limited to settings that improve project consistency across contributors.

#### Scenario: A new editor or plugin setting is proposed
- **WHEN** the setting only reflects an individual workflow preference
- **THEN** it is kept out of version control instead of being added as project policy

### Requirement: MCP and plugin usage remains minimal
The repository SHALL prefer low-context native tooling and SHALL only adopt MCPs or plugins that provide clear repository-specific value.

#### Scenario: A new integration is considered
- **WHEN** an MCP server or plugin duplicates capabilities already covered by native tooling, `gh`, skills, or existing editor support
- **THEN** the integration is not adopted as a default project dependency
