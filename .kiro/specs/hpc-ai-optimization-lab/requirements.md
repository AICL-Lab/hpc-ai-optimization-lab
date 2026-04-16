# Requirements Document: HPC-AI-Optimization-Lab

## Project Overview

**HPC-AI-Optimization-Lab** 是一个面向 AI 推理工作负载的教育性和生产级 CUDA 内核库。

### 核心目标

| 目标 | 描述 |
|------|------|
| 🎓 教学导向 | 每个模块展示从 Naive 到专家级的清晰优化演进路径 |
| 🔬 生产级质量 | 完整测试覆盖（GoogleTest 单元测试 + RapidCheck 属性测试） |
| 🚀 现代 C++ | RAII、Concepts、constexpr、requires 子句 |
| 🐍 Python 集成 | Nanobind 零拷贝绑定，PyTorch Tensor 支持 |

### 技术栈

| 组件 | 版本 | 用途 |
|------|------|------|
| CUDA | 12.4+ | GPU 编程 |
| C++ | 20 | 内核实现 |
| CMake | 3.24+ | 构建系统 |
| Python | 3.8+ | 绑定和测试 |

---

## Requirements by Module

### R1: 项目基础设施

| ID | 需求 | 验证标准 | 状态 |
|----|------|----------|------|
| R1.1 | CMake 3.24+ target-based 配置 | `cmake --version` ≥ 3.24 | ✅ |
| R1.2 | FetchContent 自动依赖管理 | 构建成功拉取 gtest, nanobind, fmt, cutlass | ✅ |
| R1.3 | GPU 架构自动检测 | `-gencode` 参数正确设置 | ✅ |
| R1.4 | Docker 开发环境 | CUDA 12.4+ 镜像构建成功 | ✅ |
| R1.5 | 跨平台支持 | Linux 首要，Windows/WSL2 兼容 | ✅ |

### R2: 通用工具库 (Common Library)

| ID | 需求 | 实现文件 | 状态 |
|----|------|----------|------|
| R2.1 | CUDA 错误检查宏 | `cuda_check.cuh` | ✅ |
| R2.2 | 高精度 GPU 计时器 | `timer.cuh` | ✅ |
| R2.3 | RAII Tensor 内存管理 | `tensor.cuh` | ✅ |
| R2.4 | Half/BF16 类型封装 | `types.cuh` | ✅ |
| R2.5 | 归约原语 | `reduce.cuh` | ✅ |
| R2.6 | Kernel 启动工具 | `launch.cuh` | ✅ |

### R3: Elementwise 模块

| ID | 需求 | 优化级别 | 性能目标 | 状态 |
|----|------|----------|----------|------|
| R3.1 | ReLU Kernel | Naive → Vectorized → GridStride | 内存带宽 80%+ | ✅ |
| R3.2 | Sigmoid Kernel | Naive → Vectorized → GridStride | 内存带宽 80%+ | ✅ |
| R3.3 | Vector Add Kernel | Naive → Vectorized → GridStride | 内存带宽 80%+ | ✅ |
| R3.4 | Transpose Kernel | Naive → SharedMem → SharedMemPadded | 无 Bank Conflict | ✅ |
| R3.5 | float4 向量化加载 | 所有 elementwise 内核 | 2-4x 加速 | ✅ |
| R3.6 | Grid Stride Loop | 处理任意大小输入 | 无越界 | ✅ |

### R4: Reduction 模块

| ID | 需求 | 优化级别 | 关键技术 | 状态 |
|----|------|----------|----------|------|
| R4.1 | Softmax | Naive → WarpShuffle → Online | 单次遍历 | ✅ |
| R4.2 | LayerNorm | Warp Shuffle + Block Reduce | Welford 算法 | ✅ |
| R4.3 | RMSNorm | 优化实现 | 数值稳定 | ✅ |
| R4.4 | Warp Shuffle 原语 | 替代 Shared Memory | __shfl_down_sync | ✅ |
| R4.5 | Online Softmax | 单次遍历算法 | max/sum 在线更新 | ✅ |

### R5: GEMM 模块

| ID | 需求 | 技术 | 性能目标 | 状态 |
|----|------|------|----------|------|
| R5.1 | Step 1: Naive | 全局内存 | ~0.5 TFLOPS | ✅ |
| R5.2 | Step 2: Shared Memory Tiling | 数据复用 | ~2.0 TFLOPS | ✅ |
| R5.3 | Step 3: Double Buffer | 延迟隐藏 | ~3.5 TFLOPS | ✅ |
| R5.4 | Step 4: Register Tiling | 减少 bank 冲突 | ~6.0 TFLOPS | ✅ |
| R5.5 | Step 5: Tensor Core WMMA | 硬件加速 | ~50 TFLOPS | ✅ |
| R5.6 | Step 6: Tensor Core MMA PTX | 细粒度控制 | ~60 TFLOPS | ✅ |
| R5.7 | Step 7: Software Pipeline | 多阶段重叠 | ~70 TFLOPS | ✅ |
| R5.8 | 多精度支持 | SGEMM, HGEMM, Int8-GEMM | - | ✅ |

