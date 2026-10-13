// Header-only collection of functions used by the tests to compare floating-point values.
// Contents:
// - hmeigens::test::checkRealULPs;
// - hmeigens::test::checkComplexULPs.
#ifndef HMEIGENS_TEST_COMPARISON_HELPERS_HPP
#define HMEIGENS_TEST_COMPARISON_HELPERS_HPP

#include "hmeigens/scalar.hpp"

#include <cstdint>     // For std::uint64_t

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

namespace hmeigens::test {

    // This helper checks that two real numbers are within the expected tolerance of one another.
    inline void checkRealULPs(
        // The value to be checked.
        const Scalar toCheck,
        // The expected value.
        const Scalar expected,
        // How many ULPs they can be apart.
        const std::uint64_t toleranceULPs
    );
    
    // This helper checks that both the real and the imaginary part of two complex numbers are within the expected tolerance of one another.
    inline void checkComplexULPs(
        // The value to be checked.
        const Complex toCheck,
        // The expected value.
        const Complex expected,
        // How many ULPs can each part (re, im) be apart.
        const std::uint64_t toleranceULPs);
}

/* ----------------------------- */
/* -------- Definitions -------- */
/* ----------------------------- */
//
// See the declaration for all relevant information.
inline void hmeigens::test::checkRealULPs(const hmeigens::Scalar toCheck, const hmeigens::Scalar expected, const std::uint64_t toleranceULPs) {
    CAPTURE(toCheck, expected, toleranceULPs);
    CHECK_THAT(
        toCheck,
        Catch::Matchers::WithinULP(expected, toleranceULPs)
    );
    return;
}

// See the declaration for all relevant information.
inline void hmeigens::test::checkComplexULPs(const hmeigens::Complex toCheck, const hmeigens::Complex expected, const std::uint64_t toleranceULPs) {
    CAPTURE(toCheck, expected, toleranceULPs);
    // Real part check.
    hmeigens::test::checkRealULPs(toCheck.real(), expected.real(), toleranceULPs);
    // Imaginary part check.
    hmeigens::test::checkRealULPs(toCheck.imag(), expected.imag(), toleranceULPs);
    return;
}

#endif
