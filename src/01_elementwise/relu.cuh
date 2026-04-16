#pragma once

#include <cuda_fp16.h>
#include <cuda_runtime.h>

#include <concepts>

#include "../common/opt_level.cuh"

namespace hpc::elementwise {

// ReLU kernel interface
template <typename T, OptLevel Level = OptLevel::GridStride>
    requires std::is_same_v<T, float> || std::is_same_v<T, __half>
void relu(const T* input, T* output, size_t n, cudaStream_t stream = nullptr);

}  // namespace hpc::elementwise
