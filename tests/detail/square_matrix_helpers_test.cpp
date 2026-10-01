#include "hmeigens/constants.hpp"
#include "hmeigens/detail/square_matrix_helpers.hpp"
#include "hmeigens/square_matrix.hpp"

using hmeigens::operator""_hs;

#include <cstddef>       // For std::size_t
#include <cstdint>       // For std::intmax_t, std::uintmax_t, SIZE_MAX, UINTMAX_MAX 
#include <string>
#include <string_view>
#include <format>
#include <stdexcept>     // For std::length_error, std::invalid_argument
#include <concepts>      // For std::integral

#include <limits>        // For std::numeric_limits<>::min()

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>
#include <catch2/matchers/catch_matchers_exception.hpp>

// The value used is a magic number, but the upper boundary of the matrix size varies from machine to machine and may not fit all types.
// This is, for all intents and purposes, a number big enough to mean something, but small enough that it will always be a theoretically possible size.
constexpr int bigEnoughNumber = 10'000;

// This is a helper function to check whether hmeigens::detail::validateSize:
// - throws;
// - throws the correct exception type;
// - exception's message contains the correct information, including the invalid size and the reason why it was invalid.
// The IntType template parameter selects the appropriate overload of the validator based on whether the passed parameter is signed or not.
template <typename Exception, std::integral IntType>
static void checkInvalidSize(IntType size, std::string_view expectedText) {
    CAPTURE (size, expectedText);
    CHECK_THROWS_MATCHES(
        hmeigens::detail::validateSize(size),
        Exception,
        Catch::Matchers::MessageMatches(
            Catch::Matchers::ContainsSubstring(std::string{expectedText})
        )
    );
    return;
}

// This is a helper function to check whether hmeigens::detail::checkIfAppropriateSize:
// - throws;
// - throws the correct exception type;
// - exception's message contains the correct information, including the mismatch and the expected amount.
template <typename Exception>
static void checkMismatchingSize(std::size_t size, std::string_view expectedText, const hmeigens::SquareMatrix::Container& container) {
    CAPTURE (size, expectedText, container);
    CHECK_THROWS_MATCHES(
        hmeigens::detail::checkIfAppropriateSize(size,container.size()),
        Exception,
        Catch::Matchers::MessageMatches(
            Catch::Matchers::ContainsSubstring(std::string{expectedText})
        )
    );
    return;
}

TEST_CASE("Square matrix helpers test: a valid std::size_t is accepted.", "[square_matrix_helpers]") {
    // GIVEN a valid std::size_t for a matrix
    // WHEN  hmeigens::detail::validateSize attempts to validate it
    // THEN  the size is reported as valid
    //
    // If the size is valid, the function returns the size, so it is enough to just check against the input.
    // These values directly reach for hmgeigens::detail::validateSize(std::size_t).
    // Minimum valid size.
    CHECK(hmeigens::detail::validateSize(std::size_t{1}) == 1);
    // Largest possible value.
    CHECK(hmeigens::detail::validateSize(hmeigens::maxMatrixSize) == hmeigens::maxMatrixSize);
}

TEST_CASE("Square matrix helpers test: a valid signed size is accepted.", "[square_matrix_helpers]") {
    // GIVEN a valid unsigned size for a matrix
    // WHEN  hmeigens::detail::validateSize attempts to validate it
    // THEN  the size is reported as valid
    //
    // These values directly reach for hmgeigens::detail::validateSize(std::intmax_t).
    // The maximum size cannot be tested by these overloads, as the largest std::size_t may exceed the largest signed integer.
    CHECK(hmeigens::detail::validateSize(std::intmax_t{1}) == 1);
    CHECK(hmeigens::detail::validateSize(std::intmax_t{bigEnoughNumber}) == bigEnoughNumber);
    // These values reach the template overload in any reasonable system.
    // If an unsigned short int is intmax_t then the template is the least of the concerns.
    CHECK(hmeigens::detail::validateSize(static_cast<short int>(1)) == 1);
    CHECK(hmeigens::detail::validateSize(static_cast<short int>(bigEnoughNumber)) == bigEnoughNumber);
}

TEST_CASE("Square matrix helpers test: a valid unsigned size is accepted.", "[square_matrix_helpers]") {
    // GIVEN a valid signed size for a matrix
    // WHEN  hmeigens::detail::validateSize attempts to validate it
    // THEN  the size is reported as valid
    //
    // These values directly reach for hmgeigens::detail::validateSize(std::uintmax_t) if it exists, and for the std::size_t overload if not.
    // They may be redundant, but never strictly need to be skipped.
    // Here the max size can be tested for, as it must always fit within this type.
    CHECK(hmeigens::detail::validateSize(std::uintmax_t{1}) == 1);
    CHECK(hmeigens::detail::validateSize(std::uintmax_t{hmeigens::maxMatrixSize}) == hmeigens::maxMatrixSize);
    // These values reach the template overload in any reasonable system.
    // If a short int is uintmax_t then the template is the least of the concerns.
    // Here the max matrix size may not fit in the type, so back to bigEnoguhNumber it is.
    CHECK(hmeigens::detail::validateSize(static_cast<unsigned short int>(1)) == 1);
    CHECK(hmeigens::detail::validateSize(static_cast<unsigned short int>(bigEnoughNumber)) == bigEnoughNumber);
}

