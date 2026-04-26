/**
 * @file gemm_benchmark.cu
 * @brief Comprehensive GEMM benchmark comparing all optimization levels
 *
 * This example demonstrates:
 * - 7-step GEMM optimization progression
 * - Performance comparison with cuBLAS
 * - Roofline analysis
 */

#include <cuda_runtime.h>

#include <cstdio>
#include <random>
#include <string>
#include <vector>

#include <cublas_v2.h>

#include "common/cuda_check.cuh"
#include "common/tensor.cuh"
#include "gemm/gemm.cuh"

using hpc::gemm::gemm;
using hpc::gemm::GemmOpt;

#define CUBLAS_CHECK(call)                                                  \
    do {                                                                    \
        cublasStatus_t status = call;                                       \
        if (status != CUBLAS_STATUS_SUCCESS) {                              \
            fprintf(stderr, "cuBLAS error at %s:%d\n", __FILE__, __LINE__); \
            exit(EXIT_FAILURE);                                             \
        }                                                                   \
    } while (0)

struct BenchmarkResult {
    std::string name;
    float time_ms;
    float tflops;
    float efficiency;  // vs cuBLAS
};

/**
 * @brief Calculate TFLOPS for GEMM
 */
float calculate_tflops(int M, int N, int K, float time_ms) {
    // GEMM: 2*M*N*K FLOPs (multiply-add)
    double flops = 2.0 * M * N * K;
    return (flops / (time_ms * 1e-3)) / 1e12;
}

/**
 * @brief Benchmark a GEMM kernel
 */
