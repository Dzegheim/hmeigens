#include "hmeigens/constants.hpp"
#include "hmeigens/square_matrix.hpp"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>
#include <catch2/matchers/catch_matchers_exception.hpp>

TEST_CASE("Square matrix test: a matrix has the size it was constructed with.", "[square_matrix]") {
    // GIVEN a valid size for a matrix
    // WHEN  the matrix is constructed
    // THEN  the size of the matrix is the correct one
    CHECK(hmeigens::SquareMatrix(1).size() == 1);
    CHECK(hmeigens::SquareMatrix(500).size() == 500);
}
