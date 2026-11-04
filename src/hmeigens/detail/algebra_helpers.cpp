// Source file for all functions in algebra_helpers.hpp.
// Definitions (hmeigens::detail):
//     - twoNorm(std::span<const Complex>);
//     - complexUnitVector(Complex).
#include "hmeigens/detail/algebra_helpers.hpp"
#include "hmeigens/scalar.hpp"

#include <span>
#include <cmath>     // For std::hypot

// Euclidean 2-norm of a vector.
//
// This function is also used to compute the Frobenius norm of a matrix, as the Frobenius norm is just the 2-norm of a vector holding all the elements of the matrix.
// The implementation discussion below discusses an alternative approach from when this code was used only to compute the Frobenius norm.
// ---------------------------------------------
// There exists an approach that runs over the matrix element by element and computes it as
//     hmeigens::Scalar norm = hmeigens::Scalar{0.};
//     for(auto z : body_) {
//         norm += std::norm(z);
//     }
//     return std::sqrt(norm);
// but if the sum of squares or even a single std::norm is too large, the entire norm results in inf, even if the square root would be small enough to fit into the result. Damn floating-points.
// An equivalent form is to use std::hypot. It can be proven by induction that the two forms are algebraically equivalent.
// ---- Statement ----
// Let x_1,...,x_n be real numbers.
// Let f_0 = 0, f_k = sqrt(x_k^2 + f_(k-1)^2).
// [i.e. in the code the equivalent of calling std::hypot(std::hypot(z.real(), z.imag()), norm).]
// Then for all k in 1,...,n f_k=sqrt(sum_(i=1)^k x_i^2).
// ---- Proof ----
// Case 1 given 0:
// f_1 = sqrt(x_1^2 + f_0^2) = sqrt(x_1^2 + 0) = sqrt(x_1^2).
//
// Case k given k-1:
// f_k = sqrt(x_k^2 + f_(k-1)^2).
// Given f_(k-1) = sqrt(sum_(i=1)^(k-1) x_i^2), since x_i^2 >= 0, then
// f_(k-1)^2 = sqrt(sum_(i=1)^(k-1) x_i^2)^2 = sum_(i=1)^(k-1) x_i^2, and thus:
// f_k = sqrt(x_k^2 + sum_(i=1)^(k-1) x_i^2) = sqrt(sum_(i=1)^(k) x_i^2).
// ---- Q.E.D. ----
// This call of std::hypot is equivalent to the definition of the 2-norm, which is equivalent to that of the Frobenius norm.
// ---------------------------------------------
// Of course std::hypot is slower, but this implementation cannot explode to infinity in the middle of the computation.
// The norm of a matrix needs to be computed only once in the triangularization process, so it being slower does not really matter.
// The 2-norm of a vector is O(n), and is computed O(n) times for each transformation of a matrix to upper Hessenberg, so this is an O(n^2) use in a process which is O(n^3), so again it being slower does not really matter.
hmeigens::Scalar hmeigens::detail::twoNorm(std::span<const hmeigens::Complex> vector) {
    // Initialize the norm to 0.
    hmeigens::Scalar norm = HMEIGENS_MAKE_SCALAR(0.);
    for(const auto& z : vector) {
        norm = std::hypot(std::hypot(z.real(), z.imag()), norm);
    }
    return norm;
}

// Computes the unit length vector in the direction of input, unless the input is (0.; 0.), in which case it returns (1.; 0.).
// Fully documented in .hpp.
hmeigens::Complex hmeigens::detail::complexUnitVector(hmeigens::Complex input) {
    // The only complex number that does not have a defined phase is exactly (0.; 0.), so here == is required.
    return input == hmeigens::Complex{} ?
        // Return (1.; 0.) if the input is (0.; 0.).
        hmeigens::Complex{
            HMEIGENS_MAKE_SCALAR(1.),
            HMEIGENS_MAKE_SCALAR(0.)
        } :
        // Build the unit vector otherwise.
        // Note that using std::arg would not be useful here, as inputs with large differences in absolute value between components, i.e. close to one of the two axes, lose too much information.
        input/std::hypot(input.real(), input.imag());
}
