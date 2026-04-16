# Workflow CPU-safe CI 调整

日期：2026-03-13

## 变更内容

- 移除主线 CI 中无效的 CUDA 构建矩阵与 `continue-on-error` 掩盖逻辑，保留 `Format Check`、`Build Documentation`、`CI Success`
- 让 `CI Success` 同时依赖格式与文档构建结果，避免只看单个 job 造成误判
- 保留 C++/Python 格式检查与 Sphinx/Doxygen 文档构建，维持 Hosted Runner 上可执行的验收范围

## 背景

该仓库原先的 GPU job 在 GitHub Hosted Runner 上没有真实验收意义，而文档与格式检查才是当前最稳定、最有价值的主线质量门。本次调整将 CI 收敛到这些 CPU-safe 检查。
