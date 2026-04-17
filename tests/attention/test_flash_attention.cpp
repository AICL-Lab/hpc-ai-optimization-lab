#include <gtest/gtest.h>
#include <algorithm>
#include <cmath>
#include <limits>
#include <stdexcept>
#include <vector>

#include "attention/flash_attention.cuh"
#include "common/tensor.cuh"
#include "../test_utils.hpp"

namespace {

std::vector<float> cpu_flash_attention(const std::vector<float>& q,
                                       const std::vector<float>& k,
                                       const std::vector<float>& v,
                                       const hpc::attention::FlashAttnConfig& config) {
    const int head_dim = config.head_dim;
    const int seq_len = config.seq_len;
    const int head_stride = seq_len * head_dim;
    const int batch_head_stride = config.num_heads * head_stride;

    std::vector<float> out(q.size(), 0.0f);

    for (int batch = 0; batch < config.batch_size; ++batch) {
        for (int head = 0; head < config.num_heads; ++head) {
            const int base = batch * batch_head_stride + head * head_stride;
            for (int q_idx = 0; q_idx < seq_len; ++q_idx) {
                std::vector<float> scores(seq_len, -std::numeric_limits<float>::infinity());
                float max_score = -std::numeric_limits<float>::infinity();

                for (int kv_idx = 0; kv_idx < seq_len; ++kv_idx) {
                    if (config.causal && kv_idx > q_idx) {
                        continue;
                    }
                    float score = 0.0f;
                    for (int d = 0; d < head_dim; ++d) {
                        score += q[base + q_idx * head_dim + d] *
                                 k[base + kv_idx * head_dim + d];
                    }
                    score *= config.scale;
                    scores[kv_idx] = score;
                    max_score = std::max(max_score, score);
                }

                float denom = 0.0f;
                for (int kv_idx = 0; kv_idx < seq_len; ++kv_idx) {
                    if (scores[kv_idx] == -std::numeric_limits<float>::infinity()) {
                        continue;
                    }
                    denom += std::exp(scores[kv_idx] - max_score);
                }

                for (int d = 0; d < head_dim; ++d) {
                    float acc = 0.0f;
                    for (int kv_idx = 0; kv_idx < seq_len; ++kv_idx) {
                        if (scores[kv_idx] == -std::numeric_limits<float>::infinity()) {
                            continue;
                        }
                        const float weight = std::exp(scores[kv_idx] - max_score) / denom;
                        acc += weight * v[base + kv_idx * head_dim + d];
                    }
                    out[base + q_idx * head_dim + d] = acc;
                }
            }
        }
    }

    return out;
}

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
        batch, heads, seq, dim,
        1.0f / std::sqrt(static_cast<float>(dim)),
        causal,
    };

    hpc::attention::flash_attention_forward<float>(
        d_q.data(), d_k.data(), d_v.data(), d_o.data(), config);
    cudaDeviceSynchronize();

    const auto expected = cpu_flash_attention(q, k, v, config);
    const auto actual = d_o.to_host();

    ASSERT_EQ(actual.size(), expected.size());
    for (size_t i = 0; i < actual.size(); ++i) {
        ASSERT_TRUE(std::isfinite(actual[i]));
        EXPECT_NEAR(actual[i], expected[i], 5e-4f);
    }
}

} // namespace

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
        batch, heads, seq, dim,
        1.0f / std::sqrt(static_cast<float>(dim)),
        false,
    };

    EXPECT_THROW(
        hpc::attention::flash_attention_forward<float>(
            d_q.data(), d_k.data(), d_v.data(), d_o.data(), config),
        std::invalid_argument);
}
