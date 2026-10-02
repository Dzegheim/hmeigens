// File for functions used by hmeigens::SquareMatrix to validate parameters for the constructors.

#ifndef HMEIGENS_DETAIL_SQUARE_MATRIX_HELPERS_HPP
#define HMEIGENS_DETAIL_SQUARE_MATRIX_HELPERS_HPP

#include <cstddef>      // For std::size_t
#include <cstdint>      // For std::intmax_t, std::uintmax_t, SIZE_MAX, UINTMAX_MAX
#include <concepts>     // For std::signed_integral, std::unsigned_integral, std::same_as
#include <type_traits>  // For std::remove_cv_t

/* ------------------------------*/
/* --------- IMPORTANT --------- */
/* ------------------------------*/
//
// The main idea behind the hmeigens::detail::validateSize functions below is that std::size_t is a useful type, but a little bit of a bastard as well.
// On the machine where this code was physically written and tested std::size_t is 64 bit, and so its width matches std::uintmax_t exactly, but it may not be the case on other machines where it is, say, 32 bit.
// In this implementation the reasoning varies based on 3 (actually 2) cases:
// SIZE_MAX > UINTMAX_MAX: Impossible. If your machine satisfies this, you're not using standard C++, and this code is not meant for you.
// SIZE_MAX == UINTMAX_MAX: The machine on which this code was written and physically tested.
// It holds that std::uintmax_t is std::size_t, so the std::uintmax_t overload is a compile error. The pipeline for a number being processed, depending on its type, is as follows (where T = template, NT = non-template):
// - int -> signed_integral T -> NT intmax_t -> NT size_t;
// - unsigned int -> unsigned_integral T -> NT size_t [***];
// - std::intmax_t -> NT intmax_t -> NT size_t;
// - std::uintmax_t -> NT size_t;
// - std::size_t -> NT size_t.
// SIZE_MAX < UINTMAX_MAX: The std::uintmax_t overload exists, as the type is wider than std::size_t. The pipeline is:
// - int -> signed T -> NT intmax_t -> NT uintmax_t -> NT size_t;
// - unsigned int -> unsigned T -> NT uintmax_t -> NT size_t [***];
// - std::intmax_t -> NT intmax_t -> NT uintmax_t -> NT size_t;
// - std::uintmax_t -> NT uintmax_t -> NT size_t;
// - std::size_t -> NT size_t.
// [***] This assumes an unsigned int is not size_t. If it were, the template would simply be skipped.

namespace hmeigens::detail {
    // Helper function to validate the size of a matrix before constructing it.
    // It rejects 0 and values that are too large to be represented or held by hmeigens::SquareMatrix::Container.
    // If the size is 0, it throws std::invalid_argument.
    // If the size is too large it throws std::length_error.
    [[nodiscard]] std::size_t validateSize (std::size_t toValidate);

    // Helper function that validates the sign of a size, to avoid a negative number wrapping std::size_t and giving the wrong exception.
    // A negative size must be an std::invalid_argument, not an std::length_error, which happens for a wrapped value.
    // This function checks if the size is negative, throws if it is, or casts and calls the unsigned validator if not.
    // Note that the unsigned validator can be the std::size_t overload. See the comment at the top of the file.
    [[nodiscard]] std::size_t validateSize (std::intmax_t toValidate);

    #if SIZE_MAX < UINTMAX_MAX
    // If the maximum size is smaller than the maximum unsigned integer, this function is needed.
    // If they are equal (it cannot be that SIZE_MAX > UINTMAX_MAX) this function is not needed, as it would be an exact redeclaration/redefinition of the std::size_t one. In this case ANY unsigned integer can always be safely cast into std::size_t.
    // If this function exists, it checks that a given size fits into std::size_t, throws if it doesn't, or casts and calls the std::size_t validator if it does.
    [[nodiscard]] std::size_t validateSize (std::uintmax_t toValidate);
    #endif

    // Helper functions needed to allow for values narrower than the largest ones to be processed correctly.
    // These need to be templates, as having only the non-template overloads does not compile. The error could be, for example
    // > error: call of overloaded 'validateSize(int)' is ambiguous
    // if a value is not explicitly std::uintmax_t, std::intmax_t, or std::size_t.
    // These simply widen each value to the largest one with a cast, then call the other validators, that perform the actual checks.
    template <std::signed_integral SigInt>
    [[nodiscard]] std::size_t validateSize (SigInt toValidate);

    template <std::unsigned_integral UnsigInt>
    [[nodiscard]] std::size_t validateSize (UnsigInt toValidate);


    // Helper function to verify that the size provided to the constructor, when squared, is equal to the value of the other parameter.
    // The variable declaredSize is always assumed to already have been validated by hmeigens::detail::validateSize.
    // The variable containerSize is assumed to be the valid size of the hmeigens::SquareMatrix::Container passed to the two parameter constructor of the hmeigens::SquareMatrix class.
    // If the equality does not hold, it throws std::invalid_argument.
    [[nodiscard]] std::size_t checkIfAppropriateSize (std::size_t declaredSize, std::size_t containerSize);


    // Fold expression that allows for any Allowed type to be accepted, fundamentally an allowlist instead of a forbidlist.
    // Usage of std::remove_cv_t is because constness is always allowed and does not alter whether a type is good or not to become a size. Any const int is allowed, any const double is not.
    template <typename Candidate, typename... Allowed>
    concept IsItAllowed = (std::same_as<std::remove_cv_t<Candidate>, Allowed> or ...);
}




/* ------------------------------*/
/* -------- Definitions -------- */
/* ------------------------------*/
//
// See the declaration for all relevant information.
// These functions are basically a glorified wrapper for hmeigens::detail::validateSize() with the corresponding parameter type.
// They collect every other signed/unsigned value and cast it into the bigger mold.
// Note that the unsigned validator can be the std::size_t overload. See the comment at the top of the file.
template <std::signed_integral SigInt>
std::size_t hmeigens::detail::validateSize (SigInt toValidate) {
    return hmeigens::detail::validateSize(static_cast<std::intmax_t>(toValidate));
}

template <std::unsigned_integral UnsigInt>
std::size_t hmeigens::detail::validateSize (UnsigInt toValidate) {
    return hmeigens::detail::validateSize(static_cast<std::uintmax_t>(toValidate));
}

#endif
