// Test file for algebra_helpers.hpp/.cpp.
// Sections:
// - constants;
// - test cases:
//      - hmeigens::detail::twoNorm;
//      - hmeigens::detail::complexUnitVector.
#include "hmeigens/detail/algebra_helpers.hpp"
#include "hmeigens/scalar.hpp"
#include "hmeigens/tolerances.hpp"
#include "hmeigens/square_matrix.hpp"

#include "../comparison_helpers.hpp"

#include <catch2/catch_test_macros.hpp>

#include <cstdint>       // For std::uint64_t
#include <cmath>         // For std::sqrt, std::hypot

// Readability of tests.
using hmeigens::test::checkRealULPs;
using hmeigens::test::checkComplexULPs;
#define HMS HMEIGENS_MAKE_SCALAR

/* --------------------------------------------- */
/* ----------------- Constants ----------------- */
/* --------------------------------------------- */
//
// Tolerance for the hmeigens::detail::complexUnitVector function.
// The function uses std::hypot, so it will have at least HMEIGENS_HYPOT_ULPS of tolerance. The division is another half ULP, as it is correctly rounded.
// Since the error is relative, they must all be doubled, as per the discussion in tolerances.hpp.
// This is kept local, and out of tolerances.hpp, as it is a simple computation and belongs to hmeigens::detail, which is not documented via Doxygen.
constexpr std::uint64_t CUVTol = 2*HMEIGENS_HYPOT_ULPS+1;

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


/* ------------------------------------------------ */
/* ------ hmeigens::detail::complexUnitVector ----- */
/* ------------------------------------------------ */
//
TEST_CASE("Complex unit vector test: the direction is correct.", "[algebra_helpers][detail][complexUnitVector]") {
    // GIVEN a complex number
    // WHEN  its associated complex unit vector is computed
    // THEN  it is the expected vector with the correct length
    //
    // Special case: (0.; 0.).
    checkComplexULPs(
        hmeigens::detail::complexUnitVector({HMS(0.), HMS(0.)}),
        {HMS(1.), HMS(0.)},
        // It must return exactly 1., so no tolerance allowed.
        0
    );
    // Notable directions.
    // Real, positive.
    checkComplexULPs(
        hmeigens::detail::complexUnitVector({HMS(6.7), HMS(0.)}),
        {HMS(1.), HMS(0.)},
        CUVTol
    );
    // Real, negative.
    checkComplexULPs(
        hmeigens::detail::complexUnitVector({HMS(-4.2), HMS(0.)}),
        {HMS(-1.), HMS(0.)},
        CUVTol
    );
    // Imaginary, positive.
    checkComplexULPs(
        hmeigens::detail::complexUnitVector({HMS(0.), HMS(1000.)}),
        {HMS(0.), HMS(1.)},
        CUVTol
    );
    // Imaginary, negative.
    checkComplexULPs(
        hmeigens::detail::complexUnitVector({HMS(0.), HMS(-3.141592653)}),
        {HMS(0.), HMS(-1.)},
        CUVTol
    );
    // Generic input.
    const hmeigens::Complex z{HMS(12.), HMS(-6.)};
    const hmeigens::Complex zCUV = hmeigens::detail::complexUnitVector(z);
    // The expected value is (2/sqrt(5); -1/sqrt(5)).
    // This check already verifies the vector is unitary, as the norm of the expected value is 1: sqrt(4/5+1/5) = 1.
    checkComplexULPs(
        zCUV,
        hmeigens::Complex{HMS(2.)/std::sqrt(HMS(5.)), HMS(-1.)/std::sqrt(HMS(5.))},
        // Tolerance is CUVTol+2, with 1 ULP being from the std::sqrt function, and 1 from the division, as both are correctly rounded.
        CUVTol+2
    );
}



#undef HMS
