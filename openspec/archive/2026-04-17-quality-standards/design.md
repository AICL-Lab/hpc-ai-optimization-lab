# RFC 0002: Project Quality & Open Source Standards

> **Status**: Implemented
> **Last Updated**: 2026-04-17
> **Type**: Architecture Design Document
> **Related Spec**: [Product Spec](../product/quality-standards.md)

## 1. Architecture Overview

```
+-----------------------------------------------------------------------------+
|                        GitHub Repository                                     |
|  +-------------+  +-------------+  +-----------------------------------+  |
|  | Source Code |  |   Tests     |  |      Documentation               |  |
|  |   src/      |  |   tests/    |  |  docs/, README.md                |  |
|  +-------------+  +-------------+  +-----------------------------------+  |
|  +-------------+  +-------------+  +-----------------------------------+  |
|  |  Examples   |  |   Python    |  |      Specs (/specs)              |  |
|  | examples/   |  |   python/   |  |  requirements, design, tasks     |  |
|  +-------------+  +-------------+  +-----------------------------------+  |
+-----------------------------------------------------------------------------+
           |                   |                    |
           v                   v                    v
+-----------------------------------------------------------------------------+
|                        GitHub Actions                                        |
|  +---------------------------------------------------------------------+    |
|  |                        CI Workflow                                   |    |
|  |  +-----------+  +-----------+  +-----------------------------------+ |    |
|  |  |  Format   |  |Consistency|  |    Documentation Build            | |    |
|  |  |  Check    |  |  Check    |  |    (Doxygen + Sphinx)             | |    |
|  |  +-----------+  +-----------+  +-----------------------------------+ |    |
|  +---------------------------------------------------------------------+    |
|  +---------------------------------------------------------------------+    |
|  |                      Pages Workflow                                      |    |
|  |  +-----------+  +-----------+  +-----------------------------------+ |    |
|  |  |  Build    |  |  Combine  |  |    Deploy to                      | |    |
|  |  |  Docs     |  |  Output   |  |    GitHub Pages                   | |    |
|  |  +-----------+  +-----------+  +-----------------------------------+ |    |
|  +---------------------------------------------------------------------+    |
+-----------------------------------------------------------------------------+
                                    |
                                    v
+-----------------------------------------------------------------------------+
|                        GitHub Pages                                          |
|  +-------------+  +-------------+  +-----------------------------------+  |
|  |   Portal    |  |   C++ API   |  |      Python API                   |  |
|  | index.html  |  |  (Doxygen)  |  |      (Sphinx)                     |  |
|  | guides.html |  |             |  |                                   |  |
|  +-------------+  +-------------+  +-----------------------------------+  |
+-----------------------------------------------------------------------------+
```

---

## 2. Component Design

### 2.1 Open Source Standard Files

| File | Content Template | Purpose |
|------|------------------|---------|
| `LICENSE` | Apache 2.0 full text | Legal licensing |
| `CONTRIBUTING.md` | Development workflow, code style, PR process | Contributor guide |
| `CODE_OF_CONDUCT.md` | Contributor Covenant | Community standards |
| `CHANGELOG.md` | Keep a Changelog format | Version tracking |
| `.github/ISSUE_TEMPLATE/bug_report.md` | Environment info, reproduction steps | Bug reports |
| `.github/ISSUE_TEMPLATE/feature_request.md` | Requirements description, use cases | Feature requests |
| `.github/PULL_REQUEST_TEMPLATE.md` | Change type, tests, checklist | PR standardization |

### 2.2 CI Workflow Design

```yaml
# .github/workflows/ci.yml structure
name: CI

on: [push, pull_request]

jobs:
  format-check:
    - clang-format (C++/CUDA)
    - ruff (Python)

  consistency-check:
    - Verify example files exist
    - Verify module naming consistency
    - Verify CI documentation

  docs:
    - Doxygen build
    - Sphinx build

  ci-success:
    - Aggregate all job statuses
```

### 2.3 Pages Workflow Design

```yaml
# .github/workflows/pages.yml structure
name: Documentation

on: [push, workflow_dispatch]

jobs:
  build-docs:
    - Install Doxygen
    - Install Sphinx + dependencies
    - Build Doxygen (C++ API)
    - Build Sphinx (Python API)
    - Merge output to _site/
    - Upload artifact

  deploy:
    - Deploy to GitHub Pages
```

