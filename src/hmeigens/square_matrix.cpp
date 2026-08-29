#include "hmeigens/square_matrix.hpp"
#include "hmeigens/constants.hpp"

#include <format>
#include <stdexcept>     // For std::invalid_argument, std::length_error

namespace hmeigens {

    namespace detail {
        // Helper function to validate the size of a matrix before constructing it. It rejects 0 and values that are too large to be represented or held by Container.
        std::size_t validateSize (std::size_t toValidate) {
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
            // The requested number of elements must be one detail::Container can actually hold.
            // This check must go after the previous, or squaring toValidate could overflow.
            if (toValidate * toValidate > detail::Container{}.max_size()) {
                throw std::length_error(std::format("A {0}x{0} matrix cannot be created.\n---> Number of elements ({1}) exceeds the maximum number of elements allowed ({2}).", toValidate, toValidate*toValidate, detail::Container{}.max_size()));
            }
            return toValidate;
        }
    }

    // Constructor of the SquareMatrix class.
    // Takes the size as a parameter, validates it via helper, then if everything's fine it initializes the matrix as a 0 filled Container whose length is size*size.
    // If there is a problem with the size, the helper function throws.
    SquareMatrix::SquareMatrix(std::size_t size) :
        // Validating HERE is important, because if it's done later, there could be an attempt to make a Container with an invalid size.
        size_(detail::validateSize(size)),
        // Using size_ for initialization makes it so that if the members are somehow swapped in the header, -Wuninitialized (i. e. -Wall) would complain.
        body_(size_*size_) {}
}
