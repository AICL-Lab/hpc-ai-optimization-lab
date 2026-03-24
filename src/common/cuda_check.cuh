#pragma once

#include <cuda_runtime.h>
#include <sstream>
#include <stdexcept>
#include <string>

namespace hpc::detail {

[[noreturn]] inline void throw_cuda_error(cudaError_t err, const char* file, int line) {
    std::ostringstream message;
    message << "CUDA error at " << file << ':' << line << ": "
            << cudaGetErrorString(err);
    throw std::runtime_error(message.str());
}

} // namespace hpc::detail

// Macros are not scoped by namespaces; define them at file scope.
#define CUDA_CHECK(call)                                                        \
    do {                                                                        \
        cudaError_t err = (call);                                               \
        if (err != cudaSuccess) {                                               \
            ::hpc::detail::throw_cuda_error(err, __FILE__, __LINE__);           \
        }                                                                       \
    } while (0)

#define CUDA_CHECK_LAST() CUDA_CHECK(cudaGetLastError())