### 2.4 Documentation Portal Design

```
docs/
+-- index.html          # Documentation portal homepage
|   +-- Hero section    # Project introduction
|   +-- Module cards    # 7 feature modules
|   +-- Learning path   # Learning roadmap
|   +-- Quick ref       # Quick reference table
|
+-- guides.html         # Optimization topic center
|   +-- Memory opt      # Memory optimization
|   +-- Reduction opt   # Reduction optimization
|   +-- GEMM opt        # GEMM optimization
|   +-- FlashAttention  # Attention optimization
|   +-- CUDA 13         # New features
|
+-- assets/
|   +-- site.css        # Modern dark theme
|
+-- api/                # Doxygen output
|   +-- html/           # C++ API documentation
|
+-- python/
    +-- _build/html/    # Sphinx output
```

---

## 3. Error Handling Design

### 3.1 CI Error Handling

```python
# Consistency check example
def check_file_exists(path, description):
    if not Path(path).exists():
        print(f"Error: {description} not found at {path}")
        sys.exit(1)
    print(f"OK: {description} found")

def check_content(path, expected, description):
    content = Path(path).read_text()
    if expected not in content:
        print(f"Error: Expected '{expected}' in {path}")
        sys.exit(1)
    print(f"OK: {description} verified")
```

### 3.2 Pages Workflow Error Handling

```bash
# Documentation build with fallback
if [ -f docs/Doxyfile ]; then
    cd docs && doxygen Doxyfile || {
        echo "Warning: Doxygen build failed"
        # Continue without Doxygen output
    }
else
    echo "Info: Doxyfile not found, skipping"
fi

# Create fallback index
if [ ! -f docs/_site/index.html ]; then
    cat > docs/_site/index.html << 'EOF'
    <!DOCTYPE html>
    <html><body>
    <h1>Documentation</h1>
    <p>Build in progress...</p>
    </body></html>
    EOF
fi
```

---

## 4. Data Flow

### 4.1 PR Validation Flow

```
PR Created
    |
    +--------------------------------------+
    |                                      |
    v                                      v
Format Check                          Consistency Check
    |                                      |
    +--> Fail --> Block Merge              +--> Fail --> Block Merge
    |                                      |
    v Pass                                 v Pass
    |                                      |
    +------------------+-------------------+
                       |
                       v
                  Docs Build
                       |
                       +--> Fail --> Block Merge
                       |
                       v Pass
                       |
                       v
                 Allow Merge
```

### 4.2 Documentation Deployment Flow

```
Push to main
    |
    v
Build Doxygen --------> docs/api/html/
    |
    v
Build Sphinx ---------> docs/python/_build/html/
    |
    v
Combine --------------> docs/_site/
    |                        |
    |                        +-- index.html
    |                        +-- guides.html
    |                        +-- cpp-api/
    |                        +-- python-api/
    |
    v
Deploy ---------------> GitHub Pages
```

---

## 5. Configuration Details

### 5.1 clang-format

```yaml
BasedOnStyle: Google
IndentWidth: 4
ColumnLimit: 100
Language: Cpp
Standard: c++20
AlignAfterOpenBracket: Align
AllowShortFunctionsOnASingleLine: Empty
```

### 5.2 pre-commit

```yaml
repos:
  - repo: https://github.com/pre-commit/pre-commit-hooks
    rev: v4.5.0
    hooks:
      - id: trailing-whitespace
      - id: end-of-file-fixer
      - id: check-yaml

  - repo: https://github.com/pre-commit/mirrors-clang-format
    rev: v17.0.6
    hooks:
      - id: clang-format
        types_or: [c++, cuda]
```

### 5.3 Sphinx

```python
# docs/python/conf.py
project = 'HPC-AI-Optimization-Lab'
extensions = [
    'sphinx.ext.autodoc',
    'sphinx.ext.napoleon',
    'breathe',
    'myst_parser',
    'sphinx_copybutton',
]
html_theme = 'sphinx_rtd_theme'
```

---

## 6. Verification Matrix

| Check | Trigger | Pass Criteria |
|-------|---------|---------------|
| clang-format | PR | All files formatted |
| ruff | PR | No lint errors |
| File existence | PR | Required files present |
| Naming consistency | PR | Module names aligned |
| Doxygen build | PR, Push | HTML generated |
| Sphinx build | PR, Push | HTML generated |
| Pages deploy | Push to main | Site accessible |
