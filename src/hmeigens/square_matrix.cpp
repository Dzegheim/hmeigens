// Source file for all non-template functions in square_matrix.hpp.
// Contents:
// - hmeigens::SquareMatrix::SquareMatrix(hmeigens::SquareMatrix::Container&&);
// - hmeigens::SquareMatrix::norm() const.
#include "hmeigens/square_matrix.hpp"
#include "hmeigens/detail/square_matrix_helpers.hpp"

#include <utility>     // For std::move
#include <cstddef>     // For std::size_t
#include <cmath>       // For std::hypot
#include <complex>     // For std::conj

// Constructor of the hmeigens::SquareMatrix class.
// Takes a container object, and after checking that the container is a non-empty square matrix-like object, uses it to fill the matrix.
// If there is a problem with the body's size(), the helper function throws.
// Fully documented in .hpp.
hmeigens::SquareMatrix::SquareMatrix(hmeigens::SquareMatrix::Container&& body) :
    // Size must be checked before body is moved into the square matrix, as it needs to be a valid one.
    size_(hmeigens::detail::sizeFromBodyLength(body.size())),
    // Here the hmeigens::SquareMatrix::Container is moved into body_. The container itself is not preserved, as its contents end up in the matrix.
    // Nothing that is meant to be kept must be passed here.
    body_(std::move(body)) {}

// Frobenius norm of a matrix.
// Fully documented in .hpp.
//
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
// This call of std::hypot is equivalent to the definition of the Frobenius norm.
// Of course std::hypot is slower, but this implementation cannot explode to infinity in the middle of the computation.
// The norm needs to be computed only once in the triangularization process, so it being slower does not really matter.
hmeigens::Scalar hmeigens::SquareMatrix::norm() const {
    // Initialize the norm to 0.
    hmeigens::Scalar norm = HMEIGENS_MAKE_SCALAR(0.);
    // The body_ is just an hmeigens::SquareMatrix::Container. The order of the elements here does not matter.
    for(const auto& z : body_) {
        norm = std::hypot(std::hypot(z.real(), z.imag()), norm);
    }
    return norm;
}

// In-place adjoint of the matrix.
// Fully documented in .hpp.
hmeigens::SquareMatrix& hmeigens::SquareMatrix::adjointInPlace() {
    for (std::size_t i = 0; i < size(); ++i) {
        // Conjugate diagonal elements.
        (*this)(i,i) = std::conj((*this)(i,i));
        // Pass through all subdiagonal elements.
        for (std::size_t j = 0; j < i; ++j) {
            // Store the first one and conjugate it.
            auto temp = std::conj((*this)(i, j));
            // Overwrite the first one with the second one's conjugate.
            (*this)(i, j) = std::conj((*this)(j, i));
            // Overwrite the second one with the stored temporary.
            (*this)(j, i) = temp;
        }
    }
    return *this;
}

// Adjoint of the matrix.
// Fully documented in .hpp.
hmeigens::SquareMatrix hmeigens::SquareMatrix::adjoint() const {
    // These lines are split to allow for the local to be returned by name, so the compiler can perform NRVO and not copy twice.
    hmeigens::SquareMatrix adj = *this;
    adj.adjointInPlace();
    return adj;
}
