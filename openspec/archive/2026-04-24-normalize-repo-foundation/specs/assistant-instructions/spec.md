## ADDED Requirements

### Requirement: Layered assistant instruction stack
The repository SHALL provide a layered assistant instruction stack in which shared project rules live in one canonical location and tool-specific files only add the minimum adapter guidance needed for that tool.

#### Scenario: Shared project rule is documented
- **WHEN** a workflow, architecture, or repository rule applies across assistants
- **THEN** it is defined in the shared canonical instruction layer
- **AND** tool-specific files reference or adapt it instead of duplicating it wholesale

#### Scenario: Tool-specific file is maintained
- **WHEN** `CLAUDE.md` or `copilot-instructions.md` is updated
- **THEN** it remains concise and avoids restating generic project rules that already exist elsewhere

### Requirement: Project-specific assistant guidance
Assistant guidance MUST describe this repository’s actual closure-stage workflow, OpenSpec usage, review checkpoints, and tooling tradeoffs rather than generic best-practice filler.

#### Scenario: Contributor reads assistant guidance
- **WHEN** a human or assistant consults repository guidance
- **THEN** they can understand the expected OpenSpec workflow, cleanup posture, and review cadence for this project specifically

#### Scenario: Generic filler is encountered
- **WHEN** guidance text could apply unchanged to almost any repository
- **THEN** it is rewritten, reduced, or removed in favor of repository-specific instructions

### Requirement: Cross-tool consistency
The repository SHALL keep `AGENTS.md`, `CLAUDE.md`, and `copilot-instructions.md` behaviorally aligned on core workflow expectations.

#### Scenario: OpenSpec flow changes
- **WHEN** the repository updates its preferred explore/propose/apply/review/archive flow
- **THEN** all maintained assistant instruction files reflect the same core expectation

#### Scenario: Tool strengths differ
- **WHEN** one assistant benefits from different wording or operational emphasis
- **THEN** the tool-specific guidance may adapt the phrasing
- **AND** it must not contradict the shared workflow contract
