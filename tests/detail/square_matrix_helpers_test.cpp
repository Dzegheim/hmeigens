#include "hmeigens/constants.hpp"
#include "hmeigens/detail/square_matrix_helpers.hpp"
#include "hmeigens/square_matrix.hpp"

using hmeigens::operator""_hs;

#include <cstddef>       // For std::size_t
#include <string>
#include <string_view>
#include <format>
#include <stdexcept>     // For std::length_error, std::invalid_argument
#include <concepts>      // For std::integral
#include <limits>        // For std::numeric_limits<long long int>::min()

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>
#include <catch2/matchers/catch_matchers_exception.hpp>


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

TEST_CASE("Square matrix helpers test: a valid unsigned size is accepted.", "[square_matrix_helpers]") {
    // GIVEN a valid unsigned size for a matrix
    // WHEN  hmeigens::detail::validateSize attempts to validate it
    // THEN  the size is reported as valid
    //
    // If the size is valid, the function returns the size, so it is enough to just check against the input.
    // Minimum valid size.
    CHECK(hmeigens::detail::validateSize(std::size_t{1}) == 1);
    // A large size for practical uses.
    // This is a magic number, but the upper boundary of the matrix size varies from machine to machine and is not easy to compute.
    // This is, for all intents and purposes, a number big enough to mean something, but small enough that it will always be a theoretically possible size.
    CHECK(hmeigens::detail::validateSize(std::size_t{10'000}) == 10'000);
}

TEST_CASE("Square matrix helpers test: a valid signed size is accepted.", "[square_matrix_helpers]") {
    // GIVEN a valid signed size for a matrix
    // WHEN  hmeigens::detail::validateSize attempts to validate it
    // THEN  the size is reported as valid
    //
    // Same numbers as the unsigned case, but signed.
    // All of these must remain untouched and reach the unsigned case.
    //
    // These values skip the template overload and directly go to the long long int overload.
    CHECK(hmeigens::detail::validateSize(1ll) == 1);
    CHECK(hmeigens::detail::validateSize(10'000ll) == 10'000);
    // These values reach the template overload.
    CHECK(hmeigens::detail::validateSize(1) == 1);
    CHECK(hmeigens::detail::validateSize(10'000) == 10'000);
}

TEST_CASE("Square matrix helpers test: size 0 is correctly reported.", "[square_matrix_helpers]") {
    // GIVEN size 0
    // WHEN  hmeigens::detail::validateSize attempts to validate it
    // THEN  the correct exception is thrown, with a message containing the reason and the invalid size
    checkInvalidSize<std::invalid_argument>(std::size_t{0}, "Size 0 is invalid for a matrix.");
    // The signed overload must not touch 0.
    // Since the signed overload does not throw with this text, this test passing means 0ll reached the unsigned validator.
    checkInvalidSize<std::invalid_argument>(0ll, "Size 0 is invalid for a matrix.");
}

TEST_CASE("Square matrix helpers test: size over max is correctly reported.", "[square_matrix_helpers]") {
    // GIVEN a size over the max squarable size
    // WHEN  hmeigens::detail::validateSize attempts to validate it
    // THEN  the correct exception is thrown, with a message containing the reason and the invalid size
    //
    // This number is, by construction, too big to accept.
    // The first test checks if the exception contains the invalid size.
    // The second one checks for the reason.
    checkInvalidSize<std::length_error>(hmeigens::maxMatrixSize+1, std::format("{0}x{0}", hmeigens::maxMatrixSize+1));
    checkInvalidSize<std::length_error>(hmeigens::maxMatrixSize+1, std::format("The maximum allowed size is {0}", hmeigens::maxMatrixSize));
}

TEST_CASE("Square matrix helpers test: sizes larger than container's max are reported.", "[square_matrix_helpers][may_be_skipped]") {
    // GIVEN a size corresponding to a number of elements under the maximum but larger than the container's max
    // WHEN  hmeigens::detail::validateSize attempts to validate it
    // THEN  the correct exception is thrown, with a message containing the reason and the invalid size
    //
    // The branch this test checks can only ever be reached if hmeigens::maxMatrixSize**2 is bigger than the container's max size.
    // If that cannot happen, this test makes no sense, and is skipped.
    // Squaring hmeigens::maxMatrixSize is always safe. The static asserts in constants.hpp ensure that.
    if (hmeigens::maxMatrixSize * hmeigens::maxMatrixSize <= hmeigens::SquareMatrix::Container{}.max_size()) {
        SKIP("The maximum number of elements a matrix can hold on this machine is smaller than the maximum amount the container allows. This check can never be meaningful on this machine.");
    }
    checkInvalidSize<std::length_error>(hmeigens::maxMatrixSize, std::format("{0}x{0}", hmeigens::maxMatrixSize));
    checkInvalidSize<std::length_error>(hmeigens::maxMatrixSize, "exceeds the maximum number of elements allowed");
}

TEST_CASE("Square matrix helpers test: negative sizes are correctly reported.", "[square_matrix_helpers]") {
    // GIVEN a negative size
    // WHEN  hmeigens::detail::validateSize attempts to validate it
    // THEN  the correct exception is thrown, with a message containing the reason and the invalid size
    //
    // Maximum negative value.
    checkInvalidSize<std::invalid_argument>(-1, "A matrix cannot have a negative size.\n---> Provided value: -1");
    // Minimum negative value.
    checkInvalidSize<std::invalid_argument>(std::numeric_limits<long long int>::min(), std::format("Provided value: {0}", std::numeric_limits<long long int>::min()));
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
