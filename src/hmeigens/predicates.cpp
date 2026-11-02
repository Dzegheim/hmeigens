// Source file for all functions in predicates.hpp.
// Definitions (hmeigens):
//     - isFinite(const SquareMatrix&);
//     - getTolerance(const SquareMatrix&);
//     - isUpperHessenberg(const SquareMatrix&, const Scalar);
//     - isUpperTriangular(const SquareMatrix&, const Scalar).
#include "hmeigens/predicates.hpp"
#include "hmeigens/constants.hpp"
#include "hmeigens/square_matrix.hpp"

#include <cmath>         // For std::isfinite
#include <cstddef>       // For std::size_t
#include <complex>       // For std::abs

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

// Checks whether a matrix is in upper Hessenberg form.
// Fully documented in .hpp.
bool hmeigens::isUpperHessenberg(const hmeigens::SquareMatrix& matrix, const hmeigens::Scalar tolerance) {
    // Readability.
    const std::size_t size = matrix.size();
    // Checks that all sub-subdiagonal elements are 0. within the tolerance.
    // The outer loop can start at 2 as this condition does not affect the first two rows.
    // This also means that 1x1 and 2x2 matrices return true trivially without any checks.
    for (std::size_t i = 2; i < size; ++i) {
        // Pass through all sub-subdiagonal elements.
        for (std::size_t j = 0; j < i-1; ++j) {
            if (std::abs(matrix(i,j)) > tolerance) {
                return false;
            }
        }
    }
    return true;
}

// Checks whether a matrix is in upper triangular form.
// Fully documented in .hpp.
bool hmeigens::isUpperTriangular(const hmeigens::SquareMatrix& matrix, const hmeigens::Scalar tolerance) {
    // Readability.
    const std::size_t size = matrix.size();
    // Checks that all elements below the diagonal are 0. within the tolerance.
    // Subdiagonal elements are manually checked, then hmeigens::isUpperHessenberg verifies the rest.
    // Here i can start at 1 as the first row does not affect the check.
    for (std::size_t i = 1; i < size; ++i) {
        if (std::abs(matrix(i,i-1)) > tolerance) {
            return false;
        }
    }
    return hmeigens::isUpperHessenberg(matrix, tolerance);
}
