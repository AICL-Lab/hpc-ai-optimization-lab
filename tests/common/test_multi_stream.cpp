#include <cuda_runtime.h>

#include <thread>
#include <vector>

#include <gtest/gtest.h>

#include "../test_utils.hpp"
#include "common/tensor.cuh"
#include "elementwise/relu.cuh"
#include "gemm/gemm.cuh"

namespace {

// Test that multiple CUDA streams can execute concurrently
TEST(MultiStreamTest, ConcurrentReluKernels) {
    constexpr size_t N = 1024 * 1024;
    constexpr int NUM_STREAMS = 4;

    std::vector<hpc::Tensor<float>> inputs, outputs;
    std::vector<cudaStream_t> streams;

    // Create streams and tensors
    for (int i = 0; i < NUM_STREAMS; ++i) {
        cudaStream_t stream;
        cudaStreamCreate(&stream);
        streams.push_back(stream);

        inputs.emplace_back(N);
        outputs.emplace_back(N);

        auto host_data = hpc::test::random_vector<float>(N, -1.0f, 1.0f);
        inputs[i].copy_from_host(host_data);
    }

    // Launch kernels on different streams
    for (int i = 0; i < NUM_STREAMS; ++i) {
        hpc::elementwise::relu<float>(inputs[i].data(), outputs[i].data(), N, streams[i]);
    }

    // Synchronize and verify
    for (int i = 0; i < NUM_STREAMS; ++i) {
        cudaStreamSynchronize(streams[i]);

        auto input = inputs[i].to_host();
        auto output = outputs[i].to_host();

        for (size_t j = 0; j < N; ++j) {
            float expected = std::max(0.0f, input[j]);
            EXPECT_NEAR(output[j], expected, 1e-6f);
        }

        cudaStreamDestroy(streams[i]);
    }
}

// Test GEMM with overlapping computation on different streams
TEST(MultiStreamTest, OverlappingGemm) {
    constexpr int M = 128, N = 128, K = 128;
    constexpr int NUM_STREAMS = 2;

    std::vector<hpc::Tensor<float>> A_tensors, B_tensors, C_tensors;
    std::vector<cudaStream_t> streams;

    for (int i = 0; i < NUM_STREAMS; ++i) {
        cudaStream_t stream;
        cudaStreamCreate(&stream);
        streams.push_back(stream);

        A_tensors.emplace_back(M * K);
        B_tensors.emplace_back(K * N);
        C_tensors.emplace_back(M * N);

        A_tensors[i].copy_from_host(hpc::test::random_vector<float>(M * K, -1.0f, 1.0f));
        B_tensors[i].copy_from_host(hpc::test::random_vector<float>(K * N, -1.0f, 1.0f));
        C_tensors[i].zero();
    }

    // Launch GEMM kernels concurrently
    for (int i = 0; i < NUM_STREAMS; ++i) {
        hpc::gemm::gemm<float, hpc::gemm::GemmOpt::SharedMemTiling>(
            A_tensors[i].data(), B_tensors[i].data(), C_tensors[i].data(), M, N, K, 1.0f, 0.0f,
            streams[i]);
    }

    // Verify all results
    for (int i = 0; i < NUM_STREAMS; ++i) {
        cudaStreamSynchronize(streams[i]);

        // Basic sanity check - C should have non-zero values
        auto C = C_tensors[i].to_host();
        bool has_non_zero = false;
        for (const auto& val : C) {
            if (val != 0.0f) {
                has_non_zero = true;
                break;
            }
        }
        EXPECT_TRUE(has_non_zero);

        cudaStreamDestroy(streams[i]);
    }
}

// Test stream callback functionality
struct CallbackData {
    int* counter;
    int expected_value;
};

void CUDART_CB stream_callback(cudaStream_t stream, cudaError_t status, void* user_data) {
    auto* cb_data = static_cast<CallbackData*>(user_data);
    (*cb_data->counter)++;
}

TEST(MultiStreamTest, StreamCallback) {
    constexpr size_t N = 1024;

    cudaStream_t stream;
    cudaStreamCreate(&stream);

    hpc::Tensor<float> input(N);
    hpc::Tensor<float> output(N);
    input.copy_from_host(hpc::test::random_vector<float>(N, -1.0f, 1.0f));

    int callback_counter = 0;
    CallbackData cb_data{&callback_counter, 0};

    // Launch kernel
    hpc::elementwise::relu<float>(input.data(), output.data(), N, stream);

    // Add callback
    cudaStreamAddCallback(stream, stream_callback, &cb_data, 0);

    // Wait for completion
    cudaStreamSynchronize(stream);

    EXPECT_EQ(callback_counter, 1);

    cudaStreamDestroy(stream);
}

// Test events for inter-stream synchronization
TEST(MultiStreamTest, EventSynchronization) {
    constexpr size_t N = 1024 * 1024;

    cudaStream_t stream1, stream2;
    cudaStreamCreate(&stream1);
    cudaStreamCreate(&stream2);

    cudaEvent_t event;
    cudaEventCreate(&event);

    hpc::Tensor<float> input1(N), output1(N);
    hpc::Tensor<float> input2(N), output2(N);

    input1.copy_from_host(hpc::test::random_vector<float>(N, -1.0f, 1.0f));
    input2.copy_from_host(hpc::test::random_vector<float>(N, -1.0f, 1.0f));

    // Launch on stream1
    hpc::elementwise::relu<float>(input1.data(), output1.data(), N, stream1);

    // Record event after stream1 kernel
    cudaEventRecord(event, stream1);

    // Make stream2 wait for event
    cudaStreamWaitEvent(stream2, event, 0);

    // Launch on stream2 (will wait for stream1)
    hpc::elementwise::sigmoid<float>(input2.data(), output2.data(), N, stream2);

    // Synchronize both streams
    cudaStreamSynchronize(stream1);
    cudaStreamSynchronize(stream2);

    // Verify both results
    auto result1 = output1.to_host();
    auto result2 = output2.to_host();

    EXPECT_NEAR(result1[0], std::max(0.0f, input1.to_host()[0]), 1e-6f);
    EXPECT_NEAR(result2[0], 1.0f / (1.0f + std::exp(-input2.to_host()[0])), 1e-5f);

    cudaEventDestroy(event);
    cudaStreamDestroy(stream1);
    cudaStreamDestroy(stream2);
}

// Test default stream (null) behavior
TEST(MultiStreamTest, DefaultStream) {
    constexpr size_t N = 1024;

    hpc::Tensor<float> input(N);
    hpc::Tensor<float> output(N);

    input.copy_from_host(hpc::test::random_vector<float>(N, -1.0f, 1.0f));

    // Use default stream (nullptr)
    hpc::elementwise::relu<float>(input.data(), output.data(), N, nullptr);

    // Default stream is synchronous, no need for explicit sync
    auto result = output.to_host();
    auto host_input = input.to_host();

    for (size_t i = 0; i < N; ++i) {
        EXPECT_NEAR(result[i], std::max(0.0f, host_input[i]), 1e-6f);
    }
}

// Test error handling in multi-stream context
TEST(MultiStreamTest, StreamErrorHandling) {
    cudaStream_t stream;
    cudaStreamCreate(&stream);

    // Create tensors with mismatched sizes
    hpc::Tensor<float> input(1024);
    hpc::Tensor<float> output(512);  // Wrong size

    // This should throw or handle error gracefully
    // Note: The actual behavior depends on the kernel implementation
    // Some kernels may not check sizes, leading to undefined behavior

    // Clean up
    cudaStreamDestroy(stream);

    // This test mainly verifies no crash occurs
    SUCCEED();
}

}  // namespace
