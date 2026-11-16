// Source file for all non-template and non-inline functions in square_matrix.hpp.
// All functions are fully documented at their declarations.
// Definitions (hmeigens::SquareMatrix):
//     - SquareMatrix(Container&&);
//     - norm() const;
//     - adjointInPlace();
//     - adjoint() const.
// Definitions (hmeigens):
//     - operator*(const SquareMatrix&, const SquareMatrix&).
#include "hmeigens/square_matrix.hpp"
#include "hmeigens/detail/square_matrix_helpers.hpp"
#include "hmeigens/detail/algebra_helpers.hpp"

#include <utility>     // For std::move
#include <cstddef>     // For std::size_t
#include <complex>     // For std::conj

hmeigens::SquareMatrix::SquareMatrix(hmeigens::SquareMatrix::Container&& body) :
    // Size must be checked before body is moved into the square matrix, as it needs to be a valid one.
    size_(hmeigens::detail::sizeFromBodyLength(body.size())),
    body_(std::move(body)) {}

hmeigens::Scalar hmeigens::SquareMatrix::norm() const {
    // The Frobenius norm is equivalent to the 2-norm of a vector holding all the elements of the matrix.
    // The body_ is just an hmeigens::SquareMatrix::Container. The order of the elements here does not matter.
    return hmeigens::detail::twoNorm(body_);
}

hmeigens::SquareMatrix& hmeigens::SquareMatrix::adjointInPlace() {
    for (std::size_t i = 0; i < size(); ++i) {
        (*this)(i,i) = std::conj((*this)(i,i));
        for (std::size_t j = 0; j < i; ++j) {
            auto temp = std::conj((*this)(i, j));
            (*this)(i, j) = std::conj((*this)(j, i));
            (*this)(j, i) = temp;
        }
    }
    return *this;
}

hmeigens::SquareMatrix hmeigens::SquareMatrix::adjoint() const {
    // These lines are split to allow for the local to be returned by name, so the compiler can perform NRVO and not copy twice.
    hmeigens::SquareMatrix adj = *this;
    adj.adjointInPlace();
    return adj;
}

// The base algorithm would be, given P=L*R:
// P(i,j)=sum_k L(i,k)R(k,j),
// where i, j, k run in the range [0, size[.
// After benchmarking two design choices emerged.
// (1) benchmarks/matrix_multiplication_bench.cpp shows that the approach implemented below is the best performing of the three considered ones. That file contains a brief explanation of each.
// The approach chosen here is the one that allows for vectorization and cache access optimization.
// (2) See hmeigens::detail::product in algebra_helpers.hpp.
hmeigens::SquareMatrix hmeigens::operator* (const hmeigens::SquareMatrix& leftHand, const hmeigens::SquareMatrix& rightHand) {
    const std::size_t size = leftHand.size();
    // Construct a zero-filled matrix after checking the sizes match.
    hmeigens::SquareMatrix productMatrix{
        hmeigens::detail::checkSizeMatch(
            size,
            rightHand.size(),
            "matrix multiplication"
        )
    };
    for (std::size_t i = 0; i < size; ++i) {
        for (std::size_t k = 0; k < size; ++k) {
            // One access per value of k, instead of size ones if done inside the inner loop.
            const auto lTemp = leftHand(i,k);
            for (std::size_t j = 0; j < size; ++j) {
                productMatrix(i,j)+=hmeigens::detail::product(lTemp, rightHand(k,j));
            }
        }
    }
    return productMatrix;
}
