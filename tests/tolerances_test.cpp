// Test file for tolerances.hpp.
// Sections:
// - test cases:
//      - hmeigens::normTolerance;
//      - hmeigens::toleranceTolerance.
#include "hmeigens/tolerances.hpp"

#include <catch2/catch_test_macros.hpp>

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
