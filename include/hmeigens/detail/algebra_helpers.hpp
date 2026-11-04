// File for functions that perform algebraic operations on groups of complex numbers.
// Declarations (hmeigens::detail):
//     - twoNorm(std::span<const Complex>);
//     - complexUnitVector(Complex).
#ifndef HMEIGENS_DETAIL_ALGEBRA_HELPERS_HPP
#define HMEIGENS_DETAIL_ALGEBRA_HELPERS_HPP

#include "hmeigens/scalar.hpp"

#include <span>

namespace hmeigens::detail {
    
    // Euclidean 2-norm of a vector of complex numbers.
    [[nodiscard]] Scalar twoNorm(std::span<const Complex> vector);

    // Gets the complex unit vector of a given complex number, i.e. the unit-length complex number in the direction of input.
    // If input is exactly (0.; 0.) it returns (1.; 0.).
    // For any other input it returns a complex number of length 1. and phase equal to that of input.
    [[nodiscard]] Complex complexUnitVector(Complex input);
}

#endif
