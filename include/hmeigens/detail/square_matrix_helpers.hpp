// File for functions used by hmeigens::SquareMatrix to validate parameters for the constructors.

#ifndef HMEIGENS_DETAIL_SQUARE_MATRIX_HELPERS_HPP
#define HMEIGENS_DETAIL_SQUARE_MATRIX_HELPERS_HPP

#include <cstddef>    // For std::size_t
#include <concepts>   // For std::signed_integral

namespace hmeigens::detail {
    // Helper function to validate the size of a matrix before constructing it.
    // It rejects 0 and values that are too large to be represented or held by hmeigens::SquareMatrix::Container.
    // A number that reaches this function is assumed non-negative. Negative values go through the template overload below.
    // If the size is 0, it throws std::invalid_argument.
    // If the size it too large it throws std::length_error.
    [[nodiscard]] std::size_t validateSize (std::size_t toValidate);

    // Helper function that validates the sign of a size, to avoid a negative number wrapping std::size_t and giving the wrong exception.
    // A negative size must be an std::invalid_argument, not an std::length_error, which happens for a wrapped value.
    // This function checks if the size is negative, throws if it is, and casts to std::size_t and then calls the unsigned validator if not.
    [[nodiscard]] std::size_t validateSize (long long int toValidate);

    // Helper function needed to allow for signed values to be processed correctly.
    // This needs to be a template, as having just a simple overload with long long int does not compile, because it gives, for example,
    // > error: call of overloaded 'validateSize(int)' is ambiguous
    // if a value is not explicitly a long long int or a std::size_t.
    // Now all the following cases work:
    // - validateSize(1) // -> template -> int;
    // - validateSize(1ll) // -> non-template -> long long int;
    // - validateSize(std::size_t{1}) // non-template -> std::size_t.
    template <std::signed_integral SigInt>
    [[nodiscard]] std::size_t validateSize (SigInt toValidate);

    // Helper function to verify that the size provided to the constructor, when squared, is equal to the value of the other parameter.
    // The variable declaredSize is always assumed to already have been validated by hmeigens::detail::validateSize.
    // The variable containerSize is assumed to be the valid size of the hmeigens::SquareMatrix::Container passed to the two parameter constructor of the hmeigens::SquareMatrix class.
    // If the equality does not hold, it throws std::invalid_argument.
    [[nodiscard]] std::size_t checkIfAppropriateSize (std::size_t declaredSize, std::size_t containerSize);
}

/* ------------------------------*/
/* -------- Definitions -------- */
/* ------------------------------*/

// See the declaration for all relevant information.
// This function is basically a glorified wrapper for hmeigens::detail::validateSize(long long int) that collects every other signed value and casts it into the bigger mold.
template <std::signed_integral SigInt>
std::size_t hmeigens::detail::validateSize (SigInt toValidate) {
    return hmeigens::detail::validateSize(static_cast<long long int>(toValidate));
}

#endif
