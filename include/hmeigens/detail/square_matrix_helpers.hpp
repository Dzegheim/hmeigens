// File for functions used by hmeigens::SquareMatrix to validate parameters for the contstructors.

#ifndef HMEIGENS_DETAIL_SQUARE_MATRIX_HELPERS_HPP
#define HMEIGENS_DETAIL_SQUARE_MATRIX_HELPERS_HPP

#include "hmeigens/square_matrix.hpp"

namespace hmeigens {

    namespace detail {
        // Helper function to validate the size of a matrix before constructing it. It rejects 0 and values that are too large to be represented or held by hmeigens::SquareMatrix::Container.
        std::size_t validateSize (std::size_t toValidate);
    }
}

#endif
