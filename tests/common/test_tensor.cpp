#include <gtest/gtest.h>
#include <rapidcheck.h>
#include <rapidcheck/gtest.h>

#include "../test_utils.hpp"
#include "common/tensor.cuh"

// Feature: hpc-ai-optimization-lab, Property 1: Tensor Host-Device Round Trip
RC_GTEST_PROP(TensorTest, HostDeviceRoundTrip, ()) {
    auto size = *rc::gen::inRange<size_t>(1, 1024 * 64);
    auto input = *rc::gen::container<std::vector<float>>(size, rc::gen::arbitrary<float>());

    hpc::Tensor<float> tensor(size);
    tensor.copy_from_host(input);
    auto output = tensor.to_host();

    RC_ASSERT(input.size() == output.size());
    for (size_t i = 0; i < input.size(); ++i) {
        RC_ASSERT(input[i] == output[i]);
    }
}

TEST(TensorTest, BasicAllocation) {
    hpc::Tensor<float> tensor(1024);
    EXPECT_EQ(tensor.size(), 1024);
    EXPECT_EQ(tensor.bytes(), 1024 * sizeof(float));
    EXPECT_NE(tensor.data(), nullptr);
}

TEST(TensorTest, MoveSemantics) {
    hpc::Tensor<float> tensor1(1024);
    float* ptr = tensor1.data();

    hpc::Tensor<float> tensor2 = std::move(tensor1);
    EXPECT_EQ(tensor2.data(), ptr);
    EXPECT_EQ(tensor1.data(), nullptr);
    EXPECT_EQ(tensor1.size(), 0);
}

TEST(TensorTest, ZeroFill) {
    hpc::Tensor<float> tensor(1024);
    tensor.zero();

    auto host_data = tensor.to_host();
    for (float val : host_data) {
        EXPECT_EQ(val, 0.0f);
    }
}

TEST(TensorTest, MultiDimConstruction) {
    hpc::Tensor<float> tensor({2, 3, 4});
    EXPECT_EQ(tensor.size(), 24);
    EXPECT_EQ(tensor.ndim(), 3);
    EXPECT_EQ(tensor.dim(0), 2);
    EXPECT_EQ(tensor.dim(1), 3);
    EXPECT_EQ(tensor.dim(2), 4);
    EXPECT_TRUE(tensor.has_shape());
}

TEST(TensorTest, ShapeRoundTrip) {
    hpc::Tensor<float> tensor({4, 5});
    auto shape = tensor.shape();
    EXPECT_EQ(shape.ndim(), 2);
    EXPECT_EQ(shape[0], 4);
    EXPECT_EQ(shape[1], 5);
}

TEST(TensorTest, ReshapeValid) {
    hpc::Tensor<float> tensor(24);
    tensor.reshape({2, 3, 4});
    EXPECT_EQ(tensor.ndim(), 3);
    EXPECT_EQ(tensor.dim(0), 2);
    EXPECT_EQ(tensor.dim(1), 3);
    EXPECT_EQ(tensor.dim(2), 4);
}

TEST(TensorTest, ReshapeRejectsMismatchedSize) {
    hpc::Tensor<float> tensor(24);
    EXPECT_THROW(tensor.reshape({5, 5}), std::invalid_argument);
}

TEST(TensorTest, OffsetCalculation) {
    hpc::Tensor<float> tensor({3, 4, 5});
    // row-major layout: index(i,j,k) = ((i * 4) + j) * 5 + k
    EXPECT_EQ(tensor.offset({0, 0, 0}), 0);
    EXPECT_EQ(tensor.offset({0, 0, 1}), 1);
    EXPECT_EQ(tensor.offset({0, 1, 0}), 5);
    EXPECT_EQ(tensor.offset({1, 0, 0}), 20);
    EXPECT_EQ(tensor.offset({1, 2, 3}), 1 * 20 + 2 * 5 + 3);
}

TEST(TensorTest, OneDFallbackShape) {
    hpc::Tensor<float> tensor(100);
    EXPECT_FALSE(tensor.has_shape());
    EXPECT_EQ(tensor.ndim(), 1);
    EXPECT_EQ(tensor.dim(0), 100);
    auto shape = tensor.shape();
    EXPECT_EQ(shape.ndim(), 1);
    EXPECT_EQ(shape[0], 100);
}
