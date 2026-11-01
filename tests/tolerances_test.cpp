// Test file for tolerances.hpp.
// Sections:
// - test cases:
//      - hmeigens::normTolerance.
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
