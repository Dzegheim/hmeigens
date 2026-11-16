// File for functions that perform algebraic operations on complex numbers.
// Declarations (hmeigens::detail):
//     - twoNorm(std::span<const Complex>);
//     - complexUnitVector(Complex);
//     - product(const Complex&, const Complex&).
// Definitions (hmeigens::detail):
//     - product(const Complex&, const Complex&).
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

    // Product of two complex numbers.
    // The benchmark in benchmarks/complex_multiplication_bench.cpp shows that an explicitly written complex multiplication is faster than the standard operator* from <complex>.
    // This is because the standard version may perform safety checks on the finiteness of numbers. Those checks are not needed here, as the finiteness of the matrix elements is a prerequisite this code will enforce in the eigenvalue search. Those checks, though not required, may happen, and slow the calculations down.
    // The inputs of this function must be finite.
    // Why not operator*? Because it conflicts with the standard one. It's all fine and dandy in hmeigens::detail, but in hmeigens the compiler resolves to the standard one and the speedup is lost, so product it is.
    [[nodiscard]] Complex product(const Complex& l, const Complex& r);
}

/* ----------------------------- */
/* -------- Definitions -------- */
/* ----------------------------- */
//
// All functions are fully documented at their declarations.
//
inline hmeigens::Complex hmeigens::detail::product(const hmeigens::Complex& l, const hmeigens::Complex& r) {
    const auto lRe = l.real();
    const auto lIm = l.imag();
    const auto rRe = r.real();
    const auto rIm = r.imag();
    return hmeigens::Complex{
        lRe*rRe-lIm*rIm,
        lRe*rIm+lIm*rRe
    };
}

#endif
