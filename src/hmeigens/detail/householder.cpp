// Source file for all functions in householder.hpp.
// Definitions (hmeigens::detail):
//     - makeHouseholderInPlace(std::span<Complex>).
#include "hmeigens/detail/householder.hpp"
#include "hmeigens/scalar.hpp"
#include "hmeigens/square_matrix.hpp"
#include "hmeigens/detail/algebra_helpers.hpp"

#include <cmath>    // For std::hypot, std::sqrt

// Changes the input into the Householder vector, and computes the element to be put in the matrix's subdiagonal element for a given subdiagonal part of a column.
// The names in the comments (u, phi, ...) match those of the documentation of hmeigens::makeUpperHessenbergInPlace() from eigenvalues_solver.hpp.
// The variable vector is turned from v to v/||v||, to u/||v||, to u/||u||.
// Fully documented in .hpp.
hmeigens::Complex hmeigens::detail::makeHouseholderInPlace(std::span<hmeigens::Complex> vector) {
    // Compute the 2-norm of vector.
    const auto normV = hmeigens::detail::twoNorm(vector);
    // Normalize the vector.
    // This prevents the sum below from overflowing, as opposed to normalizing at the end.
    for (auto& element : vector) {
        element /= normV;
    }
    // Now vector holds v/||v||.
    // Compute the unit complex number associated with the first element of vector.
    // The computed value is -phi.
    // here phi having opposite phase to vector[0] is extremely important. See section 5.1.3 Computing the Householder Vector of Matrix Computations (Golub & Van Loan, 4th edition, 2013), about the severe cancellation issue.
    // This is needed to compute u/||v||.
    const auto minusPhi = hmeigens::detail::complexUnitVector(vector[0]);
    // Finish the computation of u/||v||.
    // This guarantees that vector[0] != (0.; 0.):
    // - if it was zero before, the value of minusPhi is (1.; 0.) (see the function's source in algebra_helpers.cpp);
    // - if it was not zero, the two numbers have the same phase by construction, so they cannot cancel out.
    vector[0] += minusPhi;
    // Now vector is not a unit vector, as its norm is sqrt(2*|vector[0]|).
    // Compute the normalization constant.
    const auto normalization =
        HMEIGENS_MAKE_SCALAR(1.)/
        std::sqrt(
            HMEIGENS_MAKE_SCALAR(2.)*
            std::hypot(vector[0].real(), vector[0].imag())
        );
    // Renormalize the vector.
    for (auto& element : vector) {
        element *= normalization;
    }
    // Now vector holds u/||u||.
    // Return the value to be written in the matrix.
    // Needs the sign change to be phi*||v||.
    return -minusPhi*normV;
}
