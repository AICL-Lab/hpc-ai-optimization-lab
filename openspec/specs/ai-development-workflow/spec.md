## Requirements

### Requirement: AI workflow is OpenSpec-driven
Project-level AI collaboration guidance SHALL direct feature and cleanup work through OpenSpec artifacts before implementation begins.

#### Scenario: An AI agent starts non-trivial work
- **WHEN** the task affects repository structure, behavior, workflow, or documentation policy
- **THEN** the guidance routes the work through proposal, design, specs, and tasks under OpenSpec before code changes proceed

### Requirement: Review and delegation rules are explicit
The repository SHALL define when to use review-oriented commands, when to use subagents, and when direct implementation is preferred.

#### Scenario: A contributor chooses an AI execution mode
- **WHEN** the work involves risk, broad scope, or ambiguous effects
- **THEN** the guidance specifies whether to use review, exploration, or delegated execution instead of leaving the choice implicit

### Requirement: Workflow guidance minimizes branch and session drift
The AI workflow SHALL prefer timely integration, single-change focus, and long-running non-fleet execution for large stabilization tasks unless parallelism is clearly justified.

#### Scenario: A repository-wide cleanup is executed
- **WHEN** the task can be completed as one sequential stabilization effort
- **THEN** the guidance prefers a long autopilot-style session over fragmented branches or fleet-heavy execution
