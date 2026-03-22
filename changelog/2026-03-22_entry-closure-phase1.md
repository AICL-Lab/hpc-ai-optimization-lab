# 入口闭环第一阶段完善记录

日期：2026-03-22

## 变更内容

### Python 绑定与命名统一
- 将 Python 扩展模块名从 `hpc_kernels` 统一为 `hpc_ai_opt`
- 同步更新 `python/bindings/bindings.cpp` 与 `python/CMakeLists.txt`
- 保留现有 `elementwise` / `reduction` / `gemm` 子模块 API 结构，不在本阶段引入新的高层兼容层

### 示例与文档入口收敛
- 重写 `examples/python/basic_usage.py`，使其只演示当前 bindings 真实暴露的 API
- 重写 `docs/python/index.rst`，移除不存在的 toctree 页面，改为真实单页入口
- 重写 `examples/README.md`，仅保留仓库当前真实存在的示例文件与运行方式
- 更新 `README.md` 与 `README.zh-CN.md` 中的 Python 导入、构建与示例说明，使其与 `hpc_ai_opt` 和当前 bindings 一致

### 构建入口与 CI 一致性
- 在根 `CMakeLists.txt` 中增加 `BUILD_EXAMPLES` 并接入 `examples/`
- 移除 `examples/CMakeLists.txt` 中重复定义的 `BUILD_EXAMPLES`，改由顶层统一控制
- 在 `.github/workflows/ci.yml` 中新增 `Entry Consistency Check`，校验：
  - 文档声明的示例文件真实存在
  - 关键入口文件中的 Python 模块命名一致
- 保持主线 CI 的 CPU-safe 边界，不引入依赖 GPU runner 的构建/测试矩阵

## 背景

本次工作聚焦“入口闭环”而非内核能力扩展。仓库此前的主要问题是：Python 模块命名、示例、文档、examples 构建入口和 CI 之间存在明显漂移，导致项目对外入口失真。第一阶段先解决这些问题，为后续测试增强和高级内核补强建立稳定基础。
