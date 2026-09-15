#include "hmeigens/constants.hpp"
#include "hmeigens/square_matrix.hpp"
#include "hmeigens/detail/square_matrix_helpers.hpp"

#include <format>
#include <stdexcept>     // For std::invalid_argument, std::length_error

std::size_t hmeigens::detail::validateSize (std::size_t toValidate) {
    // A matrix must have a positive integer as a size.
    // A 0 size has no mathematical meaning.
    // Throws std::invalid_argument because the user provided an invalid argument to the constructor.
    if (toValidate == 0) {
        throw std::invalid_argument ("Size 0 is invalid for a matrix.");
    }
    // The value toValidate * toValidate must be able to be represented by a std::size_t.
    // The maxMatrixSize is constructed so that it is the largest possible number that can be squared and still not make std::size_t overflow.
    if (toValidate > hmeigens::maxMatrixSize) {
        throw std::length_error(std::format("A {0}x{0} matrix cannot be created.\n---> Number of elements exceeds the maximum value allowed by std::size_t. The maximum allowed size is {1}.", toValidate, hmeigens::maxMatrixSize));
    }
    // The requested number of elements must be one Container can actually hold.
    // This check must go after the previous, or squaring toValidate could overflow.
    if (toValidate * toValidate > hmeigens::SquareMatrix::Container{}.max_size()) {
        throw std::length_error(std::format("A {0}x{0} matrix cannot be created.\n---> Number of elements ({1}) exceeds the maximum number of elements allowed ({2}).", toValidate, toValidate*toValidate, hmeigens::SquareMatrix::Container{}.max_size()));
    }
    return toValidate;
}

