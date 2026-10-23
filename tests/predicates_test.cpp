// Test file for predicates.hpp/.cpp.
// Sections:
// - constants;
// - test cases:
//     - hmeigens::isFinite.

#include "hmeigens/scalar.hpp"
#include "hmeigens/square_matrix.hpp"
#include "hmeigens/predicates.hpp"

#include <limits>        // For std::numeric_limits

#include <catch2/catch_test_macros.hpp>

/* --------------------------------------------- */
/* ----------------- Constants ----------------- */
/* --------------------------------------------- */
//
// For readability of tests.
constexpr hmeigens::Scalar NaN = std::numeric_limits<hmeigens::Scalar>::quiet_NaN();
constexpr hmeigens::Scalar Inf = std::numeric_limits<hmeigens::Scalar>::infinity();
constexpr hmeigens::Scalar max = std::numeric_limits<hmeigens::Scalar>::max();
constexpr hmeigens::Scalar lowest = std::numeric_limits<hmeigens::Scalar>::lowest();

// Local brief alias for the conversion macro to improve legibility.
#define HMS HMEIGENS_MAKE_SCALAR

/* ------------------------------------------------ */
/* ------------------ Test cases ------------------ */
/* ------------------------------------------------ */
//
/* ------------------------------------------------ */
/* -------------- hmeigens::isFinite -------------- */
/* ------------------------------------------------ */
//
TEST_CASE("Predicates test: a finite matrix is correctly reported as such.", "[predicates][isFinite]") {
    // GIVEN a matrix whose elements are all finite
    // WHEN  hmeigens::isFinite checks it
    // THEN  the function returns true
    //
    // Null matrix.
    CHECK(hmeigens::isFinite(hmeigens::SquareMatrix{1}));
    // Non-null matrix.
    const hmeigens::SquareMatrix testMatrix{
        {{HMS(1.), HMS(-1.)}, {HMS(2.), HMS(-2.)}, {HMS(3.), HMS(-3.)},
         {HMS(4.), HMS(-4.)}, {HMS(5.), HMS(-5.)}, {HMS(6.), HMS(-6.)},
         {HMS(7.), HMS(-7.)}, {HMS(8.), HMS(-8.)}, {HMS(9.), HMS(-9.)}}
    };
    CHECK(hmeigens::isFinite(testMatrix));
    // Matrix with huge values.
    CHECK(hmeigens::isFinite(hmeigens::SquareMatrix{{{max, lowest}}}));
}

TEST_CASE("Predicates test: a matrix with non-finite elements is correctly reported as such.", "[predicates][isFinite]") {
    // GIVEN a matrix whose elements are not all finite
    // WHEN  hmeigens::isFinite checks it
    // THEN  the function returns false
    //
    // Infinities.
    // Real positive.
    const hmeigens::SquareMatrix infinity1{
        {{Inf    , HMS(0.)}, {HMS(0.), HMS(0.)},
         {HMS(0.), HMS(0.)}, {HMS(0.), HMS(0.)}}
    };
    // Real negative.
    const hmeigens::SquareMatrix infinity2{
        {{HMS(0.), HMS(0.)}, {-Inf   , HMS(0.)},
         {HMS(0.), HMS(0.)}, {HMS(0.), HMS(0.)}}
    };
    // Imaginary positive.
    const hmeigens::SquareMatrix infinity3{
        {{HMS(0.), HMS(0.)}, {HMS(0.), HMS(0.)},
         {HMS(0.), Inf    }, {HMS(0.), HMS(0.)}}
    };
    // Imaginary negative.
    const hmeigens::SquareMatrix infinity4{
        {{HMS(0.), HMS(0.)}, {HMS(0.), HMS(0.)},
         {HMS(0.), HMS(0.)}, {HMS(0.), -Inf   }}
    };
    CHECK_FALSE(hmeigens::isFinite(infinity1));
    CHECK_FALSE(hmeigens::isFinite(infinity2));
    CHECK_FALSE(hmeigens::isFinite(infinity3));
    CHECK_FALSE(hmeigens::isFinite(infinity4));
    //
    // NaNs.
    // Real.
    const hmeigens::SquareMatrix nan1{
        {{HMS(0.), HMS(0.)}, {HMS(0.), HMS(0.)},
         {NaN    , HMS(0.)}, {HMS(0.), HMS(0.)}}
    };
    // Imaginary.
    const hmeigens::SquareMatrix nan2{
        {{HMS(0.), HMS(0.)}, {HMS(0.), HMS(0.)},
         {HMS(0.), HMS(0.)}, {HMS(0.), NaN   }}
    };
    CHECK_FALSE(hmeigens::isFinite(nan1));
    CHECK_FALSE(hmeigens::isFinite(nan2));
}

#undef HMS
