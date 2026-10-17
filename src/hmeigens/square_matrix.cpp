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

// Matrix multiplication.
// Fully documented in .hpp.
//
// The base algorithm would be, given P=L*R:
// P(i,j)=sum_k L(i,k)R(k,j),
// where i, j, k run in the range [0, size[.
// After benchmarking two design choices emerged.
// (1) benchmarks/matrix_multiplication_bench.cpp shows that the approach implemented below is the best performing of the three considered ones. That file contains a brief explanation of each.
// The approach chosen here is the one that allows for vectorization and cache access optimization.
// (2) benchmarks/complex_multiplication_bench.cpp shows that an explicitly written complex multiplication is faster than the standard operator* from <complex>.
// This is because the standard version may perform safety checks on the finiteness of numbers. Those checks are not needed here, as the finiteness of the matrix norm (and therefore of all of its elements) is a prerequisite this code will enforce in the eigenvalue search. Those checks, though not required, may happen, and slow the calculations down even if here they're not needed.
hmeigens::SquareMatrix hmeigens::operator* (const hmeigens::SquareMatrix& leftHand, const hmeigens::SquareMatrix& rightHand) {
    // Store the size for readability.
    const std::size_t size = leftHand.size();
    // Construct a zero-filled matrix after checking the sizes match.
    // If they don't, the helper throws std::invalid_argument.
    hmeigens::SquareMatrix product{
        hmeigens::detail::checkSizeMatch(
            size,
            rightHand.size(),
            "matrix multiplication"
        )
    };
    for (std::size_t i = 0; i < size; ++i) {
        for (std::size_t k = 0; k < size; ++k) {
            // Store the value of the left-hand matrix.
            // Two accesses per value of k, instead of 2*size ones if done inside the inner loop.
            const auto lTempRe = leftHand(i,k).real();
            const auto lTempIm = leftHand(i,k).imag();
            for (std::size_t j = 0; j < size; ++j) {
                // Store the values to access only two times rather than four.
                const auto rTempRe = rightHand(k,j).real();
                const auto rTempIm = rightHand(k,j).imag();
                product(i,j)+=hmeigens::Complex{
                    lTempRe*rTempRe-lTempIm*rTempIm,
                    lTempRe*rTempIm+lTempIm*rTempRe
                };
            }
        }
    }
    return product;
}
