#!/usr/bin/env python3
"""Basic usage example for the current hpc_ai_opt bindings."""

from __future__ import annotations

import sys

import torch

try:
    import hpc_ai_opt as opt
except ImportError as exc:
    raise SystemExit(
        "Error: hpc_ai_opt module not found.\n"
        "Build the bindings first:\n"
        "  cmake -S . -B build -DBUILD_PYTHON_BINDINGS=ON\n"
        "  cmake --build build\n"
        '  export PYTHONPATH="$(pwd)/build/python:${PYTHONPATH}"\n'
    ) from exc


def require_cuda() -> torch.device:
    if not torch.cuda.is_available():
        raise SystemExit(
            "Error: this example requires a CUDA-enabled PyTorch installation."
        )
    return torch.device("cuda")


def example_elementwise(device: torch.device) -> None:
    print("\n=== Elementwise ===")
    x = torch.randn(256, 256, device=device, dtype=torch.float32)

    relu_out = torch.empty_like(x)
    opt.elementwise.relu(x, relu_out)
    torch.testing.assert_close(relu_out, torch.relu(x))
    print("ReLU passed")

    sigmoid_out = torch.empty_like(x)
    opt.elementwise.sigmoid(x, sigmoid_out)
    torch.testing.assert_close(sigmoid_out, torch.sigmoid(x), rtol=1e-5, atol=1e-5)
    print("Sigmoid passed")

    transpose_out = torch.empty((x.shape[1], x.shape[0]), device=device, dtype=x.dtype)
    opt.elementwise.transpose(x, transpose_out, x.shape[0], x.shape[1])
    torch.testing.assert_close(transpose_out, x.transpose(0, 1))
    print("Transpose passed")


def example_reduction(device: torch.device) -> None:
    print("\n=== Reduction ===")
    x = torch.randn(64, 128, device=device, dtype=torch.float32)
    softmax_out = torch.empty_like(x)
    opt.reduction.softmax(x, softmax_out, x.shape[0], x.shape[1])
    torch.testing.assert_close(
        softmax_out, torch.softmax(x, dim=-1), rtol=1e-5, atol=1e-5
    )
    print("Softmax passed")


def example_gemm(device: torch.device) -> None:
    print("\n=== GEMM ===")
    m, n, k = 128, 96, 64
    a = torch.randn(m, k, device=device, dtype=torch.float32)
    b = torch.randn(k, n, device=device, dtype=torch.float32)
    c = torch.zeros(m, n, device=device, dtype=torch.float32)

    opt.gemm.matmul(a, b, c, m, n, k, 1.0, 0.0)
    torch.testing.assert_close(c, a @ b, rtol=1e-4, atol=1e-4)
    print("Matmul passed")


def main() -> None:
    device = require_cuda()
    print("Running hpc_ai_opt examples on", torch.cuda.get_device_name(device))
    print("Current shipped modules: elementwise, reduction, gemm")
    example_elementwise(device)
    example_reduction(device)
    example_gemm(device)
    print("\nAll current binding examples passed.")


if __name__ == "__main__":
    try:
        main()
    except RuntimeError as exc:
        print(f"Runtime error: {exc}", file=sys.stderr)
        raise
