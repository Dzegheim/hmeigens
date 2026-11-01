/// @file
/// @brief Constants used throughout the project.
// Declarations:
// - HMEIGENS_TOLERANCE_K.
// Declarations (hmeigens):
//     - epsilon;
//     - maxSquarableSize;
//     - toleranceK.
#ifndef HMEIGENS_CONSTANTS_HPP
#define HMEIGENS_CONSTANTS_HPP

#include "hmeigens/scalar.hpp"

#include <limits>
#include <cstddef>  // For std::size_t

#ifndef HMEIGENS_TOLERANCE_K
/// @brief **TEMPORARY VALUE. This value is meaningless until the project is completed and successfully benchmarked.** Multiplicative constant for the function `hmeigens::getTolerance`.
///
/// This value is a multiplicative constant used to adjust the tolerance of the checks enforced in the file `predicates.hpp`.
/// The current value of `1.` is temporary, and it is meant to be measured by the benchmarks, but this cannot be done until the solver is completed.
/// This entry will contain information on the benchmarks, when available.
/// See `hmeigens::getTolerance` for an extensive discussion on the shape of the tolerance. This constant engulfs the elements from the theory that are not tied to the matrix:
/// - the `2` factor between \f(\varepsilon\f) and \f(\mathbf{u}\f);
/// - the \f(c\f) *small constant*.
/// @warning This is a temporary value pending benchmarking.
/// @sa hmeigens::toleranceK
/// @sa hmeigens::getTolerance(const hmeigens::SquareMatrix &)
#define HMEIGENS_TOLERANCE_K 1.
#endif

namespace hmeigens {

    /// @brief The difference between `1.0` and the next `hmeigens::Scalar`.
    ///
    /// [Values](https://en.cppreference.com/cpp/types/climits):
    /// - `1.19209e-07` for `float`;
    /// - `2.22045e-16` for `double`.
    inline constexpr Scalar epsilon = std::numeric_limits<Scalar>::epsilon();

    /// @brief The maximum squarable value within `std::size_t`.
    ///
    /// Let \f(b\f) be the number of bits of `std::size_t`, then the expression \f(2^{b/2}-1\f) is the largest (unsigned) integer that can be squared without overflowing.
    inline constexpr std::size_t maxSquarableSize = (std::size_t{1} << (std::numeric_limits<std::size_t>::digits/2))-1;
    // These two assertions verify that the operation above behaved properly.
    static_assert(maxSquarableSize <= std::numeric_limits<std::size_t>::max()/maxSquarableSize, "The variable hmeigens::maxSquarableSize cannot overflow when squared.");
    static_assert(maxSquarableSize+1 > std::numeric_limits<std::size_t>::max()/(maxSquarableSize+1), "The variable hmeigens::maxSquarableSize must be the largest possible value that does not overflow when squared.");

    /// @brief The variable `HMEIGENS_TOLERANCE_K` converted to `hmeigens::Scalar`.
    /// @sa HMEIGENS_TOLERANCE_K
    inline constexpr Scalar toleranceK = static_cast<Scalar>(HMEIGENS_TOLERANCE_K);
}

#endif
