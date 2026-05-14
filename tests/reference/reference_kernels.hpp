#pragma once

#include <algorithm>
#include <cmath>
#include <limits>
#include <numeric>
#include <vector>

#include "attention/flash_attention.cuh"
#include "convolution/conv_implicit_gemm.cuh"

namespace hpc::reference {

// ---------------------------------------------------------------------------
// GEMM: C = alpha * A * B + beta * C
// ---------------------------------------------------------------------------
inline void cpu_gemm(const float* A, const float* B, float* C, int M, int N, int K, float alpha,
                     float beta) {
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

// ---------------------------------------------------------------------------
// Elementwise: ReLU
// ---------------------------------------------------------------------------
inline std::vector<float> cpu_relu(const std::vector<float>& input) {
    std::vector<float> output(input.size());
    for (size_t i = 0; i < input.size(); ++i) {
        output[i] = std::max(0.0f, input[i]);
    }
    return output;
}

// ---------------------------------------------------------------------------
// Reduction: Softmax (two-pass stable)
// ---------------------------------------------------------------------------
inline std::vector<float> cpu_softmax(const std::vector<float>& input, int batch, int seq_len) {
    std::vector<float> output(input.size());
    for (int b = 0; b < batch; ++b) {
        float max_val = -std::numeric_limits<float>::infinity();
        for (int i = 0; i < seq_len; ++i) {
            max_val = std::max(max_val, input[b * seq_len + i]);
        }
        float sum = 0.0f;
        for (int i = 0; i < seq_len; ++i) {
            sum += std::exp(input[b * seq_len + i] - max_val);
        }
        for (int i = 0; i < seq_len; ++i) {
            output[b * seq_len + i] = std::exp(input[b * seq_len + i] - max_val) / sum;
        }
    }
    return output;
}

// ---------------------------------------------------------------------------
// Attention: FlashAttention forward (naive reference)
// ---------------------------------------------------------------------------
inline std::vector<float> cpu_flash_attention(const std::vector<float>& q,
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
                        score += q[base + q_idx * head_dim + d] * k[base + kv_idx * head_dim + d];
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

// ---------------------------------------------------------------------------
// Convolution: 2D convolution (naive reference)
// ---------------------------------------------------------------------------
inline std::vector<float> cpu_conv2d(const std::vector<float>& input, const std::vector<float>& weight,
                                     const hpc::convolution::ConvParams& p) {
    const int out_h =
        (p.in_height + 2 * p.pad_h - p.dilation_h * (p.kernel_h - 1) - 1) / p.stride_h + 1;
    const int out_w =
        (p.in_width + 2 * p.pad_w - p.dilation_w * (p.kernel_w - 1) - 1) / p.stride_w + 1;
    std::vector<float> output(p.batch * p.out_channels * out_h * out_w, 0.0f);

    for (int b = 0; b < p.batch; ++b) {
        for (int oc = 0; oc < p.out_channels; ++oc) {
            for (int oh = 0; oh < out_h; ++oh) {
                for (int ow = 0; ow < out_w; ++ow) {
                    float sum = 0.0f;
                    for (int ic = 0; ic < p.in_channels; ++ic) {
                        for (int kh = 0; kh < p.kernel_h; ++kh) {
                            for (int kw = 0; kw < p.kernel_w; ++kw) {
                                const int ih = oh * p.stride_h - p.pad_h + kh * p.dilation_h;
                                const int iw = ow * p.stride_w - p.pad_w + kw * p.dilation_w;
                                if (ih < 0 || ih >= p.in_height || iw < 0 || iw >= p.in_width) {
                                    continue;
                                }
                                const int input_idx =
                                    b * (p.in_channels * p.in_height * p.in_width) +
                                    ic * (p.in_height * p.in_width) + ih * p.in_width + iw;
                                const int weight_idx =
                                    oc * (p.in_channels * p.kernel_h * p.kernel_w) +
                                    ic * (p.kernel_h * p.kernel_w) + kh * p.kernel_w + kw;
                                sum += input[input_idx] * weight[weight_idx];
                            }
                        }
                    }
                    const int output_idx =
                        b * (p.out_channels * out_h * out_w) + oc * (out_h * out_w) + oh * out_w + ow;
                    output[output_idx] = sum;
                }
            }
        }
    }

    return output;
}

}  // namespace hpc::reference
