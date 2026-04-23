## ADDED Requirements

### Requirement: Canonical repository structure
The repository SHALL define an authoritative top-level structure that makes source-of-truth ownership explicit for root documents, `docs/`, `openspec/`, `changelog/`, engineering configuration, and temporary or generated material.

#### Scenario: Canonical placement is documented
- **WHEN** a maintainer reviews the repository structure guidance
- **THEN** they can determine which files belong at the root
- **AND** they can determine which files must live under `docs/`, `openspec/`, `changelog/`, or archived locations

#### Scenario: Low-value root clutter is identified
- **WHEN** a root-level document only duplicates stronger canonical material
- **THEN** the repository structure guidance marks it for deletion or relocation

### Requirement: Replacement-before-deletion cleanup
The repository SHALL aggressively remove low-value summaries, drift documents, and overlapping guidance once a stronger canonical replacement exists.

#### Scenario: Low-value summary is superseded
- **WHEN** a summary or optimization report is replaced by canonical documentation or specs
- **THEN** the superseded file is deleted by default

#### Scenario: Historical material has reference value
- **WHEN** a low-value-looking file still provides audit or historical context that canonical docs do not preserve
- **THEN** the cleanup plan archives or relocates it instead of silently keeping it at the root

### Requirement: Dirty-worktree-safe cleanup
Foundation cleanup MUST classify existing in-progress edits before broad repository restructuring occurs.

#### Scenario: Existing modified file overlaps with cleanup scope
- **WHEN** a file targeted for normalization is already modified in the worktree
- **THEN** the implementation records whether the edit is user-intended, generated drift, or unknown before replacing or deleting it

#### Scenario: Unknown edit is encountered
- **WHEN** cleanup encounters a modified file whose intent cannot be inferred safely
- **THEN** the workflow pauses or isolates the uncertainty before destructive changes continue
