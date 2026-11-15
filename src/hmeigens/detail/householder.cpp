// Source file for all functions in householder.hpp.
// All functions are fully documented at their declarations.
// Definitions (hmeigens::detail):
//     - makeHouseholderInPlace(std::span<Complex>);
//     - applyLeftHouseholderInPlace(SquareMatrix&, std::span<const Complex>, const std::size_t).
#include "hmeigens/detail/householder.hpp"
#include "hmeigens/scalar.hpp"
#include "hmeigens/square_matrix.hpp"
#include "hmeigens/detail/algebra_helpers.hpp"

#include <cmath>      // For std::hypot, std::sqrt
#include <cstddef>    // For std::size_t

// The names in the comments (u, phi, ...) match those of the documentation of hmeigens::makeUpperHessenbergInPlace() from eigenvalues_solver.hpp.
// The variable vector is turned from v to v/||v||, to u/||v||, to u/||u||.
hmeigens::Complex hmeigens::detail::makeHouseholderInPlace(std::span<hmeigens::Complex> vector) {
    const auto normV = hmeigens::detail::twoNorm(vector);
    // Normalizing here prevents the sum below from overflowing, as opposed to normalizing at the end.
    for (auto& element : vector) {
        element /= normV;
    }
    // Now vector holds v/||v||.
    // Compute the unit complex number associated with the first element of vector, i.e. -phi.
    // Here phi having opposite phase to vector[0] is extremely important. See section 5.1.3 Computing the Householder Vector of Matrix Computations (Golub & Van Loan, 4th edition, 2013), about the severe cancellation issue.
    const auto minusPhi = hmeigens::detail::complexUnitVector(vector[0]);
    // Finish the computation of u/||v||.
    // This guarantees that vector[0] != (0.; 0.):
    // - if it was zero before, the value of minusPhi is (1.; 0.) (see the function's documentation in algebra_helpers.hpp);
    // - if it was not zero, the two numbers have the same phase by construction, so they cannot cancel out.
    vector[0] += minusPhi;
    // Now vector is not a unit vector, as its norm is sqrt(2*|vector[0]|).
    // Compute the normalization constant, then renormalize the vector.
    const auto normalization =
        HMEIGENS_MAKE_SCALAR(1.)/
        std::sqrt(
            HMEIGENS_MAKE_SCALAR(2.)*
            std::hypot(vector[0].real(), vector[0].imag())
        );
    for (auto& element : vector) {
        element *= normalization;
    }
    // Now vector holds u/||u||.
    // The value to be written in the matrix needs the sign change to be phi*||v||.
    return -minusPhi*normV;
}

// The products of complex numbers are done manually rather than with std::operator*. See the explanation at hmeigens::operator* in square_matrix.cpp for details.
void hmeigens::detail::applyLeftHouseholderInPlace(hmeigens::SquareMatrix& matrix, std::span<const hmeigens::Complex> vector, const std::size_t col) {
    for (std::size_t otherCol = col+1; otherCol < matrix.size(); ++otherCol) {
        hmeigens::Scalar scalarProductRe{};
        hmeigens::Scalar scalarProductIm{};
        // Compute the scalar product of the conjugate of vector and the matrix's column.
        for (std::size_t i = 0; i < vector.size(); ++i) {
            const auto vectorElRe = vector[i].real();
            const auto vectorElImConj = -vector[i].imag();
            const auto matrixElRe = matrix(col+1+i, otherCol).real();
            const auto matrixElIm = matrix(col+1+i, otherCol).imag();
            scalarProductRe += vectorElRe*matrixElRe-vectorElImConj*matrixElIm;
            scalarProductIm += vectorElRe*matrixElIm+vectorElImConj*matrixElRe;
        }
        // Edit the matrix by subtracting 2u times the scalar product.
        for (std::size_t i = 0; i < vector.size(); ++i) {
            const auto vectorElRe = vector[i].real();
            const auto vectorElIm = vector[i].imag();
            matrix(col+1+i, otherCol) -= hmeigens::Complex{
                HMEIGENS_MAKE_SCALAR(2.)*(scalarProductRe*vectorElRe-scalarProductIm*vectorElIm),
                HMEIGENS_MAKE_SCALAR(2.)*(scalarProductIm*vectorElRe+scalarProductRe*vectorElIm)
            };
        }
    }
    return;
}
