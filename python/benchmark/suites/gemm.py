import torch
from typing import Any, Callable, List, Optional, Tuple

from ..benchmark import BenchmarkResult, benchmark_kernel


def run_gemm_suite(
    sizes: List[int],
    gemm_runner: Callable[..., None],
    baseline_runner: Callable[..., None],
    benchmark_fn: Callable[..., BenchmarkResult] = benchmark_kernel,
    tensor_factory: Optional[Callable[[int], Tuple[Any, Any, Any]]] = None,
) -> List[BenchmarkResult]:
    """Run GEMM benchmarks for each requested square size."""
    if tensor_factory is None:
        raise ValueError("tensor_factory is required for GEMM benchmark execution")

    results: List[BenchmarkResult] = []
    for size in sizes:
        a, b, c = tensor_factory(size)
        flops = 2 * size * size * size
        bytes_accessed = 3 * size * size * 4
        results.append(
            benchmark_fn(
                f"gemm-{size}",
                gemm_runner,
                baseline_runner,
                a,
                b,
                c,
                size,
                size,
                size,
                1.0,
                0.0,
                flops=flops,
                bytes_accessed=bytes_accessed,
            )
        )

    return results


def build_gemm_suite_runner(
    hpc_module: Any,
    benchmark_fn: Callable[..., BenchmarkResult] = benchmark_kernel,
    tensor_factory: Optional[Callable[[int], Tuple[Any, Any, Any]]] = None,
) -> Callable[[List[int]], List[BenchmarkResult]]:
    """Build a GEMM suite runner from the shipped Python binding surface."""
    if not hasattr(hpc_module, "gemm"):
        raise ValueError("hpc module does not expose a gemm submodule")
    if not hasattr(hpc_module.gemm, "matmul"):
        raise ValueError("hpc module does not expose gemm.matmul")
    if not hasattr(hpc_module.gemm, "matmul_cutlass"):
        raise ValueError("hpc module does not expose gemm.matmul_cutlass")

    return lambda sizes: run_gemm_suite(
        sizes,
        hpc_module.gemm.matmul,
        hpc_module.gemm.matmul_cutlass,
        benchmark_fn=benchmark_fn,
        tensor_factory=tensor_factory,
    )


def default_gemm_tensor_factory(size: int) -> Tuple[torch.Tensor, torch.Tensor, torch.Tensor]:
    """Allocate the default square GEMM tensors on CUDA."""
    if not torch.cuda.is_available():
        raise RuntimeError("CUDA is required for the default GEMM benchmark runner")

    a = torch.randn(size, size, device="cuda", dtype=torch.float32)
    b = torch.randn(size, size, device="cuda", dtype=torch.float32)
    c = torch.zeros(size, size, device="cuda", dtype=torch.float32)
    return a, b, c
