#!/bin/bash
# GitHub Release 创建脚本 / GitHub Release Creation Script

set -e

REPO="LessUp/hpc-ai-optimization-lab"
TAG="v0.3.0"
TITLE="Release v0.3.0 - Documentation Internationalization & Professional Refactor"

# 检查 GITHUB_TOKEN
if [ -z "$GITHUB_TOKEN" ]; then
    echo "❌ 错误: GITHUB_TOKEN 环境变量未设置"
    echo "Error: GITHUB_TOKEN environment variable not set"
    echo ""
    echo "请设置您的 GitHub Personal Access Token:"
    echo "Please set your GitHub Personal Access Token:"
    echo "  export GITHUB_TOKEN='ghp_xxxxxxxxxxxx'"
    echo ""
    echo "获取 token: https://github.com/settings/tokens"
    exit 1
fi

echo "🚀 创建 GitHub Release $TAG..."
echo "Creating GitHub Release $TAG..."

# 使用 GitHub CLI 创建
echo "使用 gh CLI..."
gh release create "$TAG" \
    --title "$TITLE" \
    --notes-file docs/RELEASE_NOTES_v0.3.0.md \
    --target master

echo ""
echo "✅ Release 创建成功！/ Release created successfully!"
echo "查看: https://github.com/$REPO/releases/tag/$TAG"
