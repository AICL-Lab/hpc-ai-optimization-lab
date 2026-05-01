---
Status: Active
Phase: Implemented
Last Updated: 2026-05-01
---

## ADDED Requirements

### Requirement: Shipped Python API is the documented Python API
The repository SHALL document only the Python symbols, module structure, argument patterns, and output behavior that are actually exported by the shipped `hpc_ai_opt` module for this change.

#### Scenario: README Python example is reviewed
- **WHEN** a README or active guide includes a Python usage example for `hpc_ai_opt`
- **THEN** the example uses exported module paths and call signatures that exist in the built bindings for this change

#### Scenario: Example code is maintained
- **WHEN** an example under `examples/python/` is updated or added
- **THEN** it demonstrates only the shipped Python surface and avoids placeholder convenience APIs that are not implemented

### Requirement: Python-facing support claims distinguish shipped access paths
The repository SHALL distinguish between kernel families that exist in C++ and kernel families that are actually accessible through the shipped Python surface for this change.

#### Scenario: Support matrix or capability summary is updated
- **WHEN** documentation describes module support or feature availability
- **THEN** it makes clear whether the claim applies to the core C++/CUDA implementation, the Python bindings, or both

#### Scenario: A module is not exported to Python
- **WHEN** a kernel family exists in `src/` but is not bound in `python/bindings/`
- **THEN** active user-facing Python documentation does not imply that the module is available through `hpc_ai_opt`
