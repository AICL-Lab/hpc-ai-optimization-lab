# 文档清理与死链接修复

## 提案日期
2026-04-27

## 背景
项目文档存在多处死链接和冗余文件，VitePress 配置中 `ignoreDeadLinks: true` 掩盖了这些问题。CI 工作流中的 CPU-only build job 对 CUDA 项目无实际验证价值。

## 范围
- 修复 Release Notes 中的断裂链接
- 删除重复的 Release Notes 文件
- 移除 VitePress 死链接忽略配置
- 修复所有文档中的死链接
- 重构 CI 工作流
- 处理未实现的函数声明

## 目标
1. 文档构建无死链接警告
2. CI 工作流精简有效
3. 代码库更干净、更易维护