TEST_CASE("Square matrix helpers test: size 0 is correctly reported.", "[square_matrix_helpers]") {
    // GIVEN size 0
    // WHEN  hmeigens::detail::validateSize attempts to validate it
    // THEN  the correct exception is thrown, with a message containing the reason and the invalid size
    checkInvalidSize<std::invalid_argument>(std::size_t{0}, "Size 0 is invalid for a matrix.");
    // The other overloads must not touch 0.
    // Since the overloads don't throw with this text, this test passing means 0 reached the unsigned validator.
    // These two cases are enough as each template then calls the corresponding non-template validator, so these 0s always run through both.
    checkInvalidSize<std::invalid_argument>(static_cast<unsigned short int>(0), "Size 0 is invalid for a matrix.");
    checkInvalidSize<std::invalid_argument>(static_cast<short int>(0), "Size 0 is invalid for a matrix.");
}

TEST_CASE("Square matrix helpers test: size over max is correctly reported.", "[square_matrix_helpers]") {
    // GIVEN a size over the max
    // WHEN  hmeigens::detail::validateSize attempts to validate it
    // THEN  the correct exception is thrown, with a message containing the reason and the invalid size
    //
    // This number is, by construction, too big to accept.
    // The first test checks if the exception contains the invalid size.
    // The second one checks for the reason.
    checkInvalidSize<std::length_error>(hmeigens::maxMatrixSize+1, std::format("{0}x{0}", hmeigens::maxMatrixSize+1));
    checkInvalidSize<std::length_error>(hmeigens::maxMatrixSize+1, std::format("Note that the maximum size allowed for a matrix with the current build settings is {0}.", hmeigens::maxMatrixSize));
}

TEST_CASE("Square matrix helpers test: negative sizes are correctly reported.", "[square_matrix_helpers]") {
    // GIVEN a negative size
    // WHEN  hmeigens::detail::validateSize attempts to validate it
    // THEN  the correct exception is thrown, with a message containing the reason and the invalid size
    //
    // Maximum negative value.
    checkInvalidSize<std::invalid_argument>(static_cast<short int>(-1), "A matrix cannot have a negative size.\n---> Provided value: -1");
    // Minimum negative value.
    checkInvalidSize<std::invalid_argument>(std::numeric_limits<std::intmax_t>::min(), std::format("Provided value: {0}", std::numeric_limits<std::intmax_t>::min()));
}

TEST_CASE ("Square matrix helpers test: integer values that don't fit std::size_t are correctly reported.", "[square_matrix_helpers][may_be_skipped]") {
    // GIVEN a value that fits std::intmax_t but not std::size_t
    // WHEN  hmeigens::detail::validateSize attempts to validate it
    // THEN  the correct exception is thrown, with a message containing the reason and the invalid size
    #if SIZE_MAX == UINTMAX_MAX
    SKIP("In the current build settings std::size_t coincides with std::intmax_t.");
    #endif
    // The minimum value that may satisfy the condition.
    checkInvalidSize<std::length_error>(
        std::uintmax_t{std::numeric_limits<std::size_t>::max()}+1,
        std::format("A {0}x{0} matrix cannot be created.\n---> Number of elements is too large to be represented by std::size_t. The maximum value that can be represented by the type is {1}.", std::uintmax_t{std::numeric_limits<std::size_t>::max()}+1, std::numeric_limits<std::size_t>::max())
    );
    // The maximum possible value.
    checkInvalidSize<std::length_error>(
        std::numeric_limits<std::uintmax_t>::max(),
        std::format("A {0}x{0} matrix cannot be created.\n---> Number of elements is too large to be represented by std::size_t. The maximum value that can be represented by the type is {1}.", std::numeric_limits<std::uintmax_t>::max(), std::numeric_limits<std::size_t>::max())
    );
}

TEST_CASE("Square matrix helpers test: appropriate size comparison.", "[square_matrix_helpers]") {
    // GIVEN a hmeigens::SquareMatrix::Container with the appropriate size
    // WHEN  hmeigens::detail::checkIfAppropriateSize tries to validate it
    // THEN  it is accepted
    //
    // A 0 here passes, because this function does not perform that check.
    CHECK (hmeigens::detail::checkIfAppropriateSize(0, hmeigens::SquareMatrix::Container{}.size()) == 0);
    // A valid and meaningful size.
    CHECK (hmeigens::detail::checkIfAppropriateSize(3, hmeigens::SquareMatrix::Container{1._hs, 2._hs, 3._hs, 4._hs, 5._hs, 6._hs, 7._hs, 8._hs, 9._hs}.size()) == 3);
}

TEST_CASE("Square matrix helpers test: mismatching or inappropriate sizes are refused.", "[square_matrix_helpers]") {
    // GIVEN a hmeigens::SquareMatrix::Container with a mismatching or invalid size
    // WHEN  hmeigens::detail::checkIfAppropriateSize tries to validate it
    // THEN  it is rejected with the appropriate exception and text
    checkMismatchingSize<std::invalid_argument>(4, "The provided matrix is not 4x4.\n---> Expected elements: 16.\n---> Provided elements: 0.", hmeigens::SquareMatrix::Container{});
    checkMismatchingSize<std::invalid_argument>(2, "The provided matrix is not 2x2.\n---> Expected elements: 4.\n---> Provided elements: 6.", hmeigens::SquareMatrix::Container{1._hs, 2._hs, 3._hs, 4._hs, 5._hs, 6._hs});
}
