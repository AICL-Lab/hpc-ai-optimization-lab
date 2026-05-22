#pragma once

#include <cuda_fp16.h>
#include <cuda_runtime.h>

#include <concepts>

namespace hpc::attention {

struct FlashAttnConfig {
    int batch_size;
    int num_heads;
    int seq_len;
    int head_dim;
    float scale;  // 1/sqrt(head_dim)
    bool causal;  // Causal mask
};

template <typename T>
    requires std::is_same_v<T, float>
void flash_attention_forward(const T* Q, const T* K, const T* V, T* O,
                             const FlashAttnConfig& config, cudaStream_t stream = nullptr);

}  // namespace hpc::attention
