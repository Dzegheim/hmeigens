// File for functions used by hmeigens::SquareMatrix to validate parameters for the contstructors.

#ifndef HMEIGENS_DETAIL_SQUARE_MATRIX_HELPERS_HPP
#define HMEIGENS_DETAIL_SQUARE_MATRIX_HELPERS_HPP

#include <cstddef>  // For std::size_t

namespace hmeigens::detail {
    // Helper function to validate the size of a matrix before constructing it. It rejects 0 and values that are too large to be represented or held by hmeigens::SquareMatrix::Container.
    std::size_t validateSize (std::size_t toValidate);

    // Helper function to verify that the size provided to the constructor is the square root of the length of the hmeigens::SquareMatrix::Container given as the other parameter.
    // The variable declaredSize is always assumed to already have been validate by hmeigens::detail::validateSize.
    // The variable containerSize is assumed to be the valid size of a hmeigens::SquareMatrix::Container.
    std::size_t checkIfAppropriateSize (std::size_t declaredSize, std::size_t containerSize);

    // Helper function that throws an exception, called by the member hmeigens::SquareMatrix::checkIndex.
    // Separated here otherwise hmeigens::SquareMatrix::checkIndex cannot be inlined.
    [[noreturn]] void throwIndexError (std::size_t row, std::size_t col, std::size_t size);
}

#endif
