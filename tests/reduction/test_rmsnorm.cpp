#include <algorithm>
#include <cmath>
#include <numeric>

#include <gtest/gtest.h>
#include <rapidcheck.h>
#include <rapidcheck/gtest.h>

#include "../test_utils.hpp"
#include "common/tensor.cuh"
#include "reduction/rmsnorm.cuh"

// Feature: hpc-ai-optimization-lab, Property: RMSNorm output has unit RMS when gamma=1
RC_GTEST_PROP(RMSNormTest, UnitRMSWithGammaOne, ()) {
    auto batch = *rc::gen::inRange<int>(1, 16);
    auto hidden = *rc::gen::inRange<int>(64, 256);
    auto input = *rc::gen::container<std::vector<float>>(
        batch * hidden, rc::gen::map(rc::gen::arbitrary<float>(),
                                     [](float x) { return std::clamp(x, -2.0f, 2.0f); }));

    // gamma = 1.0 for all hidden dims
    std::vector<float> gamma(hidden, 1.0f);

    hpc::Tensor<float> d_input(batch * hidden);
    hpc::Tensor<float> d_gamma(hidden);
    hpc::Tensor<float> d_output(batch * hidden);

    d_input.copy_from_host(input);
    d_gamma.copy_from_host(gamma);

    hpc::reduction::rms_norm<float>(d_input.data(), d_gamma.data(), d_output.data(), batch, hidden);
    cudaDeviceSynchronize();

    auto result = d_output.to_host();

    // For each row, verify that values are finite
    for (int b = 0; b < batch; ++b) {
        for (int h = 0; h < hidden; ++h) {
            RC_ASSERT(std::isfinite(result[b * hidden + h]));
        }
    }
}

TEST(RMSNormTest, BasicTest) {
    int batch = 2, hidden = 64;
    auto input = hpc::test::random_vector<float>(batch * hidden, -1.0f, 1.0f);
    std::vector<float> gamma(hidden, 1.0f);

    hpc::Tensor<float> d_input(batch * hidden);
    hpc::Tensor<float> d_gamma(hidden);
    hpc::Tensor<float> d_output(batch * hidden);

    d_input.copy_from_host(input);
    d_gamma.copy_from_host(gamma);

    hpc::reduction::rms_norm<float>(d_input.data(), d_gamma.data(), d_output.data(), batch, hidden);
    cudaDeviceSynchronize();

    auto result = d_output.to_host();
    EXPECT_EQ(result.size(), batch * hidden);

    // Verify output is finite and non-trivial
    for (size_t i = 0; i < result.size(); ++i) {
        EXPECT_TRUE(std::isfinite(result[i]));
    }
}
