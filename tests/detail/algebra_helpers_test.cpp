// Test file for algebra_helpers.hpp/.cpp.
// Sections:
// - test cases:
//      - hmeigens::detail::twoNorm.
#include "hmeigens/detail/algebra_helpers.hpp"
#include "hmeigens/scalar.hpp"
#include "hmeigens/tolerances.hpp"
#include "hmeigens/square_matrix.hpp"

#include "../comparison_helpers.hpp"

#include <catch2/catch_test_macros.hpp>

// Local brief alias for the conversion macro to improve legibility.
#define HMS HMEIGENS_MAKE_SCALAR

// Readability of tests.
using hmeigens::test::checkRealULPs;

/* ------------------------------------------------ */
/* ------------------ Test cases ------------------ */
/* ------------------------------------------------ */
//
/* ------------------------------------------------ */
/* ----------- hmeigens::detail::twoNorm ---------- */
/* ------------------------------------------------ */
//
/* ----------------------------- */
/* --------- IMPORTANT --------- */
/* ----------------------------- */
//
// The hmeigens::detail::twoNorm function was originally the hmeigens::SquareMatrix::norm function, its body was simply moved to allow it to be called on other ranges as well, and the norm function became a wrapper.
// Most of the behaviours expected by this function are therefore already tested in square_matrix_test.cpp, and have been left there during the move.
// The only behaviours tested here are those that are not specific to matrices.
// On test tolerance: the tolerance was discussed extensively in tolerances.hpp for hmeigens::normTolerance. The tests here use the same tolerance, even though it is larger, as it squares the size. The vectors used are small, so this does not alter the reliability in any meaningful way.
// Tests that require exactness are still exact.
//
// All tests are performed using hmeigens::SquareMatrix::Container as the data type, since this is the type that is used throughout the code.
TEST_CASE("2-norm test: empty containers.", "[algebra_helpers][detail][twoNorm]") {
    // GIVEN an empty container
    // WHEN  its 2-norm is computed
    // THEN  it is 0.
    checkRealULPs(
        hmeigens::detail::twoNorm(hmeigens::SquareMatrix::Container{}),
        HMS(0.),
        0
    );
}

TEST_CASE("2-norm test: containers whose length isn't a perfect square.", "[algebra_helpers][detail][twoNorm]") {
    // GIVEN a container whose length is not a perfect square holding hmeigens::Complex elements
    // WHEN  its 2-norm is computed
    // THEN  it is the expected value
    //
    // Zero-filled container. Must be exact.
    checkRealULPs(
        hmeigens::detail::twoNorm(hmeigens::SquareMatrix::Container(3)),
        HMS(0.),
        0
    );
    // Arbitrary container.
    checkRealULPs(
        hmeigens::detail::twoNorm(hmeigens::SquareMatrix::Container{
            {HMS(3.), HMS(0.)}, {HMS(0.), HMS(4.)}
        }),
        HMS(5.),
        hmeigens::normTolerance(2)
    );
}

#undef HMS
