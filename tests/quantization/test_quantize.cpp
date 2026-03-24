#include <gtest/gtest.h>
#include <rapidcheck.h>
#include <rapidcheck/gtest.h>
#include <algorithm>
#include <cmath>
#include <vector>

#include "06_quantization/dequant.cuh"
#include "06_quantization/int8_quant.cuh"
#include "common/tensor.cuh"
#include "../test_utils.hpp"

namespace {

float row_max_abs(const std::vector<float>& input, int row, int cols) {
    float max_abs = 0.0f;
    for (int col = 0; col < cols; ++col) {
        max_abs = std::max(max_abs, std::abs(input[row * cols + col]));
    }
    return max_abs;
}

} // namespace

RC_GTEST_PROP(QuantizationTest, RoundTripRespectsPerRowScale, ()) {
    const auto rows = *rc::gen::inRange<int>(1, 32);
    const auto cols = *rc::gen::inRange<int>(16, 192);
    auto input = *rc::gen::container<std::vector<float>>(rows * cols,
        rc::gen::map(rc::gen::arbitrary<float>(), [](float x) {
            return std::clamp(x, -1.0f, 1.0f);
        }));

    hpc::Tensor<float> d_input(rows * cols);
    hpc::Tensor<int8_t> d_quantized(rows * cols);
    hpc::Tensor<float> d_scale(rows);
    hpc::Tensor<float> d_output(rows * cols);

    d_input.copy_from_host(input);

    hpc::quantization::quantize_int8(
        d_input.data(), d_quantized.data(), d_scale.data(), rows, cols);
    hpc::quantization::dequantize_int8(
        d_quantized.data(), d_scale.data(), d_output.data(), rows, cols);
    cudaDeviceSynchronize();

    const auto scales = d_scale.to_host();
    const auto result = d_output.to_host();

    for (int row = 0; row < rows; ++row) {
        RC_ASSERT(std::isfinite(scales[row]));
        RC_ASSERT(scales[row] > 0.0f);

        const float expected_scale = row_max_abs(input, row, cols) > 0.0f
            ? row_max_abs(input, row, cols) / 127.0f
            : 1.0f;
        RC_ASSERT(hpc::test::almost_equal(scales[row], expected_scale, 1e-5f, 1e-6f));

        for (int col = 0; col < cols; ++col) {
            const int idx = row * cols + col;
            const float error = std::abs(result[idx] - input[idx]);
            RC_ASSERT(error <= scales[row] + 1e-4f);
        }
    }
}

TEST(QuantizationTest, ZeroRowsUseUnitScaleAndStayZero) {
    constexpr int rows = 3;
    constexpr int cols = 8;
    const std::vector<float> input{
        0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        -0.5f, 0.25f, 0.75f, -1.0f, 0.125f, -0.875f, 0.3f, -0.2f,
        0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
    };

    hpc::Tensor<float> d_input(rows * cols);
    hpc::Tensor<int8_t> d_quantized(rows * cols);
    hpc::Tensor<float> d_scale(rows);
    hpc::Tensor<float> d_output(rows * cols);

    d_input.copy_from_host(input);

    hpc::quantization::quantize_int8(
        d_input.data(), d_quantized.data(), d_scale.data(), rows, cols);
    hpc::quantization::dequantize_int8(
        d_quantized.data(), d_scale.data(), d_output.data(), rows, cols);
    cudaDeviceSynchronize();

    const auto quantized = d_quantized.to_host();
    const auto scales = d_scale.to_host();
    const auto result = d_output.to_host();

    EXPECT_FLOAT_EQ(scales[0], 1.0f);
    EXPECT_FLOAT_EQ(scales[2], 1.0f);
    EXPECT_GT(scales[1], 0.0f);

    for (int col = 0; col < cols; ++col) {
        EXPECT_EQ(quantized[col], 0);
        EXPECT_EQ(quantized[2 * cols + col], 0);
        EXPECT_FLOAT_EQ(result[col], 0.0f);
        EXPECT_FLOAT_EQ(result[2 * cols + col], 0.0f);
    }
}

TEST(QuantizationTest, BasicRoundTrip) {
    constexpr int rows = 4;
    constexpr int cols = 64;
    const auto input = hpc::test::random_vector<float>(rows * cols, -1.0f, 1.0f);

    hpc::Tensor<float> d_input(rows * cols);
    hpc::Tensor<int8_t> d_quantized(rows * cols);
    hpc::Tensor<float> d_scale(rows);
    hpc::Tensor<float> d_output(rows * cols);

    d_input.copy_from_host(input);

    hpc::quantization::quantize_int8(
        d_input.data(), d_quantized.data(), d_scale.data(), rows, cols);
    hpc::quantization::dequantize_int8(
        d_quantized.data(), d_scale.data(), d_output.data(), rows, cols);
    cudaDeviceSynchronize();

    const auto scales = d_scale.to_host();
    const auto result = d_output.to_host();

    for (int row = 0; row < rows; ++row) {
        ASSERT_TRUE(std::isfinite(scales[row]));
        ASSERT_GT(scales[row], 0.0f);
        for (int col = 0; col < cols; ++col) {
            const int idx = row * cols + col;
            EXPECT_LE(std::abs(result[idx] - input[idx]), scales[row] + 1e-4f);
        }
    }
}