### R6: Attention 模块

| ID | 需求 | 关键技术 | 限制 | 状态 |
|----|------|----------|------|------|
| R6.1 | FlashAttention Forward | IO-aware, Tiling | head_dim=64 | ✅ |
| R6.2 | Q, K, V Tiling | SRAM 驻留 | - | ✅ |
| R6.3 | Online Softmax | 避免 N×N 存储 | - | ✅ |
| R6.4 | RoPE | Complex rotation | - | ✅ |
| R6.5 | MoE TopK | 快速排序/筛选 | - | ✅ |

### R7: Convolution 模块

| ID | 需求 | 说明 | 状态 |
|----|------|------|------|
| R7.1 | Implicit GEMM 卷积 | 生产就绪 | ✅ |
| R7.2 | Winograd 卷积 | Fallback 到 GEMM | ✅ |
| R7.3 | 卷积参数支持 | stride, padding, dilation | ✅ |

### R8: Quantization 模块

| ID | 需求 | 说明 | 状态 |
|----|------|------|------|
| R8.1 | Weight-Only Dequant | 权重反量化 | ✅ |
| R8.2 | INT8 量化/反量化 | 逐行缩放 | ✅ |
| R8.3 | FP8 Scaling | 精度控制 | ✅ |

### R9: CUDA 13 特性模块 (实验性)

| ID | 需求 | 当前状态 | 说明 | 状态 |
|----|------|----------|------|------|
| R9.1 | TMA 异步搬运 | Fallback | async copy 代替 | ✅ |
| R9.2 | Thread Block Clusters | Fallback | block reduce 代替 | ✅ |
| R9.3 | FP8 GEMM | Demo | scaled FP16 演示 | ✅ |

### R10: Python 绑定

| ID | 需求 | 实现 | 状态 |
|----|------|------|------|
| R10.1 | Nanobind 零拷贝 | 直接指针传递 | ✅ |
| R10.2 | PyTorch Tensor 支持 | CUDA tensor | ✅ |
| R10.3 | 模块结构 | elementwise, reduction, gemm | ✅ |
| R10.4 | 参数验证 | 友好错误信息 | ✅ |

### R11: 测试

| ID | 需求 | 覆盖范围 | 状态 |
|----|------|----------|------|
| R11.1 | GoogleTest 单元测试 | 所有公开 API | ✅ |
| R11.2 | RapidCheck 属性测试 | 边界情况 | ✅ |
| R11.3 | 正确性验证 | vs CPU/cuBLAS 参考 | ✅ |

### R12: 文档

| ID | 需求 | 内容 | 状态 |
|----|------|------|------|
| R12.1 | README | 双语、学习路径 | ✅ |
| R12.2 | API Reference | 所有模块接口 | ✅ |
| R12.3 | Architecture | 设计文档 | ✅ |
| R12.4 | 优化专题 | 5 篇深度指南 | ✅ |

---

## Non-Functional Requirements

### NFR1: 性能

| 要求 | 指标 |
|------|------|
| GEMM Tensor Core | 达到理论峰值 70%+ |
| Elementwise | 达到内存带宽 80%+ |
| FlashAttention | head_dim=64 时正确 |

### NFR2: 可移植性

| 平台 | 状态 |
|------|------|
| Linux (Ubuntu 22.04+) | 首要支持 |
| Windows (WSL2) | 兼容 |
| GPU 架构 | SM 7.0+ (Volta+) |

### NFR3: 可维护性

| 要求 | 实现 |
|------|------|
| 代码风格 | .clang-format (Google style) |
| 静态分析 | .clang-tidy |
| 文档覆盖 | 所有公开 API |

---

## Constraints

1. **CUDA 版本**: 最低 12.4，推荐 13.x
2. **GPU 架构**: 最低 SM 7.0 (Volta)，Tensor Core 需要 SM 7.0+
3. **编译器**: GCC 11+ / Clang 14+ / MSVC 2022+
4. **Python**: 3.8+ (绑定时)

---

## Out of Scope

- 训练场景优化
- 多 GPU 支持
- 动态图模式
- 完整 Hopper 特性（实验性 fallback）
