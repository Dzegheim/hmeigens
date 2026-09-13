#include "hmeigens/constants.hpp"
#include "hmeigens/square_matrix.hpp"

#include <stdexcept>
#include <format>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>
#include <catch2/matchers/catch_matchers_exception.hpp>

// This is a helper function to check whether the matrix constructor when passed an invalid size:
// - throws;
// - throws the correct exception type;
// - the exception's message contains the correct information, including the invalid size and the reason why it was invalid.
template <typename Exception>
static void checkInvalidSize(std::size_t size, std::string_view expectedText) {
    CAPTURE (size, expectedText);
    CHECK_THROWS_MATCHES(
        hmeigens::SquareMatrix(size),
        Exception,
        Catch::Matchers::MessageMatches(
            Catch::Matchers::ContainsSubstring(std::string{expectedText})
        )
    );
}

TEST_CASE("Square matrix test: a matrix has the size it was constructed with.", "[square_matrix]") {
    // GIVEN a valid size for a matrix
    // WHEN  the matrix is constructed
    // THEN  the size of the matrix is the correct one
    CHECK(hmeigens::SquareMatrix(1).size() == 1);
    CHECK(hmeigens::SquareMatrix(500).size() == 500);
}

TEST_CASE("Square matrix test: size 0 and over max are correctly reported.", "[square_matrix]") {
    // GIVEN size of 0 or over the max squarable size
    // WHEN  the matrix is constructed
    // THEN  the correct exception is thrown, with a message containing the reason and the invalid size
    checkInvalidSize<std::invalid_argument>(0, "Size 0 is invalid for a matrix.");
    // This number is, by construction, too big for the matrix constructor to accept.
    // The first test checks if the exception contains the invalid size.
    // The second one checks for the reason.
    checkInvalidSize<std::length_error>(hmeigens::maxMatrixSize+1, std::format("{0}x{0}", hmeigens::maxMatrixSize+1));
    checkInvalidSize<std::length_error>(hmeigens::maxMatrixSize+1, std::format("The maximum allowed size is {0}", hmeigens::maxMatrixSize));
}

TEST_CASE("Square matrix test: sizes larger than container's max are reported.", "[square_matrix]") {
    // GIVEN a size corresponding to a number of elements under the maximum but larger than the container's max
    // WHEN  the matrix is constructed
    // THEN  the correct exception is thrown, with a message containing the reason and the invalid size
    //
    // The branch this test checks can only ever be reached if hmeigens::maxMatrixSize**2 is bigger than the container's max size.
    // If that cannot happen, this test makes no sense, and is skipped.
    // Squaring hmeigens::maxMatrixSize is always safe. The static asserts in constants.hpp ensure that.
    if (!(hmeigens::maxMatrixSize * hmeigens::maxMatrixSize > hmeigens::detail::Container{}.max_size())) {
        SKIP("The maximum number of elements a matrix can hold on this machine is smaller than the maximum size the container allows. This check can never be meaningful on this machine.");
    }
    checkInvalidSize<std::length_error>(hmeigens::maxMatrixSize, std::format("{0}x{0}", hmeigens::maxMatrixSize));
    checkInvalidSize<std::length_error>(hmeigens::maxMatrixSize, "exceeds the maximum number of elements allowed");
}

// There is no meaningful way to test for the std::bad_alloc exception. That is generated when the checks pass on the size, but the container cannot allocate, and it's the standard library's job.
// In the documentation there is still info that the constructor can throw, but testing for it means testing std::vector.
