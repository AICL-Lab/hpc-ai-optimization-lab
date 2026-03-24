#include <gtest/gtest.h>
#include <rapidcheck.h>
#include <rapidcheck/gtest.h>
#include <stdexcept>
#include "03_gemm/gemm.cuh"
#include "common/tensor.cuh"
#include "../test_utils.hpp"

// CPU reference GEMM
void cpu_gemm(const float* A, const float* B, float* C,
              int M, int N, int K, float alpha, float beta) {
    for (int i = 0; i < M; ++i) {
        for (int j = 0; j < N; ++j) {
            float sum = 0.0f;
            for (int k = 0; k < K; ++k) {
                sum += A[i * K + k] * B[k * N + j];
            }
            C[i * N + j] = alpha * sum + beta * C[i * N + j];
        }
    }
}

// Feature: hpc-ai-optimization-lab, Property 8: GEMM Correctness
RC_GTEST_PROP(GemmTest, Correctness, ()) {
    auto M = *rc::gen::inRange<int>(1, 64);
    auto N = *rc::gen::inRange<int>(1, 64);
    auto K = *rc::gen::inRange<int>(1, 64);
    
    auto A = *rc::gen::container<std::vector<float>>(M * K,
        rc::gen::map(rc::gen::arbitrary<float>(), [](float x) {
            return std::clamp(x, -1.0f, 1.0f);
        }));
    auto B = *rc::gen::container<std::vector<float>>(K * N,
        rc::gen::map(rc::gen::arbitrary<float>(), [](float x) {
            return std::clamp(x, -1.0f, 1.0f);
        }));
    
    std::vector<float> C_cpu(M * N, 0.0f);
    std::vector<float> C_gpu(M * N, 0.0f);
    
    // CPU reference
    cpu_gemm(A.data(), B.data(), C_cpu.data(), M, N, K, 1.0f, 0.0f);
    
    // GPU implementation
    hpc::Tensor<float> d_A(M * K);
    hpc::Tensor<float> d_B(K * N);
    hpc::Tensor<float> d_C(M * N);
    d_A.copy_from_host(A);
    d_B.copy_from_host(B);
    d_C.copy_from_host(C_gpu);
    
    hpc::gemm::gemm<float, hpc::gemm::GemmOpt::SharedMemTiling>(
        d_A.data(), d_B.data(), d_C.data(), M, N, K);
    cudaDeviceSynchronize();
    
    C_gpu = d_C.to_host();
    
    for (int i = 0; i < M * N; ++i) {
        RC_ASSERT(hpc::test::almost_equal(C_gpu[i], C_cpu[i], 1e-3f, 1e-4f));
    }
}

TEST(GemmTest, BaselineImplementationsMatchReference) {
    constexpr int M = 32;
    constexpr int N = 32;
    constexpr int K = 32;
    const auto A = hpc::test::random_vector<float>(M * K, -1.0f, 1.0f);
    const auto B = hpc::test::random_vector<float>(K * N, -1.0f, 1.0f);
    std::vector<float> C_cpu(M * N, 0.0f);

    cpu_gemm(A.data(), B.data(), C_cpu.data(), M, N, K, 1.0f, 0.0f);

    hpc::Tensor<float> d_A(M * K);
    hpc::Tensor<float> d_B(K * N);
    hpc::Tensor<float> d_C(M * N);
    d_A.copy_from_host(A);
    d_B.copy_from_host(B);

    d_C.zero();
    hpc::gemm::gemm<float, hpc::gemm::GemmOpt::Naive>(
        d_A.data(), d_B.data(), d_C.data(), M, N, K);
    cudaDeviceSynchronize();
    const auto naive = d_C.to_host();
    EXPECT_TRUE(hpc::test::vectors_almost_equal(naive, C_cpu, 1e-3f, 1e-4f));

    d_C.zero();
    hpc::gemm::gemm<float, hpc::gemm::GemmOpt::SharedMemTiling>(
        d_A.data(), d_B.data(), d_C.data(), M, N, K);
    cudaDeviceSynchronize();
    const auto tiled = d_C.to_host();
    EXPECT_TRUE(hpc::test::vectors_almost_equal(tiled, C_cpu, 1e-3f, 1e-4f));
}

TEST(GemmTest, AdvancedFloatImplementationsMatchReference) {
    constexpr int M = 64;
    constexpr int N = 64;
    constexpr int K = 64;
    const auto A = hpc::test::random_vector<float>(M * K, -1.0f, 1.0f);
    const auto B = hpc::test::random_vector<float>(K * N, -1.0f, 1.0f);
    std::vector<float> C_cpu(M * N, 0.0f);

    cpu_gemm(A.data(), B.data(), C_cpu.data(), M, N, K, 1.0f, 0.0f);

    hpc::Tensor<float> d_A(M * K);
    hpc::Tensor<float> d_B(K * N);
    hpc::Tensor<float> d_C(M * N);
    d_A.copy_from_host(A);
    d_B.copy_from_host(B);

    d_C.zero();
    hpc::gemm::gemm<float, hpc::gemm::GemmOpt::DoubleBuffer>(
        d_A.data(), d_B.data(), d_C.data(), M, N, K);
    cudaDeviceSynchronize();
    const auto double_buffer = d_C.to_host();
    EXPECT_TRUE(hpc::test::vectors_almost_equal(double_buffer, C_cpu, 1e-3f, 1e-4f));

    d_C.zero();
    hpc::gemm::gemm<float, hpc::gemm::GemmOpt::RegisterTiling>(
        d_A.data(), d_B.data(), d_C.data(), M, N, K);
    cudaDeviceSynchronize();
    const auto register_tiling = d_C.to_host();
    EXPECT_TRUE(hpc::test::vectors_almost_equal(register_tiling, C_cpu, 1e-3f, 1e-4f));
}

