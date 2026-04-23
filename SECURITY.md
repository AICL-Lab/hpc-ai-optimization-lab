# Security Policy

## Supported scope

Security-minded fixes are handled against the latest repository state on the default branch. Older snapshots and historical tags should be treated as unsupported unless explicitly noted otherwise.

This project contains educational and optimization-focused CUDA code. Review, validate, and constrain any deployment context yourself before using it in sensitive environments.

## Reporting a vulnerability

Please **do not** open a public issue for a suspected security problem.

When possible:

1. Use GitHub's private vulnerability reporting / security advisory flow for this repository.
2. If that is not available, contact the maintainer through a non-public repository-owner channel before disclosing details publicly.
3. Include:
   - affected file, module, or workflow
   - reproduction steps
   - impact assessment
   - any mitigation or patch suggestion you already have

## Response expectations

Reports are handled on a best-effort basis. The project will prioritize issues that affect:

- repository credential or token safety
- unsafe workflow permissions or automation behavior
- build or release-chain integrity
- clear security-impacting bugs in the active codebase

## General guidance

- Review workflow permissions and GitHub settings before enabling new automation.
- Prefer least-privilege credentials and short-lived tokens.
- Keep local dependencies and toolchains current when building or testing the project.
