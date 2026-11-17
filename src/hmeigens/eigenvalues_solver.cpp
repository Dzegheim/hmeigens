// Source file for all functions in eigenvalues_solver.hpp.
// All functions are fully documented at their declarations.
// Definitions (hmeigens):
//     - makeUpperHessenbergInPlace(SquareMatrix&);
//     - makeUpperHessenberg(SquareMatrix).
#include "hmeigens/eigenvalues_solver.hpp"
#include "hmeigens/square_matrix.hpp"
#include "hmeigens/scalar.hpp"
#include "hmeigens/detail/householder.hpp"

#include <algorithm>      // For std::ranges::all_of
#include <span>
#include <cstddef>        // For std::size_t

hmeigens::SquareMatrix& hmeigens::makeUpperHessenbergInPlace(hmeigens::SquareMatrix& matrix) {
    const std::size_t size = matrix.size();
    // Here size-1 is the largest size the vector needs. Allocated once.
    hmeigens::SquareMatrix::Container storage(size-1);
    // Writing col < size-2 would wrap if size is 1.
    for (std::size_t col = 0; col+2 < size; ++col) {
        // Reflection involves size-col-1 elements.
        const std::size_t amount = size-col-1;
        // Get a view over storage, to avoid allocating memory multiple times.
        std::span vector = std::span{storage}.first(amount);
        // Store the first element outside of the loop, as the other elements will be rewritten as 0., so this will either be:
        // - kept as is, if the rest of the column is 0., as it is the correct value;
        // - rewritten later by hmeigens::detail::makeHouseholderInPlace.
        vector[0] = matrix(col+1, col);
        for (std::size_t i = 1; i < amount; ++i) {
            vector[i] = matrix(col+1+i, col);
            // All the elements besides the first will become 0. anyway, so this saves a step later.
            // If the column is already 0., this is harmless.
            matrix(col+1+i, col) = HMEIGENS_MAKE_SCALAR(0.);
        }
        // If all the elements of the column except the first are already 0., no operation needs to be performed, as the column is already ok.
        if (std::ranges::all_of(
                vector.subspan(1),
                [](const hmeigens::Complex& z) {
                    // Here checking for exactly 0. is warranted.
                    // The values in the matrix have already been changed to 0., so skipping small numbers here would lose them.
                    // Even a small number still requires being processed, as this will affect vector[0] and matrix.
                    return z == hmeigens::Complex{};
                }
            )
        ) {
            continue;
        }
        // Overwrite the subdiagonal element with the appropriate value.
        // The function also modifies vector, changing it into u/||u||.
        // This simplifies the computations for the application of the reflectors, and prevents overflows or underflows due to ||u||^2.
        matrix(col+1, col) = hmeigens::detail::makeHouseholderInPlace(vector);
        // The matrix is edited in place by the applications.
        hmeigens::detail::applyLeftHouseholderInPlace(matrix, vector, col);
        hmeigens::detail::applyRightHouseholderInPlace(matrix, vector, col);
    }
    return matrix;
}

hmeigens::SquareMatrix hmeigens::makeUpperHessenberg(hmeigens::SquareMatrix matrix) {
    // These lines are split to allow for the parameter to be returned by name, so the compiler can move it rather than copying it again.
    hmeigens::makeUpperHessenbergInPlace(matrix);
    return matrix;
}
