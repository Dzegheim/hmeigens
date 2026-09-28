#include "hmeigens/scalar.hpp"

using hmeigens::operator""_hs;

#include <type_traits>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

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

TEST_CASE("Literal suffix _hs test: the return type matches the one expected from the build.", "[scalar]") {
    // GIVEN a floating-point value with the _hs literal
    // WHEN  its type is compared with the appropriate floating-point one of that build
    // THEN  they're the same type
    #ifdef HMEIGENS_SCALAR_FLOAT
        STATIC_REQUIRE(
            std::is_same_v<
                decltype(1.2345_hs),
                float
            >
        );
    #else
        STATIC_REQUIRE(
            std::is_same_v<
                decltype(1.2345_hs),
                double
            >
        );
    #endif
}

TEST_CASE("Literal suffix _hs test: expected values match.", "[scalar]") {
    // GIVEN a floating-point value with the _hs literal
    // WHEN  its value is compared with a corresponding hmeigens::Scalar
    // THEN  they're within 0 ULPs of each other
    // This value is exactly representable both in float and double.
    CHECK_THAT(
        1._hs,
        Catch::Matchers::WithinULP(static_cast<hmeigens::Scalar>(1.), 0)
    );
    // These two values are modified by the conversion. The tests check that they are modified in the same way, as is expected.
    CHECK_THAT(
        1.2345_hs,
        Catch::Matchers::WithinULP(static_cast<hmeigens::Scalar>(1.2345), 0)
    );
    CHECK_THAT(
        1e-3_hs,
        Catch::Matchers::WithinULP(static_cast<hmeigens::Scalar>(1e-3), 0)
    );
}
