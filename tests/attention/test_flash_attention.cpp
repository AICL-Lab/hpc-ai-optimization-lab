#include <algorithm>
#include <cmath>
#include <limits>
#include <stdexcept>
#include <vector>

#include <gtest/gtest.h>

#include "../reference/reference_kernels.hpp"
#include "../test_utils.hpp"
#include "attention/flash_attention.cuh"
#include "common/tensor.cuh"

namespace {

using hpc::reference::cpu_flash_attention;

void expect_attention_matches_reference(bool causal) {
    constexpr int batch = 1;
    constexpr int heads = 2;
    constexpr int seq = 16;
    constexpr int dim = 64;
    const int total = batch * heads * seq * dim;

    const auto q = hpc::test::random_vector<float>(total, -1.0f, 1.0f);
    const auto k = hpc::test::random_vector<float>(total, -1.0f, 1.0f);
    const auto v = hpc::test::random_vector<float>(total, -1.0f, 1.0f);

    hpc::Tensor<float> d_q(total);
    hpc::Tensor<float> d_k(total);
    hpc::Tensor<float> d_v(total);
    hpc::Tensor<float> d_o(total);

    d_q.copy_from_host(q);
    d_k.copy_from_host(k);
    d_v.copy_from_host(v);

    const hpc::attention::FlashAttnConfig config{
        batch, heads, seq, dim, 1.0f / std::sqrt(static_cast<float>(dim)), causal,
    };

    hpc::attention::flash_attention_forward<float>(d_q.data(), d_k.data(), d_v.data(), d_o.data(),
                                                   config);
    cudaDeviceSynchronize();

    const auto expected = cpu_flash_attention(q, k, v, config);
    const auto actual = d_o.to_host();

    ASSERT_EQ(actual.size(), expected.size());
    for (size_t i = 0; i < actual.size(); ++i) {
        ASSERT_TRUE(std::isfinite(actual[i]));
        EXPECT_NEAR(actual[i], expected[i], 5e-4f);
    }
}

}  // namespace

TEST(FlashAttentionTest, MatchesReferenceWithoutCausalMask) {
    expect_attention_matches_reference(false);
}

TEST(FlashAttentionTest, MatchesReferenceWithCausalMask) {
    expect_attention_matches_reference(true);
}

TEST(FlashAttentionTest, RejectsUnsupportedHeadDim) {
    constexpr int batch = 1;
    constexpr int heads = 1;
    constexpr int seq = 8;
    constexpr int dim = 32;
    const int total = batch * heads * seq * dim;

    hpc::Tensor<float> d_q(total);
    hpc::Tensor<float> d_k(total);
    hpc::Tensor<float> d_v(total);
    hpc::Tensor<float> d_o(total);
    d_q.zero();
    d_k.zero();
    d_v.zero();
    d_o.zero();

    const hpc::attention::FlashAttnConfig config{
        batch, heads, seq, dim, 1.0f / std::sqrt(static_cast<float>(dim)), false,
    };

    EXPECT_THROW(hpc::attention::flash_attention_forward<float>(d_q.data(), d_k.data(), d_v.data(),
                                                                d_o.data(), config),
                 std::invalid_argument);
}
