// Test file for tolerances.hpp.
// Sections:
// - test cases:
//      - hmeigens::normTolerance;
//      - hmeigens::toleranceTolerance;
//      - hmeigens::normPreservationTolerance;
//      - hmeigens::eigenPreservationTolerance.
#include "hmeigens/tolerances.hpp"
#include "hmeigens/square_matrix.hpp"
#include "hmeigens/constants.hpp"
#include "hmeigens/scalar.hpp"

#include "comparison_helpers.hpp"

#include <catch2/catch_test_macros.hpp>

// Readability of tests.
using hmeigens::test::checkRealULPs;
#define HMS HMEIGENS_MAKE_SCALAR

/* ------------------------------------------------ */
/* ----------- hmeigens::normTolerance ------------ */
/* ------------------------------------------------ */
//
TEST_CASE("Tolerances test: normTolerance gives the expected values.", "[tolerances][normTolerance]") {
    // GIVEN a positive integer
    // WHEN  hmeigens::normTolerance is called
    // THEN  it returns the expected value
    //
    // Minimum meaningful input.
    STATIC_REQUIRE(hmeigens::normTolerance(1) == 2*HMEIGENS_HYPOT_ULPS);
    // Meaningful input.
    STATIC_REQUIRE(hmeigens::normTolerance(5) == 50*HMEIGENS_HYPOT_ULPS);
}

/* ------------------------------------------------ */
/* --------- hmeigens::toleranceTolerance --------- */
/* ------------------------------------------------ */
//
TEST_CASE("Tolerances test: toleranceTolerance gives the expected values.", "[tolerances][toleranceTolerance]") {
    // GIVEN one or two positive integers
    // WHEN  hmeigens::toleranceTolerance is called
    // THEN  it returns the expected value
    //
    // Default argument.
    // Minimum meaningful input.
    STATIC_REQUIRE(hmeigens::toleranceTolerance(1) == 4*HMEIGENS_HYPOT_ULPS+2);
    // Meaningful input.
    STATIC_REQUIRE(hmeigens::toleranceTolerance(5) == 100*HMEIGENS_HYPOT_ULPS+2);
    // Non-default argument. Same inputs.
    // Same as the default version with an addition.
    STATIC_REQUIRE(hmeigens::toleranceTolerance(1, 2) == hmeigens::toleranceTolerance(1)+2);
    // Explicitly computed value.
    STATIC_REQUIRE(hmeigens::toleranceTolerance(5, 20) == 100*HMEIGENS_HYPOT_ULPS+22);
}

/* ------------------------------------------------ */
/* ----- hmeigens::normPreservationTolerance ------ */
/* ------------------------------------------------ */
//
TEST_CASE("Tolerances test: normPreservationTolerance gives the expected values.", "[tolerances][normPreservationTolerance]") {
    // GIVEN a matrix
    // WHEN  hmeigens::normPreservationTolerance is called
    // THEN  it returns the expected value
    //
    // Null matrix. Exact value.
    checkRealULPs(
        hmeigens::normPreservationTolerance(
            hmeigens::SquareMatrix{{HMS(0.), HMS(0.), HMS(0.), HMS(0.)}}
        ),
        HMS(0.),
        0
    );
    // Single element.
    const auto hypotULPs = static_cast<hmeigens::Scalar>(HMEIGENS_HYPOT_ULPS);
    checkRealULPs(
        hmeigens::normPreservationTolerance(
            hmeigens::SquareMatrix{{HMS(0.), HMS(0.), HMS(8.), HMS(0.)}}
        ),
        hmeigens::epsilon*(HMS(32.)*hmeigens::toleranceK+HMS(128.)*hypotULPs),
        // The tolerance is the one for hmeigens::getTolerance + 1 for the sum. The second part is exact.
        hmeigens::toleranceTolerance(2)+1
    );
}

/* ------------------------------------------------ */
/* ----- hmeigens::eigenPreservationTolerance ----- */
/* ------------------------------------------------ */
//
TEST_CASE("Tolerances test: eigenPreservationTolerance gives the expected values.", "[tolerances][eigenPreservationTolerance]") {
    // GIVEN a matrix
    // WHEN  hmeigens::eigenPreservationTolerance is called
    // THEN  it returns the expected value
    //
    // Null matrix. Exact value.
    checkRealULPs(
        hmeigens::eigenPreservationTolerance(
            hmeigens::SquareMatrix{{HMS(0.), HMS(0.), HMS(0.), HMS(0.)}},
            1000,
            HMS(0.)
        ),
        HMS(0.),
        0
    );
    // Single element.
    const hmeigens::SquareMatrix testMatrix{
        {HMS(8.), HMS(0.), HMS(0.), HMS(0.),
         HMS(0.), HMS(0.), HMS(0.), HMS(0.),
         HMS(0.), HMS(0.), HMS(0.), HMS(0.),
         HMS(0.), HMS(0.), HMS(0.), HMS(0.)}
    };
    // The tolerance is the one for hmeigens::getTolerance + 1 for the sum and + 1 for std::pow.
    const auto tolerance = hmeigens::toleranceTolerance(4)+2;
    // Trivial power.
    checkRealULPs(
        hmeigens::eigenPreservationTolerance(
            testMatrix,
            1,
            // Norm of testMatrix^0 = I.
            HMS(2.)
        ),
        hmeigens::epsilon*(HMS(256.)*hmeigens::toleranceK+HMS(80.)),
        tolerance
    );
    // Non-trivial power different from size.
    checkRealULPs(
        hmeigens::eigenPreservationTolerance(
            testMatrix,
            3,
            // Norm of testMatrix^2.
            HMS(64.)
        ),
        hmeigens::epsilon*(HMS(24576.)*hmeigens::toleranceK+HMS(15360.)),
        tolerance
    );
}

#undef HMS
