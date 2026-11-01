// Source file for all functions in predicates.hpp.
// Definitions (hmeigens):
//     - isFinite();
//     - getTolerance().
#include "hmeigens/predicates.hpp"
#include "hmeigens/constants.hpp"
#include "hmeigens/square_matrix.hpp"

#include <cmath>         // For std::isfinite
#include <cstddef>       // For std::size_t

// Checks that all elements are finite.
// Fully documented in .hpp.
bool hmeigens::isFinite(const hmeigens::SquareMatrix& matrix) {
    // Readability.
    const std::size_t size = matrix.size();
    for (std::size_t row = 0; row < size; ++row) {
        for (std::size_t col = 0; col < size; ++col) {
            // Check that both the real and imaginary part are finite.
            if (
                not std::isfinite(matrix(row, col).real()) or
                not std::isfinite(matrix(row, col).imag())
            ) {
                // Even a single bad element is enough to return false.
                return false;
            }
        }
    }
    return true;
}

// Computes the tolerance.
// Fully documented in .hpp.
// See documentation for reasoning on the chosen operands.
hmeigens::Scalar hmeigens::getTolerance(const hmeigens::SquareMatrix& matrix) {
    // Get the squared size as an hmeigens::Scalar.
    const hmeigens::Scalar sizeSquared = static_cast<hmeigens::Scalar>(matrix.size()*matrix.size());
    return // For each element of the formula:
        hmeigens::epsilon*    // \varepsilon;
        hmeigens::toleranceK* // k;
        sizeSquared*          // n^2;
        matrix.norm();        // ||M||.
}
