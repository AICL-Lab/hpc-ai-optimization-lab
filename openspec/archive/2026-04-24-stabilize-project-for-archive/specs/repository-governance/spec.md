## ADDED Requirements

### Requirement: OpenSpec is the sole active planning system
The repository SHALL use `openspec/` as the only active system for change planning, requirements, and implementation tracking for repository-wide work.

#### Scenario: Legacy planning paths are encountered
- **WHEN** active planning content exists outside `openspec/`
- **THEN** that content is removed, archived, or replaced with references that point back to the relevant OpenSpec artifact

### Requirement: Repository structure has explicit ownership boundaries
The repository SHALL define a canonical purpose for each top-level active directory and SHALL avoid duplicate responsibility across documentation, specs, scripts, and engineering configuration.

#### Scenario: A file belongs to an ambiguous location
- **WHEN** a document or configuration file could plausibly live in more than one active location
- **THEN** the repository structure definition names a single canonical location and the duplicate path is removed or archived

### Requirement: Low-value active assets are aggressively reduced
The active repository surface SHALL exclude outdated, duplicated, generic, or low-signal assets unless they carry clear current value for this project.

#### Scenario: An outdated artifact is discovered
- **WHEN** a document, changelog entry, generated note, or governance file no longer reflects the current repository model
- **THEN** it is deleted, archived, or rewritten rather than left active beside the new source of truth
