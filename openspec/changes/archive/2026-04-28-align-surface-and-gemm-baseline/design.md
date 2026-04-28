## Context

The repository already contains the ingredients for a credible runtime-facing CUDA lab, but its public surface currently overstates what is actually wired together. The README shows a simplified Python API that the nanobind module does not export, the Python benchmark framework contains device modeling and report generation but still stops at placeholder execution, and the existing GEMM API advertises a CUTLASS comparison hook without implementing it. This creates a trust gap in exactly the area the repository now wants to harden: what users can actually run, compare, and learn from.

This change is intentionally bounded. It targets the first runtime-facing slice that can be made truthful and demonstrable without expanding into a broad new product surface: shipped Python API alignment, executable benchmark entrypoints, and a GEMM baseline path using the CUTLASS dependency that is already fetched by CMake.

## Goals / Non-Goals

**Goals:**
- Make the documented Python surface match the nanobind module and shipped examples.
- Turn the benchmark entrypoint into a real, bounded execution path for GEMM-first comparisons.
- Implement a CUTLASS-backed GEMM baseline/fallback path that can be consumed by benchmarks and comparison tests.
- Add enough validation that docs, bindings, examples, and benchmark behavior stay aligned.
- Express the above as OpenSpec capabilities so later runtime work can build on a stable contract.

**Non-Goals:**
- Expanding Python bindings to every existing CUDA module in this change.
- Implementing Hopper/TMA/FP8 systemization, paged attention, or broader inference-runtime features.
- Rewriting the project into a packaging-first Python distribution in this phase.
- Replacing the educational GEMM path with CUTLASS; CUTLASS is introduced as baseline/fallback, not as the primary pedagogical implementation.

## Decisions

### 1. Split the change into a truthfulness surface and a benchmark baseline within one bounded runtime phase

**Decision:** This change will combine the public-surface corrections (README, examples, support descriptions, benchmark entrypoint) with the minimum kernel-side work needed to make those corrections meaningful: a real GEMM benchmark path and a CUTLASS baseline.

**Rationale:** Doing only documentation cleanup would leave the benchmark surface weak, while doing only CUTLASS work would still leave the repository telling an inaccurate story. Bundling these two pieces creates one coherent "trust restoration + baseline establishment" change.

**Alternatives considered:**
- **Docs-only first:** lower implementation cost, but still leaves the benchmark path unconvincing.
- **CUTLASS-only first:** improves internals, but does not fix the current public mismatch.

### 2. Treat the current nanobind module as the source of truth, then expand only what this change explicitly adds

**Decision:** The shipped Python surface will be defined by the actual exported module structure (`elementwise`, `reduction`, `gemm`) plus any deliberate additions made in this bounded change. Documentation and examples will be rewritten to that surface rather than to aspirational convenience APIs.

**Rationale:** The current trust issue comes from docs getting ahead of implementation. Reversing that relationship makes later growth safer.

**Alternatives considered:**
- **Add convenience APIs just to preserve the README shape:** rejected because it would expand scope unnecessarily.
- **Keep aspirational examples with disclaimers:** rejected because they still blur what is shipped.

### 3. Implement CUTLASS as a comparison path, not the default GEMM teaching path

**Decision:** `gemm_cutlass()` will be implemented as a dedicated baseline/fallback path that sits alongside the existing staged GEMM implementations, and benchmarks will compare against it explicitly.

**Rationale:** The repository's educational value depends on keeping the 7-step GEMM progression visible. CUTLASS should strengthen credibility and benchmarking, not collapse the learning narrative into a single black-box kernel.

**Alternatives considered:**
- **Replace advanced GEMM paths with CUTLASS:** rejected because it weakens the lab's pedagogical structure.
- **Leave CUTLASS unused:** rejected because the dependency already exists and the missing baseline is currently a notable gap.

### 4. Make the benchmark CLI execute real workloads with explicit bounded scope

**Decision:** The benchmark framework entrypoint will execute real kernels for the suites supported by this phase, starting with GEMM and any already-wired adjacent comparisons, and it will emit JSON/HTML/chart outputs only from real result sets.

**Rationale:** The repository already has result formatting, roofline analysis, and report generation. The missing value is not more framework code but actual kernel invocation and a truthful execution contract.

**Alternatives considered:**
- **Keep the framework as a library and defer the CLI:** rejected because the user-facing gap is specifically at the entrypoint.
- **Support every suite immediately:** rejected because it would turn a bounded change into a broad expansion.

### 5. Use tests and examples to lock the public/runtime contract together

**Decision:** This change will add or adjust tests and examples so README snippets, Python examples, benchmark execution, and the CUTLASS path are all validated against the same bounded contract.

**Rationale:** Public-surface drift is likely to recur unless it is enforced through executable checks close to the changed code.

**Alternatives considered:**
- **Rely on manual review only:** rejected because the mismatch already survived without machine-enforced checks.

## Risks / Trade-offs

- **[Risk] CUTLASS integration adds complexity or architecture-specific constraints** → Mitigation: keep the initial path limited to bounded GEMM shapes/types already supported by the repository and document any constraints explicitly.
- **[Risk] Benchmark CLI grows into a large product surface** → Mitigation: scope this phase to real execution for a bounded suite rather than to universal benchmark coverage.
- **[Risk] README truthfulness work looks like a feature reduction** → Mitigation: pair the doc alignment with an actually improved benchmark and baseline story so the change clearly increases credibility.
- **[Risk] Python ergonomics remain limited after alignment** → Mitigation: document that broader Python-surface expansion is a later sequential change and avoid overloading this one.
- **[Risk] Existing tests do not fully cover public-surface drift** → Mitigation: add focused tests or executable example coverage around the changed API and benchmark paths.

## Migration Plan

1. Define the runtime-facing OpenSpec requirements for the Python surface and GEMM baseline/benchmark contract.
2. Align README, support descriptions, and Python examples with the bounded shipped surface.
3. Implement `gemm_cutlass()` and any supporting benchmark wiring needed for real comparisons.
4. Turn the benchmark entrypoint into a real execution path for the supported suite(s).
5. Add or update validation around bindings, examples, and benchmark outputs.
6. Leave later Python-surface expansion and advanced Hopper/attention work for subsequent changes.

Rollback is straightforward at repository level: the public-surface edits and benchmark wiring can be reverted together if the CUTLASS or benchmark path proves too unstable, restoring the previous bounded behavior while keeping the OpenSpec history.

## Open Questions

- Whether the first benchmarked GEMM baseline should be limited to `float` or immediately include the repository's current half/int8 support envelope.
- Whether benchmark result artifacts should be committed only as documentation/examples or remain purely generated output.
