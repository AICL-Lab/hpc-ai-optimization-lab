# Design Document: 项目质量完善

## 1. Architecture Overview

```
┌─────────────────────────────────────────────────────────────────────┐
│                        GitHub Repository                             │
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────────────────────┐  │
│  │ Source Code │  │   Tests     │  │      Documentation          │  │
│  │   src/      │  │   tests/    │  │  docs/, README.md           │  │
│  └─────────────┘  └─────────────┘  └─────────────────────────────┘  │
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────────────────────┐  │
│  │  Examples   │  │   Python    │  │      .kiro Specs            │  │
│  │ examples/   │  │   python/   │  │  requirements, design, tasks │  │
│  └─────────────┘  └─────────────┘  └─────────────────────────────┘  │
└─────────────────────────────────────────────────────────────────────┘
           │                   │                    │
           ▼                   ▼                    ▼
┌─────────────────────────────────────────────────────────────────────┐
│                        GitHub Actions                                │
│  ┌─────────────────────────────────────────────────────────────┐    │
│  │                        CI Workflow                            │    │
│  │  ┌───────────┐  ┌───────────┐  ┌───────────────────────────┐ │    │
│  │  │  Format   │  │Consistency│  │    Documentation Build    │ │    │
│  │  │  Check    │  │  Check    │  │    (Doxygen + Sphinx)     │ │    │
│  │  └───────────┘  └───────────┘  └───────────────────────────┘ │    │
│  └─────────────────────────────────────────────────────────────┘    │
│  ┌─────────────────────────────────────────────────────────────┐    │
│  │                      Pages Workflow                           │    │
│  │  ┌───────────┐  ┌───────────┐  ┌───────────────────────────┐ │    │
│  │  │  Build    │  │  Combine  │  │    Deploy to              │ │    │
│  │  │  Docs     │  │  Output   │  │    GitHub Pages           │ │    │
│  │  └───────────┘  └───────────┘  └───────────────────────────┘ │    │
│  └─────────────────────────────────────────────────────────────┘    │
└─────────────────────────────────────────────────────────────────────┘
                                    │
                                    ▼
┌─────────────────────────────────────────────────────────────────────┐
│                        GitHub Pages                                  │
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────────────────────┐  │
│  │   Portal    │  │   C++ API   │  │      Python API             │  │
│  │ index.html  │  │  (Doxygen)  │  │      (Sphinx)               │  │
│  │ guides.html │  │             │  │                             │  │
│  └─────────────┘  └─────────────┘  └─────────────────────────────┘  │
└─────────────────────────────────────────────────────────────────────┘
```

---

## 2. Component Design

### 2.1 开源标准文件

| 文件 | 内容模板 | 用途 |
|------|----------|------|
| `LICENSE` | Apache 2.0 全文 | 法律许可 |
| `CONTRIBUTING.md` | 开发流程、代码风格、PR 步骤 | 贡献者指南 |
| `CODE_OF_CONDUCT.md` | Contributor Covenant | 社区规范 |
| `CHANGELOG.md` | Keep a Changelog 格式 | 版本追踪 |
| `.github/ISSUE_TEMPLATE/bug_report.md` | 环境信息、复现步骤 | Bug 报告 |
| `.github/ISSUE_TEMPLATE/feature_request.md` | 需求描述、用例 | 功能请求 |
| `.github/PULL_REQUEST_TEMPLATE.md` | 变更类型、测试、检查清单 | PR 规范 |

### 2.2 CI Workflow Design

```yaml
# .github/workflows/ci.yml 结构
name: CI

on: [push, pull_request]

jobs:
  format-check:
    - clang-format (C++/CUDA)
    - ruff (Python)

  consistency-check:
    - 验证示例文件存在
    - 验证模块命名一致
    - 验证 CI 范围文档

  docs:
    - Doxygen 构建
    - Sphinx 构建

  ci-success:
    - 汇总所有 job 状态
```

### 2.3 Pages Workflow Design

```yaml
# .github/workflows/pages.yml 结构
name: Documentation

on: [push, workflow_dispatch]

jobs:
  build-docs:
    - 安装 Doxygen
    - 安装 Sphinx + 依赖
    - 构建 Doxygen (C++ API)
    - 构建 Sphinx (Python API)
    - 合并文档到 _site/
    - 上传 artifact

  deploy:
    - 部署到 GitHub Pages
```

### 2.4 文档门户设计

```
docs/
├── index.html          # 文档门户首页
│   ├── Hero section    # 项目介绍
│   ├── Module cards    # 7 个功能模块
│   ├── Learning path   # 学习路线
│   └── Quick ref       # 速查表
│
├── guides.html         # 优化专题中心
│   ├── Memory opt      # 访存优化
│   ├── Reduction opt   # 归约优化
│   ├── GEMM opt        # GEMM 优化
│   ├── FlashAttention  # 注意力优化
│   └── CUDA 13         # 新特性
│
├── assets/
│   └── site.css        # 现代深色主题
│
├── api/                # Doxygen 输出
│   └── html/           # C++ API 文档
│
└── python/
    └── _build/html/    # Sphinx 输出
```

---

## 3. Error Handling Design

### 3.1 CI Error Handling

```python
# 一致性检查示例
def check_file_exists(path, description):
    if not Path(path).exists():
        print(f"Error: {description} not found at {path}")
        sys.exit(1)
    print(f"✓ {description} found")

def check_content(path, expected, description):
    content = Path(path).read_text()
    if expected not in content:
        print(f"Error: Expected '{expected}' in {path}")
        sys.exit(1)
    print(f"✓ {description} verified")
```

### 3.2 Pages Workflow Error Handling

```bash
# 文档构建带 fallback
if [ -f docs/Doxyfile ]; then
    cd docs && doxygen Doxyfile || {
        echo "Warning: Doxygen build failed"
        # Continue without Doxygen output
    }
else
    echo "Info: Doxyfile not found, skipping"
fi

# 创建 fallback index
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
    │
    ├──────────────────────────────────────┐
    │                                      │
    ▼                                      ▼
Format Check                          Consistency Check
    │                                      │
    ├─► Fail ──► Block Merge              ├─► Fail ──► Block Merge
    │                                      │
    ▼ Pass                                 ▼ Pass
    │                                      │
    └──────────────────┬───────────────────┘
                       │
                       ▼
                  Docs Build
                       │
                       ├─► Fail ──► Block Merge
                       │
                       ▼ Pass
                       │
                       ▼
                 Allow Merge
```

### 4.2 Documentation Deployment Flow

```
Push to main
    │
    ▼
Build Doxygen ──────► docs/api/html/
    │
    ▼
Build Sphinx ───────► docs/python/_build/html/
    │
    ▼
Combine ────────────► docs/_site/
    │                      │
    │                      ├── index.html
    │                      ├── guides.html
    │                      ├── cpp-api/
    │                      └── python-api/
    │
    ▼
Deploy ─────────────► GitHub Pages
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

---

## 7. Status

✅ All design components implemented and verified
