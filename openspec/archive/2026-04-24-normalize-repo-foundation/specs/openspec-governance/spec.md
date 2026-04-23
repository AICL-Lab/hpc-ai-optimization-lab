## ADDED Requirements

### Requirement: Canonical OpenSpec main specs
The repository SHALL maintain canonical main specifications under `openspec/specs/` for repository behaviors that govern ongoing work.

#### Scenario: Capability is made canonical
- **WHEN** a new foundational repository behavior is defined for ongoing maintenance
- **THEN** it is represented as a capability under `openspec/specs/<capability>/spec.md`

#### Scenario: Repository lacks a canonical requirement
- **WHEN** maintainers need to decide whether behavior is intentional
- **THEN** they consult `openspec/specs/` instead of ad hoc summary documents

### Requirement: OpenSpec-first change lifecycle
Significant repository changes SHALL follow the OpenSpec lifecycle of exploration, proposal, implementation, review, and archival.

#### Scenario: New bounded cleanup effort starts
- **WHEN** maintainers identify a non-trivial cleanup or restructuring task
- **THEN** they use `/opsx:explore` or equivalent investigation first
- **AND** they create a bounded change with `/opsx:propose` before implementation proceeds

#### Scenario: Change is ready to merge
- **WHEN** a change completes its implementation tasks
- **THEN** the workflow includes `/review` before merge
- **AND** the change is archived with `/opsx:archive` after completion

### Requirement: Closure-oriented workflow discipline
The repository MUST prefer serial, bounded changes and timely merges over parallel long-lived branches during the closure program.

#### Scenario: Multiple cleanup ideas exist
- **WHEN** maintainers plan follow-up work
- **THEN** they split it into a small sequence of bounded changes instead of one oversized change

#### Scenario: Assistant-driven implementation is used
- **WHEN** autopilot or similar assistant-driven execution is used for a change
- **THEN** the change remains scoped tightly enough to support a single long-running session without requiring `/fleet` by default
