// File for functions that perform various parts of the Householder reflection.
// Declarations (hmeigens::detail):
//     - makeHouseholderInPlace(std::span<Complex>).
#ifndef HMEIGENS_DETAIL_HOUSEHOLDER_HPP
#define HMEIGENS_DETAIL_HOUSEHOLDER_HPP

#include "hmeigens/scalar.hpp"
#include "hmeigens/square_matrix.hpp"

#include <span>

namespace hmeigens::detail {

    // This function computes the Householder vector for a given vector of complex numbers.
    // Input must be the part of the column of a matrix below the diagonal.
    // Using the notation from the documentation of hmeigens::makeUpperHessenbergInPlace from eigenvalues_solver.hpp, the input vector is overwritten to become u/||u||, which can then be used to build the reflector.
    // The function returns the value to be written in the subdiagonal place of the matrix where vector was taken from, i.e. it returns phi*||v|| (if vector[0] is (0.; 0.), phi is (-1.; 0.)).
    // Note that the returned value is different from vector[0].
    // Important: vector must not be empty. No safety check is done before accessing it, and vector[0] is accessed directly, so it must hold at least 1 element. It must also be true that at least one of its elements is not (0.; 0.), as the function divides by its norm.
    [[nodiscard]] Complex makeHouseholderInPlace(std::span<Complex> vector);
}

#endif
