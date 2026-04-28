## Why

The repository currently presents a more complete Python and benchmarking surface than it actually ships: the README shows a Python API shape that does not match the current nanobind module, support claims blur the line between C++-only and Python-accessible functionality, and the benchmark framework entrypoint is still placeholder-driven. This change is needed now to restore trust in the public surface while establishing a concrete GEMM baseline path that can support credible performance work.

## What Changes

- Align README Python examples, support claims, and user-facing docs with the bindings and kernels that are actually shipped.
- Define and implement the supported Python kernel surface for the first bounded runtime-facing phase, including how examples and docs must reflect that surface.
- Turn the benchmark framework from a placeholder entrypoint into a real executable path for bounded kernel suites, starting with GEMM and adjacent comparison hooks.
- Add a CUTLASS-backed GEMM baseline/fallback path and make it available to benchmark and comparison workflows.
- Update tests and validation surfaces so documentation, bindings, and benchmark behavior remain consistent after the change.

## Capabilities

### New Capabilities
- `python-kernel-surface`: Define the shipped Python-facing kernel API, its documented examples, and the consistency rules between bindings, examples, and public claims.
- `gemm-baseline-benchmarking`: Define a real GEMM comparison and benchmarking path, including a CUTLASS baseline/fallback and reproducible benchmark outputs.

### Modified Capabilities
- `documentation-rationalization`: Tighten active documentation requirements so runtime-facing README examples and support descriptions reflect the actually shipped API surface rather than aspirational or placeholder behavior.
- `stabilization-sweep`: Capture runtime-surface trust issues discovered in docs, bindings, and benchmark entrypoints as first-class stabilization work rather than leaving them as informal follow-up.

## Impact

- Affected areas include `README.md`, Python examples and bindings under `python/` and `examples/python/`, GEMM implementation files under `src/gemm/`, benchmark tooling, tests, and the new OpenSpec capability set for runtime-facing behavior.
- This change introduces a CUTLASS-backed baseline path on top of the existing dependency already fetched through CMake, but it does not broaden the project into a new runtime family beyond the bounded GEMM and public-surface scope.
- Public-facing behavior will become stricter: documentation and examples may be reduced or rewritten where the current repository overstates shipped functionality.
