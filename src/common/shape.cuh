#pragma once

#include <cstddef>
#include <initializer_list>
#include <stdexcept>
#include <string>
#include <vector>

namespace hpc {

// ---------------------------------------------------------------------------
// Shape: a small, deep module for multi-dimensional tensor indexing.
// ---------------------------------------------------------------------------
struct Shape {
    std::vector<int> dims;

    Shape() = default;
    explicit Shape(std::initializer_list<int> dims) : dims(dims) {
        for (int d : dims) {
            if (d <= 0) {
                throw std::invalid_argument("Shape dimensions must be positive");
            }
        }
    }
    explicit Shape(const std::vector<int>& dims) : dims(dims) {
        for (int d : dims) {
            if (d <= 0) {
                throw std::invalid_argument("Shape dimensions must be positive");
            }
        }
    }

    [[nodiscard]] size_t size() const noexcept {
        size_t s = 1;
        for (int d : dims) {
            s *= static_cast<size_t>(d);
        }
        return s;
    }

    [[nodiscard]] int ndim() const noexcept {
        return static_cast<int>(dims.size());
    }

    [[nodiscard]] int operator[](int i) const noexcept {
        return dims[i];
    }

    [[nodiscard]] bool operator==(const Shape& other) const noexcept {
        return dims == other.dims;
    }

    [[nodiscard]] bool operator!=(const Shape& other) const noexcept {
        return dims != other.dims;
    }

    [[nodiscard]] std::string to_string() const {
        std::string s = "(";
        for (size_t i = 0; i < dims.size(); ++i) {
            if (i > 0) s += ", ";
            s += std::to_string(dims[i]);
        }
        s += ")";
        return s;
    }
};

}  // namespace hpc
