#pragma once

#include <cuda_bf16.h>
#include <cuda_fp16.h>
#include <cuda_runtime.h>

#include <concepts>
#include <cstring>
#include <vector>

#include "cuda_check.cuh"
#include "shape.cuh"

namespace hpc {

// Concept for CUDA-compatible numeric types
template <typename T>
concept CudaNumeric =
    std::is_arithmetic_v<T> || std::is_same_v<T, __half> || std::is_same_v<T, __nv_bfloat16>;

// RAII wrapper for GPU memory with optional multi-dimensional shape
//
// Depth added by this module:
//   - Memory management (allocation / deallocation)
//   - Host-device transfers (sync and async)
//   - Shape metadata (self-describing dimensions for kernels that consume it)
//
// The shape seam is lazy: a 1D Tensor constructed with only a size has an
// empty dims vector, preserving the original flat-pointer interface.
// Callers may optionally reshape() to attach semantic dimensions.
template <CudaNumeric T>
class Tensor {
public:
    // 1D constructor (original behaviour)
    explicit Tensor(size_t size) : size_(size), data_(nullptr) {
        if (size_ > 0) {
            CUDA_CHECK(cudaMalloc(&data_, size_ * sizeof(T)));
        }
    }

    // Multi-dimensional constructor
    explicit Tensor(const Shape& shape) : Tensor(shape.size()) {
        dims_ = shape.dims;
    }

    explicit Tensor(std::initializer_list<int> dims) : Tensor(Shape(dims)) {}

    ~Tensor() {
        if (data_) {
            cudaFree(data_);
        }
    }

    // Move semantics
    Tensor(Tensor&& other) noexcept
        : size_(other.size_), data_(other.data_), dims_(std::move(other.dims_)) {
        other.data_ = nullptr;
        other.size_ = 0;
    }

    Tensor& operator=(Tensor&& other) noexcept {
        if (this != &other) {
            if (data_)
                cudaFree(data_);
            data_ = other.data_;
            size_ = other.size_;
            dims_ = std::move(other.dims_);
            other.data_ = nullptr;
            other.size_ = 0;
        }
        return *this;
    }

    // Delete copy operations
    Tensor(const Tensor&) = delete;
    Tensor& operator=(const Tensor&) = delete;

    // Accessors
    [[nodiscard]] T* data() noexcept {
        return data_;
    }
    [[nodiscard]] const T* data() const noexcept {
        return data_;
    }
    [[nodiscard]] size_t size() const noexcept {
        return size_;
    }
    [[nodiscard]] size_t bytes() const noexcept {
        return size_ * sizeof(T);
    }
    [[nodiscard]] bool empty() const noexcept {
        return size_ == 0;
    }

    // Shape seam
    [[nodiscard]] bool has_shape() const noexcept {
        return !dims_.empty();
    }

    [[nodiscard]] Shape shape() const noexcept {
        return has_shape() ? Shape(dims_) : Shape{static_cast<int>(size_)};
    }

    [[nodiscard]] int ndim() const noexcept {
        return has_shape() ? static_cast<int>(dims_.size()) : 1;
    }

    [[nodiscard]] int dim(int i) const noexcept {
        if (has_shape()) {
            return dims_[i];
        }
        // 1D fallback: only valid index is 0, returns total size
        return (i == 0) ? static_cast<int>(size_) : 0;
    }

    // Flatten a multi-dimensional index into a linear offset
    [[nodiscard]] size_t offset(std::initializer_list<int> coords) const noexcept {
        size_t idx = 0;
        size_t stride = 1;
        auto it = coords.end();
        const auto& d = has_shape() ? dims_ : std::vector<int>{static_cast<int>(size_)};
        auto dim_it = d.end();
        while (it != coords.begin()) {
            --it;
            --dim_it;
            idx += static_cast<size_t>(*it) * stride;
            stride *= static_cast<size_t>(*dim_it);
        }
        return idx;
    }

    void reshape(const Shape& shape) {
        if (shape.size() != size_) {
            throw std::invalid_argument("reshape: new shape size (" + std::to_string(shape.size()) +
                                        ") does not match total elements (" +
                                        std::to_string(size_) + ")");
        }
        dims_ = shape.dims;
    }

    void reshape(std::initializer_list<int> dims) {
        reshape(Shape(dims));
    }

    // Host-device transfers
    void copy_from_host(const T* host_data) {
        CUDA_CHECK(cudaMemcpy(data_, host_data, bytes(), cudaMemcpyHostToDevice));
    }

    void copy_to_host(T* host_data) const {
        CUDA_CHECK(cudaMemcpy(host_data, data_, bytes(), cudaMemcpyDeviceToHost));
    }

    void copy_from_host(const std::vector<T>& host_vec) {
        copy_from_host(host_vec.data());
    }

    [[nodiscard]] std::vector<T> to_host() const {
        std::vector<T> result(size_);
        copy_to_host(result.data());
        return result;
    }

    // Fill with zeros
    void zero() {
        CUDA_CHECK(cudaMemset(data_, 0, bytes()));
    }

    // Async versions
    void copy_from_host_async(const T* host_data, cudaStream_t stream) {
        CUDA_CHECK(cudaMemcpyAsync(data_, host_data, bytes(), cudaMemcpyHostToDevice, stream));
    }

    void copy_to_host_async(T* host_data, cudaStream_t stream) const {
        CUDA_CHECK(cudaMemcpyAsync(host_data, data_, bytes(), cudaMemcpyDeviceToHost, stream));
    }

private:
    size_t size_;
    T* data_;
    std::vector<int> dims_;  // Empty for legacy 1D-only tensors
};

}  // namespace hpc
