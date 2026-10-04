#include "hmeigens/square_matrix.hpp"
#include "hmeigens/detail/square_matrix_helpers.hpp"

#include <utility>     // For std::move
#include <cstddef>     // For std::size_t
#include <stdexcept>   // For std::out_of_range
#include <format>

// Constructor of the hmeigens::SquareMatrix class.
// Takes a container object, and after checking that the container is a non-empty square matrix-like object, uses it to fill the matrix.
// If there is a problem with the size, the helper function throws.
// Fully documented in .hpp.
hmeigens::SquareMatrix::SquareMatrix(hmeigens::SquareMatrix::Container&& body) :
    // Size must be checked before body is moved into the square matrix, as it needs to be a valid one.
    size_(hmeigens::detail::sizeFromBodyLength(body.size())),
    // Here the hmeigens::SquareMatrix::Container is moved into body_. The container itself is not preserved, as its contents end up in the matrix.
    // Nothing that is meant to be kept must be passed here.
    body_(std::move(body)) {}

// Gives the size of a matrix.
// Fully documented in .hpp.
std::size_t hmeigens::SquareMatrix::size() const {
    return size_;
}

// This and the three after are the accessors.
// Notable thing: The operators are index safe in DEBUG MODE ONLY.
// All fully documented in .hpp.
const hmeigens::Complex& hmeigens::SquareMatrix::operator()(std::size_t row, std::size_t col) const {
    #ifndef NDEBUG
    hmeigens::SquareMatrix::checkIndex(row, col);
    #endif
    return body_[hmeigens::SquareMatrix::getIndex(row, col)];
}

// See hmeigens::SquareMatrix::operator() const.
hmeigens::Complex& hmeigens::SquareMatrix::operator()(std::size_t row, std::size_t col) {
    #ifndef NDEBUG
    hmeigens::SquareMatrix::checkIndex(row, col);
    #endif
    return body_[hmeigens::SquareMatrix::getIndex(row, col)];
}

// See hmeigens::SquareMatrix::operator() const.
const hmeigens::Complex& hmeigens::SquareMatrix::at(std::size_t row, std::size_t col) const {
    hmeigens::SquareMatrix::checkIndex(row, col);
    return body_[hmeigens::SquareMatrix::getIndex(row, col)];
}

// See hmeigens::SquareMatrix::operator() const.
hmeigens::Complex& hmeigens::SquareMatrix::at(std::size_t row, std::size_t col) {
    hmeigens::SquareMatrix::checkIndex(row, col);
    return body_[hmeigens::SquareMatrix::getIndex(row, col)];
}

// Given a row and a column, gives the corresponding index for the body.
// Fully documented in .hpp.
std::size_t hmeigens::SquareMatrix::getIndex(std::size_t row, std::size_t col) const {
    return row * size_ + col;
}

// Verifies that an index in within the matrix, throws if not.
// Checking for just the expression row * size_ + col < body_.size() could result in wrong indexation being accepted.
// Something like (0,8) for a 3x3 matrix would give the element in position (2,2), which is not what was asked, as what was asked makes no sense.
// Not that anyone writing this code would make that mistake and write a comment about it...
void hmeigens::SquareMatrix::checkIndex(std::size_t row, std::size_t col) const {
    if (row >= size_ or col >= size_) {
        throw std::out_of_range{std::format("Invalid index ({0},{1}) for {2}x{2} matrix. Please note matrices are 0-indexed.", row, col, size_)};
    }
    return;
}
