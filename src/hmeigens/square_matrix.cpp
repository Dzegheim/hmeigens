#include "hmeigens/square_matrix.hpp"
#include "hmeigens/constants.hpp"
#include "hmeigens/detail/square_matrix_helpers.hpp"

#include <utility>     // For std::move
#include <cstddef>     // For std::size_t
#include <stdexcept>   // For std::out_of_range
#include <format>

// Constructor of the hmeigens::SquareMatrix class.
// Takes the size as a parameter, validates it via helper, then if everything's fine it initializes the matrix as a 0 filled hmeigens::SquareMatrix::Container whose length is size*size.
// If there is a problem with the size, the helper function throws.
hmeigens::SquareMatrix::SquareMatrix (std::size_t size) :
    // Validating HERE is important, because if it's done later, there could be an attempt to make a hmeigens::SquareMatrix::Container with an invalid size.
    size_(hmeigens::detail::validateSize(size)),
    // Using size_ for initialization makes it so that if the members are somehow swapped in the header, -Wuninitialized (i. e. -Wall) would complain.
    // The container may generate a std::bad_alloc. That is deliberately not handled here.
    body_(size_*size_) {}

// Constructor of the hmeigens::SquareMatrix class.
// Takes a size and a container object, and after validating the size and checking that the container is a square matrix-like object, uses it to fill the matrix.   
// If there is a problem with the size, the helper functions throw.
hmeigens::SquareMatrix::SquareMatrix (std::size_t size, hmeigens::SquareMatrix::Container&& body) :
    // Size must be validated before checking if it is appropriate for the given body, as it tests for size * size and could overflow otherwise.
    size_(
        hmeigens::detail::checkIfAppropriateSize(
            hmeigens::detail::validateSize(size),
            body.size()
        )
    ),
    // Here the hmeigens::SquareMatrix::Container is moved into body_. The container itself is not preserved, as its contents end up in the matrix.
    // Nothing that is meant to be kept must be passed here.
    body_(std::move(body)) {}

// Verifies that an index in within the matrix, throws if not.
// Checking for just the expression row * size_ + col < body_.size() could result in wrong indexation being accepted.
// Something like (0,8) for a 3x3 matrix would give the element in position (2,2), which is not what was asked, as what was asked makes no sense.
// Not that anyone writing this code would make that mistake and write a comment about it...
void hmeigens::SquareMatrix::checkIndex (std::size_t row, std::size_t col) const {
    if (row >= size_ or col >= size_) {
        throw std::out_of_range{std::format("Invalid index ({0},{1}) for {2}x{2} matrix. Please note matrices are 0-indexed.", row, col, size_)};
    }
    return;
}
