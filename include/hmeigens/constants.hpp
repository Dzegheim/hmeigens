/// @file
/// @brief Constants used throughout the project.
#ifndef HMEIGENS_CONSTANTS_HPP
#define HMEIGENS_CONSTANTS_HPP

#include "hmeigens/scalar.hpp"

#include <limits>
#include <cstddef>  // For std::size_t
#include <cstdint>  // For std::uint64_t

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

    /// @brief The maximum squarable value within std::size_t.
    ///
    /// Let `BST` be the number of bits of `std::size_t` and `**` be the exponentiation operation, then the expression `2**(BST/2)-1` is the largest (unsigned) integer that can be squared without overflowing.
    inline constexpr std::size_t maxSquarableSize = (std::size_t{1} << (std::numeric_limits<std::size_t>::digits/2))-1;
    // These two assertions verify that the operation above behaved properly.
    static_assert(maxSquarableSize <= std::numeric_limits<std::size_t>::max()/maxSquarableSize, "The variable hmeigens::maxSquarableSize cannot overflow when squared.");
    static_assert(maxSquarableSize+1 > std::numeric_limits<std::size_t>::max()/(maxSquarableSize+1), "The variable hmeigens::maxSquarableSize must be the largest possible value that does not overflow when squared.");

    /// @brief Returns the base tolerance for the computation of the norm by hmeigens::SquareMatrix::norm(), used in tests. The true tolerance used in the tests is multiplied by `HMEIGENS_HYPOT_ULPS`.
    ///
    /// The base tolerance is `size*size`, which in the code is always safe to compute provided it comes from an hmeigens::SquareMatrix, as the size of a matrix has the prerequisite of always being squarable within `std::size_t`.
    /// Implementations *usually* guarantee that the precision is less than 1 ULP.
    /// [Source](https://en.cppreference.com/cpp/numeric/math/hypot):
    /// > Implementations usually guarantee precision of less than 1 ulp (Unit in the Last Place — Unit of Least Precision): [GNU](https://sourceware.org/git/?p=glibc.git;a=blob_plain;f=sysdeps/ieee754/dbl-64/e_hypot.c), [BSD](https://svnweb.freebsd.org/base/head/lib/msun/src/e_hypot.c).
    /// Given that a matrix of size `n` has `n*n` elements and assuming a (*usual*) deviation of `1` ULP per call, the tolerance obtained if all errors go in the same direction is the number of elements in ULPs.
    /// Since the standard leaves the accuracy to the implementation, this is taken as a baseline that can then be increased with the compile-time parameter `HMEIGENS_HYPOT_ULPS`, defined in `tests/CMakeLists.txt`.
    /// The value of `HMEIGENS_HYPOT_ULPS` is set to `2` as a default because of measurements [published](https://homepages.loria.fr/PZimmermann/papers/glibc242.pdf) by Gladman, Innocente, Mather, Zimmermann in August 2025.
    /// The comparison of `std::hypot` gives a maximum measured error of `1.57` ULPs for single precision and `1.89` ULPs for double precision. A value of `2` is enough to account for these values, which are however not maximum theoretical errors, but only measured.
    /// The actual tolerance enforced by the tests is therefore, by default, `2*n*n`. The value is however modifiable, because that upper bound is not a standard requirement, and some implementations may break it.
    /// @param size The size of the matrix whose norm is being computed.
    /// @return A `std::uint64_t` that is the square of `size`.
    /// @pre `size <= hmeigens::maxMatrixSize`.
    /// @warning The function performs no overflow check itself. It assumes the given `size` always comes from a matrix.
    inline constexpr std::uint64_t normTolerance(std::size_t size);
}

/* ----------------------------- */
/* -------- Definitions -------- */
/* ----------------------------- */
//
// See the declaration for all relevant information.
inline constexpr std::uint64_t hmeigens::normTolerance(std::size_t size) {
    // Squaring size is always safe if it comes from a matrix.
    return size*size;
}

#endif
