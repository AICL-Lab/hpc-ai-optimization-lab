#include <vector>

#include <gtest/gtest.h>

#include "../reference/reference_kernels.hpp"
#include "../test_utils.hpp"
#include "common/tensor.cuh"
#include "convolution/conv_implicit_gemm.cuh"

using hpc::reference::cpu_conv2d;

TEST(ConvolutionTest, ImplicitGemmMatchesReference) {
    const hpc::convolution::ConvParams params{
        1, 2, 3, 5, 5, 3, 3, 1, 1, 1, 1, 1, 1,
    };
    const int out_h =
        (params.in_height + 2 * params.pad_h - params.dilation_h * (params.kernel_h - 1) - 1) /
            params.stride_h +
        1;
    const int out_w =
        (params.in_width + 2 * params.pad_w - params.dilation_w * (params.kernel_w - 1) - 1) /
            params.stride_w +
        1;

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

    hpc::convolution::conv2d_implicit_gemm<float>(d_input.data(), d_weight.data(), d_output.data(),
                                                  params);
    cudaDeviceSynchronize();

    const auto output = d_output.to_host();
    ASSERT_EQ(output.size(),
              static_cast<size_t>(params.batch * params.out_channels * out_h * out_w));
    EXPECT_TRUE(hpc::test::vectors_almost_equal(output, expected, 1e-4f, 1e-4f));
}
