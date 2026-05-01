---
Status: Active
Phase: Implemented
Last Updated: 2026-05-01
---

## ADDED Requirements

### Requirement: Runtime-facing examples reflect the shipped API surface
Active runtime-facing documentation SHALL show only examples, signatures, and support descriptions that match the repository's shipped bindings and executable benchmark surface for the current change.

#### Scenario: Python API documentation is reviewed
- **WHEN** README or active guides present Python usage or performance-reporting examples
- **THEN** the examples reflect the actual binding structure, callable signatures, and supported reporting outputs shipped by the repository

#### Scenario: Runtime support is described
- **WHEN** active documentation summarizes module or benchmark availability
- **THEN** it avoids aspirational claims and describes unsupported or C++-only surfaces in a way that does not mislead Python users
