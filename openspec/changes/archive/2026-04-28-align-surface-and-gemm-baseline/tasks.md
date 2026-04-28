## 1. OpenSpec contract and surface alignment

- [x] 1.1 Audit the current README, Python example, bindings, and benchmark entrypoint to capture the exact public/runtime mismatches this change must eliminate
- [x] 1.2 Rewrite the runtime-facing README/support-language sections so Python usage, support claims, and benchmark descriptions match the bounded shipped surface
- [x] 1.3 Update `examples/python/basic_usage.py` and any adjacent user-facing entrypoints so they demonstrate only the supported bindings and benchmark contract for this phase

## 2. CUTLASS GEMM baseline

- [x] 2.1 Implement `gemm_cutlass()` for the supported initial GEMM configuration using the existing CUTLASS dependency and explicit failure behavior for unsupported cases
- [x] 2.2 Add focused tests or comparison coverage that validate the CUTLASS GEMM path against the existing GEMM contract for the supported envelope

## 3. Real benchmark execution

- [x] 3.1 Replace the placeholder benchmark CLI flow with real GEMM benchmark execution that calls the actual repository kernel and CUTLASS baseline paths
- [x] 3.2 Wire benchmark result generation so JSON/HTML/chart outputs are emitted only from real measured result sets and unsupported suites fail clearly

## 4. Validation and close-out

- [x] 4.1 Add or update checks around bindings/examples/docs consistency so future public-surface drift is easier to catch
- [x] 4.2 Re-run the repository build/test path needed for the changed runtime/docs surfaces and update the OpenSpec artifacts if implementation discoveries require scope corrections
