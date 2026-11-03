// File for functions that perform algebraic operations on groups of complex numbers.
// Declarations (hmeigens::detail):
//     - twoNorm(std::span<const Complex>).
#ifndef HMEIGENS_DETAIL_ALGEBRA_HELPERS_HPP
#define HMEIGENS_DETAIL_ALGEBRA_HELPERS_HPP

#include "hmeigens/scalar.hpp"

#include <span>

namespace hmeigens::detail {
    
    // Euclidean 2-norm of a vector of complex numbers.
    [[nodiscard]] Scalar twoNorm(std::span<const Complex> vector);
}

#endif
