# HPC-AI-Optimization-Lab Examples

This directory documents only the examples that currently exist in the repository.

## Available examples

| Path | Description |
|------|-------------|
| `01_elementwise/relu_example.cu` | Minimal CUDA example for the elementwise ReLU kernel |
| `03_gemm/gemm_benchmark.cu` | GEMM benchmark comparing project kernels with cuBLAS |
| `python/basic_usage.py` | Python binding example for the current `hpc_ai_opt` module |

## Build CUDA examples

```bash
cmake -S . -B build -DBUILD_EXAMPLES=ON
cmake --build build --target relu_example gemm_benchmark

./build/examples/relu_example
./build/examples/gemm_benchmark
```

## Run the Python example

```bash
cmake -S . -B build -DBUILD_PYTHON_BINDINGS=ON
cmake --build build
export PYTHONPATH="$(pwd)/build/python:${PYTHONPATH}"
python examples/python/basic_usage.py
```

## Notes

- The Python example uses PyTorch CUDA tensors and expects a CUDA-capable environment.
- Examples not listed above are not currently shipped in this repository.
