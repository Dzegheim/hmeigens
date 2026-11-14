// Source file for all functions in predicates.hpp.
// All functions are fully documented at their declarations.
// Definitions (hmeigens):
//     - isFinite(const SquareMatrix&);
//     - getTolerance(const SquareMatrix&);
//     - isUpperHessenberg(const SquareMatrix&, const Scalar);
//     - isUpperTriangular(const SquareMatrix&, const Scalar).
#include "hmeigens/predicates.hpp"
#include "hmeigens/constants.hpp"
#include "hmeigens/square_matrix.hpp"
#include "hmeigens/scalar.hpp"

#include <cmath>         // For std::isfinite
#include <cstddef>       // For std::size_t
#include <complex>       // For std::abs

bool hmeigens::isFinite(const hmeigens::SquareMatrix& matrix) {
    const std::size_t size = matrix.size();
    for (std::size_t row = 0; row < size; ++row) {
        for (std::size_t col = 0; col < size; ++col) {
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

hmeigens::Scalar hmeigens::getTolerance(const hmeigens::SquareMatrix& matrix) {
    const hmeigens::Scalar sizeSquared = static_cast<hmeigens::Scalar>(matrix.size()*matrix.size());
    return // For each element of the formula:
        hmeigens::epsilon*    // \varepsilon;
        hmeigens::toleranceK* // k;
        sizeSquared*          // n^2;
        matrix.norm();        // ||M||.
}

bool hmeigens::isUpperHessenberg(const hmeigens::SquareMatrix& matrix, const hmeigens::Scalar tolerance) {
    // The outer loop can start at 2 as this condition does not affect the first two rows.
    // This also means that 1x1 and 2x2 matrices return true trivially without any checks.
    for (std::size_t row = 2; row < matrix.size(); ++row) {
        // Pass through all sub-subdiagonal elements.
        for (std::size_t col = 0; col < row-1; ++col) {
            if (std::abs(matrix(row, col)) > tolerance) {
                return false;
            }
        }
    }
    return true;
}

bool hmeigens::isUpperTriangular(const hmeigens::SquareMatrix& matrix, const hmeigens::Scalar tolerance) {
    // Subdiagonal elements are manually checked, then hmeigens::isUpperHessenberg verifies the rest.
    // Here row can start at 1 as the first row does not affect the check.
    for (std::size_t row = 1; row < matrix.size(); ++row) {
        if (std::abs(matrix(row, row-1)) > tolerance) {
            return false;
        }
    }
    return hmeigens::isUpperHessenberg(matrix, tolerance);
}
