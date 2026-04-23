## ADDED Requirements

### Requirement: Trustworthy local validation baseline
The repository SHALL document and maintain a minimum trustworthy configure, build, and test baseline for local cleanup work.

#### Scenario: Maintainer needs the baseline
- **WHEN** a maintainer prepares to validate a foundational change
- **THEN** the repository guidance identifies the canonical configure/build/test entry points
- **AND** it states any known limitations of the baseline explicitly

#### Scenario: Baseline does not currently validate expected targets
- **WHEN** the discovered local build tree does not expose the expected tests or validation targets
- **THEN** the cleanup work records the gap and restores a trustworthy baseline before treating validation results as authoritative

### Requirement: Minimal high-value LSP baseline
The repository SHALL provide a minimal, high-value LSP/editor baseline centered on `clangd` and repository-generated compile commands.

#### Scenario: C++ or CUDA contributor configures an editor
- **WHEN** the contributor sets up code intelligence for this repository
- **THEN** the documented primary path uses `clangd` with `compile_commands.json`

#### Scenario: Additional editor tooling is considered
- **WHEN** maintainers evaluate extra LSP integrations or editor plugins
- **THEN** they only adopt them if they provide concrete recurring value beyond the `clangd` baseline

### Requirement: Purposeful local automation
Hooks and local engineering automation MUST stay narrow, repository-specific, and justified by recurring value.

#### Scenario: Hook or local check is proposed
- **WHEN** maintainers consider adding or keeping a local hook or automation step
- **THEN** they verify that it protects a real recurring failure mode in this repository

#### Scenario: Automation is broad but low-signal
- **WHEN** a hook or local automation step adds friction without strong project-specific payoff
- **THEN** it is simplified, scoped down, or removed
