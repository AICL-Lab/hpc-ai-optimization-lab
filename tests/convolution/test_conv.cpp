#include <gtest/gtest.h>
#include <vector>

#include "04_convolution/conv_implicit_gemm.cuh"
#include "04_convolution/conv_winograd.cuh"
#include "common/tensor.cuh"
#include "../test_utils.hpp"

namespace {

std::vector<float> cpu_conv2d(const std::vector<float>& input,
                              const std::vector<float>& weight,
                              const hpc::convolution::ConvParams& p) {
    const int out_h = (p.in_height + 2 * p.pad_h - p.dilation_h * (p.kernel_h - 1) - 1) / p.stride_h + 1;
    const int out_w = (p.in_width + 2 * p.pad_w - p.dilation_w * (p.kernel_w - 1) - 1) / p.stride_w + 1;
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
                                const int input_idx = b * (p.in_channels * p.in_height * p.in_width) +
                                                      ic * (p.in_height * p.in_width) +
                                                      ih * p.in_width + iw;
                                const int weight_idx = oc * (p.in_channels * p.kernel_h * p.kernel_w) +
                                                       ic * (p.kernel_h * p.kernel_w) +
                                                       kh * p.kernel_w + kw;
                                sum += input[input_idx] * weight[weight_idx];
                            }
                        }
                    }
                    const int output_idx = b * (p.out_channels * out_h * out_w) +
                                           oc * (out_h * out_w) + oh * out_w + ow;
                    output[output_idx] = sum;
                }
            }
        }
    }

    return output;
}

} // namespace

TEST(ConvolutionTest, ImplicitGemmMatchesReference) {
    const hpc::convolution::ConvParams params{
        1, 2, 3, 5, 5,
        3, 3, 1, 1, 1, 1, 1, 1,
    };
    const int out_h = (params.in_height + 2 * params.pad_h - params.dilation_h * (params.kernel_h - 1) - 1) /
                      params.stride_h + 1;
    const int out_w = (params.in_width + 2 * params.pad_w - params.dilation_w * (params.kernel_w - 1) - 1) /
                      params.stride_w + 1;

    const auto input = hpc::test::random_vector<float>(
        params.batch * params.in_channels * params.in_height * params.in_width, -1.0f, 1.0f);
    const auto weight = hpc::test::random_vector<float>(
        params.out_channels * params.in_channels * params.kernel_h * params.kernel_w, -1.0f, 1.0f);
    const auto expected = cpu_conv2d(input, weight, params);

    hpc::Tensor<float> d_input(input.size());
    hpc::Tensor<float> d_weight(weight.size());
    hpc::Tensor<float> d_output(expected.size());

    d_input.copy_from_host(input);
    d_weight.copy_from_host(weight);
    d_output.zero();

    hpc::convolution::conv2d_implicit_gemm<float>(
        d_input.data(), d_weight.data(), d_output.data(), params);
    cudaDeviceSynchronize();

    const auto output = d_output.to_host();
    ASSERT_EQ(output.size(), static_cast<size_t>(params.batch * params.out_channels * out_h * out_w));
    EXPECT_TRUE(hpc::test::vectors_almost_equal(output, expected, 1e-4f, 1e-4f));
}

TEST(ConvolutionTest, WinogradPathMatchesImplicitGemmFallback) {
    constexpr int batch = 1;
    constexpr int in_channels = 2;
    constexpr int out_channels = 2;
    constexpr int height = 6;
    constexpr int width = 6;
    constexpr int kernel = 3;
    const int out_h = height;  // stride=1, pad=1, kernel=3 -> same output size
    const int out_w = width;
    constexpr int output_size = batch * out_channels * out_h * out_w;

    const auto input = hpc::test::random_vector<float>(batch * in_channels * height * width, -1.0f, 1.0f);
    const auto weight = hpc::test::random_vector<float>(out_channels * in_channels * kernel * kernel, -1.0f, 1.0f);

    hpc::Tensor<float> d_input(input.size());
    hpc::Tensor<float> d_weight(weight.size());
    hpc::Tensor<float> d_implicit(output_size);
    hpc::Tensor<float> d_winograd(output_size);

    d_input.copy_from_host(input);
    d_weight.copy_from_host(weight);
    d_implicit.zero();
    d_winograd.zero();

    const hpc::convolution::ConvParams params{
        batch, in_channels, out_channels, height, width,
        kernel, kernel, 1, 1, 1, 1, 1, 1,
    };

    hpc::convolution::conv2d_implicit_gemm<float>(
        d_input.data(), d_weight.data(), d_implicit.data(), params);
    hpc::convolution::conv2d_winograd(
        d_input.data(), d_weight.data(), d_winograd.data(), params);
    cudaDeviceSynchronize();

    const auto implicit_output = d_implicit.to_host();
    const auto winograd_output = d_winograd.to_host();
    EXPECT_TRUE(hpc::test::vectors_almost_equal(winograd_output, implicit_output, 1e-4f, 1e-4f));
}
