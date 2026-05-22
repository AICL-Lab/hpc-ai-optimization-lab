#include <gtest/gtest.h>

#include "../test_utils.hpp"
#include "common/tensor.cuh"
#include "cuda13/fp8_gemm.cuh"

TEST(FP8GemmTest, BasicTest) {
    int M = 32, N = 32, K = 32;
    auto A_float = hpc::test::random_vector<float>(M * K, -1.0f, 1.0f);
    auto B_float = hpc::test::random_vector<float>(K * N, -1.0f, 1.0f);

    // Convert to half precision
    std::vector<__half> A_half(M * K);
    std::vector<__half> B_half(K * N);
    for (size_t i = 0; i < A_float.size(); ++i) {
        A_half[i] = __float2half(A_float[i]);
    }
    for (size_t i = 0; i < B_float.size(); ++i) {
        B_half[i] = __float2half(B_float[i]);
    }

    hpc::Tensor<__half> d_A(M * K);
    hpc::Tensor<__half> d_B(K * N);
    hpc::Tensor<__half> d_C(M * N);

    d_A.copy_from_host(A_half);
    d_B.copy_from_host(B_half);
    d_C.zero();

    hpc::cuda13::fp8_gemm(d_A.data(), d_B.data(), d_C.data(), M, N, K,
                          hpc::cuda13::FP8GEMMConfig{});
    cudaDeviceSynchronize();

    auto C = d_C.to_host();
    EXPECT_EQ(C.size(), M * N);
}

TEST(FP8GemmTest, RejectsUnsupportedNativeTileShape) {
    int M = 32, N = 32, K = 32;
    hpc::Tensor<__half> d_A(M * K);
    hpc::Tensor<__half> d_B(K * N);
    hpc::Tensor<__half> d_C(M * N);
    d_A.zero();
    d_B.zero();
    d_C.zero();

    hpc::cuda13::FP8GEMMConfig config{};
    config.tile_m = 64;

    EXPECT_THROW(hpc::cuda13::fp8_gemm(d_A.data(), d_B.data(), d_C.data(), M, N, K, config),
                 std::invalid_argument);
}