TEST(GemmTest, SoftwarePipelineMatchesReference) {
    constexpr int M = 64;
    constexpr int N = 64;
    constexpr int K = 64;
    const auto A = hpc::test::random_vector<float>(M * K, -1.0f, 1.0f);
    const auto B = hpc::test::random_vector<float>(K * N, -1.0f, 1.0f);
    std::vector<float> C_cpu(M * N, 0.0f);

    cpu_gemm(A.data(), B.data(), C_cpu.data(), M, N, K, 1.0f, 0.0f);

    hpc::Tensor<float> d_A(M * K);
    hpc::Tensor<float> d_B(K * N);
    hpc::Tensor<float> d_C(M * N);
    d_A.copy_from_host(A);
    d_B.copy_from_host(B);
    d_C.zero();

    hpc::gemm::gemm<float, hpc::gemm::GemmOpt::SoftwarePipeline>(
        d_A.data(), d_B.data(), d_C.data(), M, N, K);
    cudaDeviceSynchronize();

    const auto C_gpu = d_C.to_host();
    EXPECT_TRUE(hpc::test::vectors_almost_equal(C_gpu, C_cpu, 1e-3f, 1e-4f));
}

TEST(GemmTest, Int8NaiveFallbackMatchesSharedMemPath) {
    constexpr int M = 32;
    constexpr int N = 32;
    constexpr int K = 32;
    std::vector<int8_t> A(M * K, 2);
    std::vector<int8_t> B(K * N, -1);

    hpc::Tensor<int8_t> d_A(M * K);
    hpc::Tensor<int8_t> d_B(K * N);
    hpc::Tensor<int8_t> d_naive(M * N);
    hpc::Tensor<int8_t> d_tiled(M * N);
    d_A.copy_from_host(A);
    d_B.copy_from_host(B);
    d_naive.zero();
    d_tiled.zero();

    hpc::gemm::gemm<int8_t, hpc::gemm::GemmOpt::Naive>(
        d_A.data(), d_B.data(), d_naive.data(), M, N, K);
    hpc::gemm::gemm<int8_t, hpc::gemm::GemmOpt::SharedMemTiling>(
        d_A.data(), d_B.data(), d_tiled.data(), M, N, K);
    cudaDeviceSynchronize();

    const auto naive = d_naive.to_host();
    const auto tiled = d_tiled.to_host();
    EXPECT_EQ(naive, tiled);
}

TEST(GemmTest, TensorCoreMmaMatchesWmmaFallback) {
    constexpr int M = 64;
    constexpr int N = 64;
    constexpr int K = 64;
    std::vector<__half> A(M * K, __float2half(0.5f));
    std::vector<__half> B(K * N, __float2half(0.25f));

    hpc::Tensor<__half> d_A(M * K);
    hpc::Tensor<__half> d_B(K * N);
    hpc::Tensor<__half> d_wmma(M * N);
    hpc::Tensor<__half> d_mma(M * N);
    d_A.copy_from_host(A);
    d_B.copy_from_host(B);
    d_wmma.zero();
    d_mma.zero();

    hpc::gemm::gemm<__half, hpc::gemm::GemmOpt::TensorCoreWMMA>(
        d_A.data(), d_B.data(), d_wmma.data(), M, N, K);
    hpc::gemm::gemm<__half, hpc::gemm::GemmOpt::TensorCoreMMA>(
        d_A.data(), d_B.data(), d_mma.data(), M, N, K);
    cudaDeviceSynchronize();

    const auto wmma = d_wmma.to_host();
    const auto mma = d_mma.to_host();
    ASSERT_EQ(wmma.size(), mma.size());
    for (size_t i = 0; i < wmma.size(); ++i) {
        EXPECT_NEAR(__half2float(wmma[i]), __half2float(mma[i]), 1e-2f);
    }
}

TEST(GemmTest, TensorCorePathsRejectInvalidShapes) {
    constexpr int M = 18;
    constexpr int N = 18;
    constexpr int K = 18;
    std::vector<__half> A(M * K, __float2half(1.0f));
    std::vector<__half> B(K * N, __float2half(1.0f));

    hpc::Tensor<__half> d_A(M * K);
    hpc::Tensor<__half> d_B(K * N);
    hpc::Tensor<__half> d_C(M * N);
    d_A.copy_from_host(A);
    d_B.copy_from_host(B);
    d_C.zero();

    EXPECT_THROW((hpc::gemm::gemm<__half, hpc::gemm::GemmOpt::TensorCoreWMMA>(
        d_A.data(), d_B.data(), d_C.data(), M, N, K)), std::invalid_argument);
}
