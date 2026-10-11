/// @file
/// @brief Constants used throughout the project.
#ifndef HMEIGENS_CONSTANTS_HPP
#define HMEIGENS_CONSTANTS_HPP

#include "hmeigens/scalar.hpp"

#include <limits>
#include <cstddef>  // For std::size_t

namespace hmeigens {

    /// @brief The difference between `1.0` and the next `hmeigens::Scalar`.
    ///
    /// [Values](https://en.cppreference.com/cpp/types/climits):
    /// - `1.19209e-07` for `float`;
    /// - `2.22045e-16` for `double`.
    inline constexpr Scalar epsilon = std::numeric_limits<Scalar>::epsilon();

    /// @brief The maximum squarable value within `std::size_t`.
    ///
    /// Let `BST` be the number of bits of `std::size_t` and `**` be the exponentiation operation, then the expression `2**(BST/2)-1` is the largest (unsigned) integer that can be squared without overflowing.
    inline constexpr std::size_t maxSquarableSize = (std::size_t{1} << (std::numeric_limits<std::size_t>::digits/2))-1;
    // These two assertions verify that the operation above behaved properly.
    static_assert(maxSquarableSize <= std::numeric_limits<std::size_t>::max()/maxSquarableSize, "The variable hmeigens::maxSquarableSize cannot overflow when squared.");
    static_assert(maxSquarableSize+1 > std::numeric_limits<std::size_t>::max()/(maxSquarableSize+1), "The variable hmeigens::maxSquarableSize must be the largest possible value that does not overflow when squared.");
}

#endif
