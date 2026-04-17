# Examples

Standalone example programs demonstrating how to use HPC-AI-Optimization-Lab.

## Building Examples

Examples are built by default when `BUILD_EXAMPLES=ON`:

```bash
cmake .. -DCMAKE_BUILD_TYPE=Release -DBUILD_EXAMPLES=ON
cmake --build . -j$(nproc)
```

## Available Examples

| Example | Description | Module |
|---------|-------------|--------|
| [ReLU Example](#relu) | Basic elementwise ReLU kernel | Elementwise |
| [GEMM Benchmark](#gemm) | Matrix multiplication benchmark | GEMM |
| [Softmax Benchmark](#softmax) | Reduction kernel benchmark | Reduction |
| [Convolution Example](#conv) | Convolution kernel example | Convolution |
| [FlashAttention Example](#attention) | Attention kernel example | Attention |
| [Quantization Example](#quant) | INT8 quantization example | Quantization |
| [CUDA 13 Example](#cuda13) | Hopper features example | CUDA 13 |

## Running Examples

```bash
# Run ReLU example
./bin/examples/relu_example

# Run GEMM benchmark
./bin/examples/gemm_benchmark
```

## Example Details

### ReLU Example

Located in: `examples/elementwise/relu_example.cu`

```cpp
#include "elementwise/relu.cuh"

int main() {
    const size_t n = 1024;
    // Create input/output tensors
    float* d_input;
    float* d_output;
    
    cudaMalloc(&d_input, n * sizeof(float));
    cudaMalloc(&d_output, n * sizeof(float));
    
    // Launch kernel
    hpc::module::relu(d_input, d_output, n);
    
    cudaFree(d_input);
    cudaFree(d_output);
    return 0;
}
```

### GEMM Benchmark

Located in: `examples/gemm/gemm_benchmark.cu`

```bash
# Run with default parameters
./bin/examples/gemm_benchmark

# Custom matrix size
./bin/examples/gemm_benchmark --size 2048
```

## Next Steps

After running the examples:

1. Read the [Quick Start Guide](../en/guide/quick-start.md)
2. Explore the [API Reference](../en/API_REFERENCE.md)
3. Try optimizing the examples yourself!
