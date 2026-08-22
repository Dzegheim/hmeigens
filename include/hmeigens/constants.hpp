#ifndef HMEIGENS_CONSTANTS_HPP
#define HMEIGENS_CONSTANTS_HPP

#include "hmeigens/scalar.hpp"

#include <limits>

namespace hmeigens {

    // The difference between 1.0 and the next Scalar. 
    // Values: 1.19209e-07 float, 2.22045e-16 double.
    // Source: https://en.cppreference.com/cpp/types/climits.
    inline constexpr hmeigens::Scalar epsilon = std::numeric_limits<Scalar>::epsilon();

    // Tolerance for parsing.
    // The parsed value must be within 1 ULP of the input.
    // No arithmetic is performed on the value, so tolerance is the tightest each type allows.
    // SOURCE: https://en.cppreference.com/cpp/utility/from_chars
    // SOURCE: "In any case, the resulting value is one of at most two floating-point values closest to the value of the string matching the pattern, after rounding according to std::round_to_nearest."
    inline constexpr hmeigens::Scalar parseTolerance = epsilon;
}

#endif
