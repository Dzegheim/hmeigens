#include "hmeigens/constants.hpp"
#include "hmeigens/square_matrix.hpp"
#include "hmeigens/detail/square_matrix_helpers.hpp"
#include "hmeigens/detail/isqrt.hpp"

#include <format>
#include <stdexcept>     // For std::invalid_argument, std::length_error
#include <cstddef>       // For std::size_t
#include <cstdint>       // For std::intmax_t, std::uintmax_t, SIZE_MAX, UINTMAX_MAX
#include <utility>       // For std::cmp_greater
#include <algorithm>     // For std::min

#if SIZE_MAX < UINTMAX_MAX
// If this does not hold the only function using it doesn't exist.
#include <limits>        // For std::numeric_limits<>::max()
#endif

/* ------------------------------*/
/* --------- IMPORTANT --------- */
/* ------------------------------*/
//
// For comments about the design logic, see square_matrix_helpers.hpp.

std::size_t hmeigens::detail::validateSize(std::size_t toValidate) {
    // A matrix must have a positive integer as a size.
    // A 0 size has no mathematical meaning.
    // Throws std::invalid_argument because the user provided an invalid argument to the constructor.
    if (toValidate == 0) {
        throw std::invalid_argument{"Size 0 is invalid for a matrix."};
    }
    // The value toValidate * toValidate must be able to be represented by a std::size_t.
    // It must also hold that the requested number of elements must be one hmeigens::SquareMatrix::Container can actually hold.
    // The variable hmeigens::maxMatrixSize is constructed so that it always satisfies both conditions.
    if (toValidate > hmeigens::maxMatrixSize) {
        throw std::length_error{std::format("A {0}x{0} matrix cannot be created.\n---> Number of elements would exceed the maximum allowed amount.\n---> Note that the maximum size allowed for a matrix with the current build settings is {1}.", toValidate, hmeigens::maxMatrixSize)};
    }
    return toValidate;
}

std::size_t hmeigens::detail::validateSize(std::intmax_t toValidate) {
    // A matrix must have a positive integer as a size.
    // This function checks that the size is non-negative.
    // The other checks are handled by the unsigned overloads.
    // Note that the unsigned validator can be the std::size_t overload. See the comment at the top of the square_matrix_helpers.hpp file.
    if (toValidate < 0) {
        throw std::invalid_argument{std::format("A matrix cannot have a negative size.\n---> Provided value: {0}.", toValidate)};
    }
    // If the size is not negative it can always be converted safely into an std::uintmax_t.
    // The unsigned validator then performs its own checks.
    // In the case in which std::size_t is as wide as std::uintmax_t, this correctly passes to the std::size_t overload.
    return hmeigens::detail::validateSize(static_cast<std::uintmax_t>(toValidate));
}

#if SIZE_MAX < UINTMAX_MAX
// See the comment to the declaration in square_matrix_helpers.hpp for the preprocessor #if.
std::size_t hmeigens::detail::validateSize(std::uintmax_t toValidate) {
    // Check that the value fits into an std::size_t.
    if (std::cmp_greater(toValidate, std::numeric_limits<std::size_t>::max())) {
        throw std::length_error{
            // Inform the user of the error of their ways: they're trying to give a size that does not fit into std::size_t, and that also makes it certain that it will not be valid because a valid size must be both squarable within std::size_t and smaller than the maximum allowed number of elements of the container, which is a std::size_t.
            // Basically this value makes no sense in so many different ways that the diagnostic is a favour.
            std::format(
                "A {0}x{0} matrix cannot be created.\n---> Number of elements is too large to be represented by std::size_t. The maximum value that can be represented by the type is {1}.\n---> Note that the maximum size allowed for a matrix with the current build settings is {2}.",
                toValidate,
                std::numeric_limits<std::size_t>::max(),
                hmeigens::maxMatrixSize
            )
        };
    }
    // If the size is not too wide, it can be converted safely into an std::size_t and validated by the appropriate overload.
    return hmeigens::detail::validateSize(static_cast<std::size_t>(toValidate));
}
#endif

std::size_t hmeigens::detail::sizeFromBodyLength(std::size_t bodyLength) {
    // An empty container cannot be used to construct a matrix because it would have size 0.
    if (bodyLength == 0) {
        throw std::invalid_argument{"Cannot construct a matrix from an empty container."};
    }
    const std::size_t root = hmeigens::detail::isqrt(bodyLength);
    // A matrix needs to be square, if the body's size is not a perfect square it cannot be mapped appropriately to a square matrix.
    if (root * root != bodyLength) {
        throw std::invalid_argument{std::format("The provided container does not represent a square matrix.\n---> Number of elements {0} is not a perfect square.", bodyLength)};
    }
    return root;
}
