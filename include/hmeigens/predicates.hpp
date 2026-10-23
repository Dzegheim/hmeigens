/// @file
/// @brief A collection of functions that verify the properties of a given matrix.
// Declarations (hmeigens):
//     - isFinite().
#ifndef HMEIGENS_PREDICATES_HPP
#define HMEIGENS_PREDICATES_HPP

#include "hmeigens/square_matrix.hpp"

namespace hmeigens {

    /// @brief Checks that all elements of the matrix satisfy `std::isfinite()` for both their real and imaginary parts.
    /// @param matrix The matrix to be checked.
    /// @return `true` if all elements of the matrix are finite, `false` otherwise.
    [[nodiscard]] bool isFinite(const SquareMatrix& matrix);
}

#endif
