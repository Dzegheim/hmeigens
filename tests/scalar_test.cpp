// Test file for the header only scalar.hpp.
// Sections:
// - test cases:
//      - hmeigens::Scalar;
//      - HMEIGENS_MAKE_SCALAR.
#include "hmeigens/scalar.hpp"

#include <type_traits>

#include <catch2/catch_test_macros.hpp>

/* ------------------------------------------------ */
/* ------------------ Test cases ------------------ */
/* ------------------------------------------------ */
//
/* ------------------------------------------------ */
/* --------------- hmeigens::Scalar --------------- */
/* ------------------------------------------------ */
//
TEST_CASE("Scalar name test: the string naming the type matches the type from the build.", "[scalar]") {
    // GIVEN the project
    // WHEN  it is built
    // THEN  the name of the selected scalar type matches the type
    #ifdef HMEIGENS_SCALAR_FLOAT
        // When hmeigens::Scalar is float...
        STATIC_REQUIRE(
            std::is_same_v<
                hmeigens::Scalar,
                float
            >
        );
        // Then the name is "float".
        CHECK(hmeigens::scalarType == "float");
    #else
        // When hmeigens::Scalar is double...
        STATIC_REQUIRE(
            std::is_same_v<
                hmeigens::Scalar,
                double
            >
        );
        // Then the name is "double".
        CHECK(hmeigens::scalarType == "double");
    #endif
}

/* ------------------------------------------------ */
/* ------------ HMEIGENS_MAKE_SCALAR -------------- */
/* ------------------------------------------------ */
//

// Local brief alias for the conversion macro to improve legibility.
#define HMS HMEIGENS_MAKE_SCALAR
TEST_CASE("HMEIGENS_MAKE_SCALAR test: the converted literal's type matches the one expected from the build.", "[scalar][HMEIGENS_MAKE_SCALAR]") {
    // GIVEN a floating-point value in the HMEIGENS_MAKE_SCALAR macro
    // WHEN  its type is compared with the appropriate floating-point one of that build
    // THEN  they're the same type
    #ifdef HMEIGENS_SCALAR_FLOAT
        STATIC_REQUIRE(
            std::is_same_v<
                decltype(HMS(1.2345)),
                float
            >
        );
        STATIC_REQUIRE(
            std::is_same_v<
                decltype(HMS(1e0)),
                float
            >
        );
        STATIC_REQUIRE(
            std::is_same_v<
                decltype(HMS(-1.)),
                float
            >
        );
    #else
        STATIC_REQUIRE(
            std::is_same_v<
                decltype(HMS(1.2345)),
                double
            >
        );
        STATIC_REQUIRE(
            std::is_same_v<
                decltype(HMS(1e0)),
                double
            >
        );
        STATIC_REQUIRE(
            std::is_same_v<
                decltype(HMS(-1.)),
                double
            >
        );
    #endif
}

#undef HMS
