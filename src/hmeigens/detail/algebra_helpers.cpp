// Source file for all functions in algebra_helpers.hpp.
// All functions are fully documented at their declarations.
// Definitions (hmeigens::detail):
//     - twoNorm(std::span<const Complex>);
//     - complexUnitVector(Complex).
#include "hmeigens/detail/algebra_helpers.hpp"
#include "hmeigens/scalar.hpp"

#include <span>
#include <cmath>     // For std::hypot

// This function is also used to compute the Frobenius norm of a matrix, as the Frobenius norm is just the 2-norm of a vector holding all the elements of the matrix.
// The implementation discussion below shows an alternative approach from when this code was used only to compute the Frobenius norm.
// ---------------------------------------------
// There exists an approach that runs over the matrix element by element and computes it as
//     hmeigens::Scalar norm = hmeigens::Scalar{0.};
//     for(auto z : body_) {
//         norm += std::norm(z);
//     }
//     return std::sqrt(norm);
// but if the sum of squares or even a single std::norm is too large, the entire norm results in inf, even if the square root would be small enough to fit into the result. Damn floating-points.
// An equivalent form is to use std::hypot. For 2 complex numbers z1 = (x1, y1), z2=(x2, y2), if X=hypot(x1, y1) = sqrt(x1^2+y1^2), then hypot(X, hypot(x2, y2)) = sqrt(X^2 + x2^2 + y2^2), which is the norm.
// ---------------------------------------------
// Of course std::hypot is slower, but this implementation cannot explode to infinity in the middle of the computation.
// The norm of a matrix needs to be computed only once in the triangularization process, so it being slower does not really matter.
// The 2-norm of a vector is O(n), and is computed O(n) times for each transformation of a matrix to upper Hessenberg, so this is an O(n^2) use in a process which is O(n^3). It being slower does not really matter.
hmeigens::Scalar hmeigens::detail::twoNorm(std::span<const hmeigens::Complex> vector) {
    hmeigens::Scalar norm = HMEIGENS_MAKE_SCALAR(0.);
    for(const auto& z : vector) {
        norm = std::hypot(std::hypot(z.real(), z.imag()), norm);
    }
    return norm;
}

hmeigens::Complex hmeigens::detail::complexUnitVector(hmeigens::Complex input) {
    // The only complex number that does not have a defined phase is exactly (0.; 0.), so here == is required.
    return input == hmeigens::Complex{} ?
        // Returning (1.; 0.) if the input is (0.; 0.) is arbitrary, but it is assumed in the rest of the code, so it needs to stay like this.
        hmeigens::Complex{
            HMEIGENS_MAKE_SCALAR(1.),
            HMEIGENS_MAKE_SCALAR(0.)
        } :
        // Note that using std::arg would not be useful here, as inputs with large differences in absolute value between components, i.e. close to one of the two axes, lose too much information.
        input/std::hypot(input.real(), input.imag());
}