template <typename GemmFunc>
float benchmark_gemm(GemmFunc gemm_func, int iterations = 100) {
    cudaEvent_t start, stop;
    cudaEventCreate(&start);
    cudaEventCreate(&stop);

    // Warm-up
    gemm_func();
    cudaDeviceSynchronize();

    // Timed runs
    cudaEventRecord(start);
    for (int i = 0; i < iterations; i++) {
        gemm_func();
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
 * @brief Run all GEMM benchmarks for a given size
 */
void run_benchmarks(int M, int N, int K) {
    printf("\n========================================\n");
    printf("GEMM Benchmark: M=%d, N=%d, K=%d\n", M, N, K);
    printf("========================================\n");

    const size_t size_A = M * K * sizeof(float);
    const size_t size_B = K * N * sizeof(float);
    const size_t size_C = M * N * sizeof(float);

    // Allocate host memory and initialize with random values
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    std::vector<float> h_A(M * K);
    std::vector<float> h_B(K * N);
    for (auto& val : h_A)
        val = dist(rng);
    for (auto& val : h_B)
        val = dist(rng);

    // Allocate device memory (RAII)
    hpc::Tensor<float> d_A(M * K);
    hpc::Tensor<float> d_B(K * N);
    hpc::Tensor<float> d_C(M * N);

    d_A.copy_from_host(h_A);
    d_B.copy_from_host(h_B);

    std::vector<BenchmarkResult> results;

    // cuBLAS baseline
    cublasHandle_t handle;
    CUBLAS_CHECK(cublasCreate(&handle));
    float alpha = 1.0f, beta = 0.0f;

    auto cublas_gemm = [&]() {
        cublasSgemm(handle, CUBLAS_OP_N, CUBLAS_OP_N, N, M, K, &alpha, d_B.data(), N, d_A.data(), K,
                    &beta, d_C.data(), N);
    };
    float cublas_time = benchmark_gemm(cublas_gemm);
    float cublas_tflops = calculate_tflops(M, N, K, cublas_time);
    results.push_back({"cuBLAS", cublas_time, cublas_tflops, 100.0f});

    // Step 1: Naive GEMM
    auto naive_gemm = [&]() {
        gemm<float, GemmOpt::Naive>(d_A.data(), d_B.data(), d_C.data(), M, N, K, 1.0f, 0.0f);
    };
    float naive_time = benchmark_gemm(naive_gemm);
    float naive_tflops = calculate_tflops(M, N, K, naive_time);
    results.push_back(
        {"Step 1: Naive", naive_time, naive_tflops, (cublas_time / naive_time) * 100.0f});

    // Step 2: SharedMem Tiling
    auto tiled_gemm = [&]() {
        gemm<float, GemmOpt::SharedMemTiling>(d_A.data(), d_B.data(), d_C.data(), M, N, K, 1.0f,
                                              0.0f);
    };
    float tiled_time = benchmark_gemm(tiled_gemm);
    float tiled_tflops = calculate_tflops(M, N, K, tiled_time);
    results.push_back({"Step 2: SharedMem Tiling", tiled_time, tiled_tflops,
                       (cublas_time / tiled_time) * 100.0f});

    // Step 3: Double Buffer
    auto db_gemm = [&]() {
        gemm<float, GemmOpt::DoubleBuffer>(d_A.data(), d_B.data(), d_C.data(), M, N, K, 1.0f, 0.0f);
    };
    float db_time = benchmark_gemm(db_gemm);
    float db_tflops = calculate_tflops(M, N, K, db_time);
    results.push_back(
        {"Step 3: Double Buffer", db_time, db_tflops, (cublas_time / db_time) * 100.0f});

    // Step 4: Register Tiling
    auto reg_gemm = [&]() {
        gemm<float, GemmOpt::RegisterTiling>(d_A.data(), d_B.data(), d_C.data(), M, N, K, 1.0f,
                                             0.0f);
    };
    float reg_time = benchmark_gemm(reg_gemm);
    float reg_tflops = calculate_tflops(M, N, K, reg_time);
    results.push_back(
        {"Step 4: Register Tiling", reg_time, reg_tflops, (cublas_time / reg_time) * 100.0f});

    // Step 5: Software Pipeline
    auto pipe_gemm = [&]() {
        gemm<float, GemmOpt::SoftwarePipeline>(d_A.data(), d_B.data(), d_C.data(), M, N, K, 1.0f,
                                               0.0f);
    };
    float pipe_time = benchmark_gemm(pipe_gemm);
    float pipe_tflops = calculate_tflops(M, N, K, pipe_time);
    results.push_back(
        {"Step 5: Software Pipeline", pipe_time, pipe_tflops, (cublas_time / pipe_time) * 100.0f});

    // Print results
    printf("\n%-25s %12s %12s %12s\n", "Implementation", "Time (ms)", "TFLOPS", "vs cuBLAS");
    printf("%-25s %12s %12s %12s\n", "---------------", "---------", "------", "---------");
    for (const auto& r : results) {
        printf("%-25s %12.4f %12.2f %11.1f%%\n", r.name.c_str(), r.time_ms, r.tflops, r.efficiency);
    }

    // Cleanup (Tensors cleaned up by RAII)
    CUBLAS_CHECK(cublasDestroy(handle));
}

int main(int argc, char** argv) {
    // Print device info
    cudaDeviceProp prop;
    cudaGetDeviceProperties(&prop, 0);
    printf("Device: %s\n", prop.name);
    printf("Compute Capability: %d.%d\n", prop.major, prop.minor);
    printf("Peak FP32 TFLOPS: %.2f\n", (prop.clockRate * 1e-6f) * prop.multiProcessorCount *
                                           (prop.major >= 8 ? 128 : 64) * 2 / 1000.0f);

    // Run benchmarks for different sizes
    std::vector<std::tuple<int, int, int>> sizes = {
        {1024, 1024, 1024},
        {2048, 2048, 2048},
        {4096, 4096, 4096},
        {8192, 8192, 8192},
    };

    for (const auto& [M, N, K] : sizes) {
        run_benchmarks(M, N, K);
    }

    printf("\n=== Benchmark Complete ===\n");
    return 0;
}
