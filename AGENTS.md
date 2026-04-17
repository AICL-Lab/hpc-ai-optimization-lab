# AGENTS.md - AI Agent Workflow Instructions

> **Purpose**: Configuration for AI coding assistants (Qwen Code, Claude, Cursor, etc.)
> **Last Updated**: 2026-04-17

---

## Project Philosophy: Spec-Driven Development (SDD)

This project strictly follows the **Spec-Driven Development (SDD)** paradigm. All code implementations must use the specification documents in the `/specs` directory as the single source of truth.

---

## Directory Context

| Directory | Purpose |
|-----------|---------|
| `/specs/product/` | Product feature definitions and acceptance criteria |
| `/specs/rfc/` | Technical design documents (Request for Comments) |
| `/specs/api/` | API interface definitions (e.g., OpenAPI.yaml) |
| `/specs/db/` | Database/schema model definitions |
| `/specs/testing/` | Test specifications and task tracking |
| `/docs/` | User-facing documentation and tutorials |

---

## AI Agent Workflow Instructions

When you (the AI) are asked to develop a new feature, modify an existing one, or fix a bug, **you must strictly follow this workflow. Do not skip any steps**:

### Step 1: Review Specifications

- First, read the relevant documents in the `/specs` directory (product specs, RFCs, API definitions).
- If the user's instruction conflicts with the existing spec, **stop immediately** and point out the conflict. Ask the user whether they want to update the spec first.
- If no spec exists for the requested feature, propose creating one before writing any code.

### Step 2: Spec-First Update

- If this is a new feature, or if it changes existing interfaces/database structures, **you must first propose modifying or creating the relevant spec documents** (e.g., updating a product spec or creating a new RFC).
- Wait for user confirmation of the spec changes before proceeding to the code-writing phase.

### Step 3: Code Implementation

- When writing code, **follow the specs 100%** (including variable names, API paths, data types, status codes, etc.).
- Do not add features that are not defined in the specs (no gold-plating).
- Follow the design patterns and architectural guidelines documented in `/specs/rfc/`.

### Step 4: Test Against Spec

- Write unit tests and integration tests based on the acceptance criteria described in the specs.
- Ensure test cases cover all boundary conditions described in the product specs.
- Reference the `/specs/testing/` directory for existing test strategies and task tracking.

---

## Code Generation Rules

1. **API Changes**: Any externally exposed API changes **must** be synchronized with the corresponding spec documents (e.g., update `/specs/api/openapi.yaml` if it exists, or update the product spec).
2. **Architecture Decisions**: If uncertain about technical details, consult the `/specs/rfc/` directory for architectural conventions. Do not invent design patterns on your own.
3. **Documentation Sync**: When modifying code, check if related documentation in `/docs/` needs to be updated.
4. **No Gold-Plating**: Implement only what is specified. Do not add extra features beyond the spec without explicit user approval.

---

## Project Tech Stack

| Component | Version/Technology |
|-----------|-------------------|
| Language | C++20, CUDA 12.4+ |
| Build System | CMake 3.24+ |
| Python Bindings | nanobind |
| Testing | GoogleTest, RapidCheck |
| Formatting | clang-format (Google style) |
| Static Analysis | clang-tidy |
| Documentation | Doxygen, Sphinx, VitePress |

---

## Key Architectural Patterns

### Module Interface Pattern

Each kernel module follows a unified interface:

```cpp
namespace hpc::module {

template <typename T, OptLevel Level = OptLevel::Advanced>
    requires std::is_same_v<T, float> || std::is_same_v<T, __half>
void kernel_name(const T* input, T* output, size_t n,
                 cudaStream_t stream = nullptr);

} // namespace hpc::module
```

### Resource Management

- Use RAII for GPU memory management (see `Tensor<T>` class)
- Delete copy constructors, use move semantics
- Use `CUDA_CHECK` macros for error handling

### Testing Strategy

- Unit tests with GoogleTest for all public APIs
- Property-based tests with RapidCheck for edge cases
- Tolerance guidelines: FP32 (1e-5), FP16 (1e-3)

---

## Contributing References

- See [CONTRIBUTING.md](CONTRIBUTING.md) for human contributor guidelines
- See [specs/product/](specs/product/) for product requirements
- See [specs/rfc/](specs/rfc/) for technical design documents
- See [specs/testing/](specs/testing/) for task tracking and test strategies
