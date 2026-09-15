/// @file
/// @brief Constants used throughout the project.
#ifndef HMEIGENS_CONSTANTS_HPP
#define HMEIGENS_CONSTANTS_HPP

#include "hmeigens/scalar.hpp"

#include <limits>

namespace hmeigens {
    /// @brief The difference between `1.0` and the next `hmeigens::Scalar`.
    ///
    /// [Values](https://en.cppreference.com/cpp/types/climits):
    /// - `1.19209e-07` for `float`;
    /// - `2.22045e-16` for `double`.
    inline constexpr Scalar epsilon = std::numeric_limits<Scalar>::epsilon();

    /// @brief Tolerance for parsing tests.
    ///
    /// The parsed value must be within 1 ULP of the input.
    /// No arithmetic is performed on the value, so tolerance is the tightest each type allows.
    /// [Source](https://en.cppreference.com/cpp/utility/from_chars):
    /// > In any case, the resulting value is one of at most two floating-point values closest to the value of the string matching the pattern, after rounding according to `std::round_to_nearest`.
    inline constexpr Scalar parseTolerance = epsilon;

    /// @brief The maximum size a square matrix may have.
    ///
    /// The maximum size must account for the fact that a matrix contains `size * size` elements. The value `size` is verified against this constant, so if `size  > hmeigens::maxMatrixSize` its square would overflow, and a matrix of that `size` would make no sense.
    /// Let `BST` be the number of bits of `std::size_t` and `**` be the exponentiation operation, then the expression `2**(BST/2)-1` is the largest (unsigned) integer that can be squared without overflowing.
    /// @note This and the maximum size of the container used (which is something that the standard handles) are the only upper bounds on the matrix size enforced by this code. If a `1'000'000`x`1'000'000` matrix is created, the user is expected to know what they're doing by creating such a large table of numbers. Who even needs such a large matrix? (:
    inline constexpr std::size_t maxMatrixSize = (std::size_t{1} << (std::numeric_limits<std::size_t>::digits / 2)) - 1;
    // These two assertions verify that the operation above behaved properly.
    static_assert(maxMatrixSize <= std::numeric_limits<std::size_t>::max() / maxMatrixSize, "The variable maxMatrixSize cannot overflow when squared.");
    static_assert(maxMatrixSize + 1 > std::numeric_limits<std::size_t>::max() / (maxMatrixSize + 1),"The variable maxMatrixSize must be the largest possible value that does not overflow when squared.");
}

#endif
