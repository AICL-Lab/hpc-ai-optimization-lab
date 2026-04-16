#pragma once

namespace hpc::elementwise {

// Optimization levels for elementwise operations
enum class OptLevel {
    Naive,       // Basic implementation
    Vectorized,  // float4 load/store
    GridStride   // Grid stride loop
};

}  // namespace hpc::elementwise
