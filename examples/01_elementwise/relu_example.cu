/**
 * @file relu_example.cu
 * @brief Example demonstrating optimized ReLU activation
 *
 * This example shows:
 * - Basic ReLU implementation
 * - Vectorized ReLU using float4
 * - Performance comparison
 */

#include <cuda_runtime.h>
#include <cstdio>
#include <random>
#include <vector>

#include "01_elementwise/relu.cuh"
#include "common/cuda_check.cuh"
#include "common/tensor.cuh"

/**
 * @brief Naive ReLU kernel for comparison
 */
__global__ void relu_naive(const float* __restrict__ input,
                           float* __restrict__ output,
                           int n) {
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    if (idx < n) {
        output[idx] = fmaxf(input[idx], 0.0f);
    }
}

/**
 * @brief Benchmark a kernel
 */
template <typename KernelFunc>
float benchmark_kernel(KernelFunc kernel, int iterations = 100) {
    cudaEvent_t start, stop;
    cudaEventCreate(&start);
    cudaEventCreate(&stop);

    // Warm-up
    kernel();
    cudaDeviceSynchronize();

    // Timed runs
    cudaEventRecord(start);
    for (int i = 0; i < iterations; i++) {
        kernel();
    }
    cudaEventRecord(stop);
    cudaEventSynchronize(stop);

    float ms;
    cudaEventElapsedTime(&ms, start, stop);

    cudaEventDestroy(start);
    cudaEventDestroy(stop);

    return ms / iterations;
}

/**
 * @brief Verify results match
 */
bool verify_results(const float* a, const float* b, int n, float tolerance = 1e-5f) {
    for (int i = 0; i < n; i++) {
        if (fabsf(a[i] - b[i]) > tolerance) {
            printf("Mismatch at index %d: %f vs %f\n", i, a[i], b[i]);
            return false;
        }
    }
    return true;
}

int main(int argc, char** argv) {
    // Configuration
    const int N = 1 << 24;  // 16M elements
    const size_t bytes = N * sizeof(float);

    printf("=== ReLU Example ===\n");
    printf("Array size: %d elements (%.2f MB)\n", N, bytes / (1024.0f * 1024.0f));

    // Initialize input with random values
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(-1.0f, 1.0f);
    std::vector<float> h_input(N);
    for (int i = 0; i < N; i++) {
        h_input[i] = dist(rng);
    }

    // Allocate device memory (RAII)
    hpc::Tensor<float> d_input(N);
    hpc::Tensor<float> d_output_naive(N);
    hpc::Tensor<float> d_output_optimized(N);

    // Copy input to device
    d_input.copy_from_host(h_input);

    // Kernel configurations
    const int block_size = 256;
    const int grid_size_naive = (N + block_size - 1) / block_size;
    const int grid_size_vec4 = (N / 4 + block_size - 1) / block_size;

    printf("\nKernel configurations:\n");
    printf("  Naive:     grid=%d, block=%d\n", grid_size_naive, block_size);
    printf("  Vectorized: grid=%d, block=%d\n", grid_size_vec4, block_size);

    // Benchmark naive kernel
    auto naive_kernel = [&]() {
        relu_naive<<<grid_size_naive, block_size>>>(d_input.data(), d_output_naive.data(), N);
    };
    float naive_time = benchmark_kernel(naive_kernel);

    // Benchmark optimized kernel (GridStride level)
    auto optimized_kernel = [&]() {
        hpc::elementwise::relu<float>(d_input.data(), d_output_optimized.data(), N);
    };
    float optimized_time = benchmark_kernel(optimized_kernel);

    // Copy results back
    auto h_output_naive = d_output_naive.to_host();
    auto h_output_optimized = d_output_optimized.to_host();

    // Verify correctness
    printf("\nVerifying results...\n");
    bool correct = verify_results(h_output_naive.data(), h_output_optimized.data(), N);
    printf("Results %s\n", correct ? "MATCH ✓" : "MISMATCH ✗");

    // Performance results
    printf("\n=== Performance Results ===\n");
    printf("Naive ReLU:     %.4f ms\n", naive_time);
    printf("Optimized ReLU: %.4f ms\n", optimized_time);
    printf("Speedup:        %.2fx\n", naive_time / optimized_time);

    // Calculate bandwidth
    float naive_bandwidth = (2.0f * bytes) / (naive_time * 1e6);  // GB/s
    float optimized_bandwidth = (2.0f * bytes) / (optimized_time * 1e6);
    printf("\nMemory Bandwidth:\n");
    printf("  Naive:     %.2f GB/s\n", naive_bandwidth);
    printf("  Optimized: %.2f GB/s\n", optimized_bandwidth);

    // Cleanup handled by RAII (hpc::Tensor destructors)
    return correct ? 0 : 1;
}
