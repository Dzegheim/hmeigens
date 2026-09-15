#include "hmeigens/square_matrix.hpp"
#include "hmeigens/constants.hpp"
#include "hmeigens/detail/square_matrix_helpers.hpp"


namespace hmeigens {
    // Constructor of the SquareMatrix class.
    // Takes the size as a parameter, validates it via helper, then if everything's fine it initializes the matrix as a 0 filled hmeigens::SquareMatrix::Container whose length is size*size.
    // If there is a problem with the size, the helper function throws.
    SquareMatrix::SquareMatrix (std::size_t size) :
        // Validating HERE is important, because if it's done later, there could be an attempt to make a hmeigens::SquareMatrix::Container with an invalid size.
        size_(detail::validateSize(size)),
        // Using size_ for initialization makes it so that if the members are somehow swapped in the header, -Wuninitialized (i. e. -Wall) would complain.
        // The container may generate a std::bad_alloc. That is deliberately not handled here.
        body_(size_*size_) {}
}
