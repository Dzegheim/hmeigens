/// @file
/// @brief Tolerances enforced by the tests of the library.
#ifndef HMEIGENS_TOLERANCES_HPP
#define HMEIGENS_TOLERANCES_HPP

#include <cstddef>  // For std::size_t
#include <cstdint>  // For std::uint64_t

#ifndef HMEIGENS_HYPOT_ULPS
/// @brief ULP multiplier for the norm tests. Default value is 2. Can be set at configure time with `-DHMEIGENS_HYPOT_ULPS=<n>`.
///
/// Default is `2` because of measurements [published](https://homepages.loria.fr/PZimmermann/papers/glibc242.pdf) by Gladman, Innocente, Mather, Zimmermann in August 2025.
/// The comparison of `std::hypot` gives a maximum measured error of `1.57` ULPs for single precision and `1.89` ULPs for double precision. A value of `2` is enough to account for these values, which are however not maximum theoretical errors, but only measured.
/// @note If the variable is set via CMake, its value persists as long as it is cached. To return it to its default value either override it again to `2`, undefine it with `-UHMEIGENS_HYPOT_ULPS`, or simply delete the build directory.
/// @note Tests must be built for the override to be applied (`HMEIGENS_BUILD_TESTS=ON`).
/// @sa hmeigens::normTolerance(std::size_t)
#define HMEIGENS_HYPOT_ULPS 2
#endif

namespace hmeigens {

    /// @brief Tolerance for parsing tests: `1` ULP.
    ///
    /// The parsed value must be within `1` ULP of the expected value, which is a literal in the tests.
    /// All parsing of numbers is ultimately done by `std::from_chars`. No arithmetic is performed on the value, so the tolerance is the tightest the standard guarantees.
    /// For `std::from_chars` the error is `0` ULPs.
    /// [Source](https://en.cppreference.com/cpp/utility/from_chars):
    /// > In any case, the resulting value is one of at most two floating-point values closest to the value of the string matching the pattern, after rounding according to `std::round_to_nearest`.
    ///
    /// This means the only rounding that can happen *not to nearest* on the values is that done by the compiler when reading literals, that is at most `1` ULP.
    /// [Source (note that this is NOT an ISO publication)](https://timsong-cpp.github.io/cppwp/n4861/lex.fcon#3):
    /// > If the scaled value is not in the range of representable values for its type, the program is ill-formed. Otherwise, the value of a floating-point-literal is the scaled value if representable, else the larger or smaller representable value nearest the scaled value, chosen in an implementation-defined manner.
    ///
    /// Depending on the implementation, the rounding could go to either value, so the `1` ULP tolerance is enough.
    inline constexpr std::uint64_t parseTolerance = 1;

    /// @brief Returns the tolerance for the computation of the norm by `hmeigens::SquareMatrix::norm()`.
    ///
    /// Implementations *usually* guarantee that the error is less than 1 ULP.
    /// [Source](https://en.cppreference.com/cpp/numeric/math/hypot):
    /// > Implementations usually guarantee precision of less than 1 ulp (Unit in the Last Place — Unit of Least Precision): [GNU](https://sourceware.org/git/?p=glibc.git;a=blob_plain;f=sysdeps/ieee754/dbl-64/e_hypot.c), [BSD](https://svnweb.freebsd.org/base/head/lib/msun/src/e_hypot.c).
    /// Each element passes through two calls to `std::hypot`, one for the norm and one for the modulus of the complex number.
    /// Given that a matrix of size `n` has `n*n` elements and assuming a (*usual*) deviation of `2` ULPs per element (one per call of `std::hypot`), the tolerance obtained if all errors go in the same direction is twice the number of elements in ULPs.
    /// Since the standard leaves the accuracy to the implementation, this is taken as a baseline that can then be increased with the parameter `HMEIGENS_HYPOT_ULPS`, whose default value is 2.
    /// The actual tolerance enforced by the tests is therefore, by default, \f(4n^2\f). The value is however modifiable at configure time, because that upper bound is not a standard requirement, and some implementations may break it.
    /// See the documentation of `HMEIGENS_HYPOT_ULPS` for details on how its default was chosen and how it can be changed.
    /// @param size The size of the matrix whose norm is being computed.
    /// @return A `std::uint64_t` that is `2*size*size*HMEIGENS_HYPOT_ULPS`.
    /// @pre `size <= hmeigens::maxMatrixSize`.
    /// @warning The function performs no overflow check itself. It assumes the given `size` always comes from a matrix, and that its square can be multiplied by `2*HMEIGENS_HYPOT_ULPS`.
    /// @sa HMEIGENS_HYPOT_ULPS
    /// @sa hmeigens::SquareMatrix::norm()
    inline constexpr std::uint64_t normTolerance(std::size_t size);
}

/* ----------------------------- */
/* -------- Definitions -------- */
/* ----------------------------- */
//
// See the declaration for all relevant information.
inline constexpr std::uint64_t hmeigens::normTolerance(std::size_t size) {
    // Squaring size is always safe if it comes from a matrix.
    // For all realistic cases, multiplying a squared size by 2*HMEIGENS_HYPOT_ULPS will never overflow.
    return HMEIGENS_HYPOT_ULPS*2*size*size;
}

#endif
