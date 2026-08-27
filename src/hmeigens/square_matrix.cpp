#include "hmeigens/square_matrix.hpp"
#include "hmeigens/constants.hpp"

#include <string>
#include <stdexcept>     // For std::invalid_argument, std::runtime_error, std::length_error
#include <new>           // For std::bad_alloc
#include <exception>     // For std::throw_with_nested

namespace hmeigens {

    namespace detail {
        // Helper function to validate the size of a matrix before constructing it. It rejects 0 and values that are too large to be represented.
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
                throw std::length_error ("A matrix of size " + std::to_string(toValidate) + "x" + std::to_string(toValidate) + " cannot be created.\n---> Number of elements exceeds the maximum value allowed by std::size_t." );
            }
            // The requested number of elements must be one detail::Container can actually hold.
            // This check must go after the previous, or squaring toValidate could overflow.
            if (toValidate * toValidate > detail::Container{}.max_size()) {
                throw std::length_error ("A matrix of size " + std::to_string(toValidate) + "x" + std::to_string(toValidate) + " cannot be created.\n---> Value exceeds the maximum number of elements allowed." );
            }
            return toValidate;
        }
    }

    // Constructor of the SquareMatrix class.
    // Takes the size as a parameter, validates it via helper, then if everything's fine it initializes the matrix as a 0 filled Container whose length is size*size.
    // If there is a problem with the size, the helper function throws.
    // If Container throws it's not this code's fault.
    SquareMatrix::SquareMatrix(std::size_t size) try :
        // Validating HERE is important, because if it's done later, there could be an attempt to make a Container with an invalid size.
        size_(detail::validateSize(size)),
        // Using size_ for initialization makes it so that if the members are somehow swapped in the header, -Wuninitialized would complain.
        body_(size_*size_) {}
        // If the code gets here, the size was validated, but Container threw. This part informs the user why.
        catch (const std::bad_alloc&) {
            std::throw_with_nested(std::runtime_error("Could not allocate a matrix of size " + std::to_string(size) + "x" + std::to_string(size) + ".\n---> Not enough memory available."));
        }
}
