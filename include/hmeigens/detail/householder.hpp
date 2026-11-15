// File for functions that perform various parts of the Householder reflection.
// Declarations (hmeigens::detail):
//     - makeHouseholderInPlace(std::span<Complex>);
//     - applyLeftHouseholderInPlace(SquareMatrix&, std::span<const Complex>, const std::size_t).
#ifndef HMEIGENS_DETAIL_HOUSEHOLDER_HPP
#define HMEIGENS_DETAIL_HOUSEHOLDER_HPP

#include "hmeigens/scalar.hpp"
#include "hmeigens/square_matrix.hpp"

#include <span>
#include <cstddef>    // For std::size_t

/* ----------------------------- */
/* --------- IMPORTANT --------- */
/* ----------------------------- */
//
// The names of the values cited in the comments (u, v, H, phi, etc...) are those of the variables used in the documentation of hmeigens::makeUpperHessenbergInPlace in eigenvalues_solver.hpp.
// Note: in the algebra notation indices start at 1, in the code at 0.

namespace hmeigens::detail {

    // This function computes the Householder vector for a given vector of complex numbers.
    // Input must be the part of the column of a matrix below the diagonal.
    // Using the notation from the documentation of hmeigens::makeUpperHessenbergInPlace from eigenvalues_solver.hpp, the input vector is overwritten to become u/||u||, which can then be used to build the reflector.
    // The function returns the value to be written in the subdiagonal place of the matrix where vector was taken from, i.e. it returns phi*||v|| (if vector[0] is (0.; 0.), phi is (-1.; 0.)).
    // Note that the returned value is different from vector[0].
    // Important: vector must not be empty. No safety check is done before accessing it, and vector[0] is accessed directly, so it must hold at least 1 element. It must also be true that at least one of its elements is not (0.; 0.), as the function divides by its norm.
    [[nodiscard]] Complex makeHouseholderInPlace(std::span<Complex> vector);

    // This function applies a Householder matrix constructed from vector to matrix in place from the left, i.e. it computes the relevant portion of H*A.
    // Here vector is assumed to be u/||u|| as it is computed by hmeigens::detail::makeHouseholderInPlace for column col of matrix.
    // The algebraic formula equivalent to the operation is
    // H = I-2*u*u^dagger/||u||^2
    // but this function only edits the relevant components of the matrix, as column col is edited by hmeigens::makeUpperHessenbergInPlace.
    // Columns before col are assumed to be 0. below the subdiagonal.
    // Columns after col have their rows from col+1 onwards changed to be those of H*A.
    // Note: it must hold that vector.size() == matrix.size()-col-1. No safety checks are performed before accessing memory.
    void applyLeftHouseholderInPlace(SquareMatrix& matrix, std::span<const Complex> vector, const std::size_t col);
}

#endif
