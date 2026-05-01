---
Status: Active
Phase: Implemented
Last Updated: 2026-05-01
---

## ADDED Requirements

### Requirement: GEMM baseline path is executable through a CUTLASS-backed implementation
The repository SHALL provide an executable GEMM baseline path backed by CUTLASS so benchmark and comparison workflows can run against a non-placeholder reference implementation.

#### Scenario: GEMM baseline is requested
- **WHEN** benchmark or comparison code requests the CUTLASS GEMM path
- **THEN** the repository executes a real CUTLASS-backed GEMM implementation for the supported type and shape envelope of this change

#### Scenario: CUTLASS baseline is unavailable for a requested case
- **WHEN** a benchmark or test requests an unsupported CUTLASS configuration
- **THEN** the repository fails clearly or scopes the request explicitly rather than silently pretending to run a reference baseline

### Requirement: Benchmark entrypoints execute real workloads for supported suites
The benchmark framework SHALL run real kernel workloads for the suites supported by this change and SHALL emit reports only from real measured result sets.

#### Scenario: Benchmark CLI is invoked for a supported suite
- **WHEN** a user runs the benchmark entrypoint for a supported benchmark suite in this change
- **THEN** the CLI executes the actual kernel and baseline functions, measures results, and can emit the configured JSON, HTML, or chart outputs from those measurements

#### Scenario: Benchmark CLI is invoked for an unsupported suite
- **WHEN** the benchmark entrypoint is asked to run a suite that this change does not wire to real execution
- **THEN** it reports the unsupported state explicitly instead of printing placeholder-success guidance

### Requirement: Public benchmark examples align with the executable benchmark contract
Examples and documentation SHALL describe the benchmark surface in terms of the workloads and outputs that the repository can actually execute in this change.

#### Scenario: Benchmark documentation is updated
- **WHEN** active docs or examples describe benchmark usage
- **THEN** they point to supported benchmark commands and output shapes that match the implemented CLI behavior
