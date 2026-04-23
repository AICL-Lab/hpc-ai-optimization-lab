## ADDED Requirements

### Requirement: Core documents have explicit audience and purpose
Each active core document SHALL state a project-specific purpose and SHALL avoid generic boilerplate that does not help contributors or readers of this repository.

#### Scenario: A core document is reviewed
- **WHEN** README, CONTRIBUTING, SECURITY, CHANGELOG, ROADMAP, AGENTS, or CLAUDE guidance is updated
- **THEN** the resulting content clearly serves a defined audience and removes non-specific template material

### Requirement: Active documentation matches the stabilization phase
The active documentation set SHALL describe the project as entering a low-noise stabilization and close-out phase without advertising speculative future expansion as the primary narrative.

#### Scenario: Roadmap-style content is evaluated
- **WHEN** a roadmap, release narrative, or supporting guide emphasizes long-term expansion that is inconsistent with the stabilization goal
- **THEN** that content is removed, reduced, archived, or reframed to match the repository's current intent

### Requirement: Documentation duplication is minimized
The repository SHALL minimize repeated explanations across README, Pages, governance docs, and supporting guides.

#### Scenario: The same guidance appears in multiple places
- **WHEN** duplicate guidance is found in active documents
- **THEN** one canonical version is retained and other documents link or summarize instead of repeating the full content
