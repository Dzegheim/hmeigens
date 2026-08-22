#ifndef HMEIGENS_CONSTANTS_HPP
#define HMEIGENS_CONSTANTS_HPP

#include "hmeigens/scalar.hpp"

#include <limits>

namespace hmeigens {

    // The difference between 1.0 and the next Scalar. 
    // Values: 1.19209e-07 float, 2.22045e-16 double, 1.0842e-19 long double.
    // Source: https://en.cppreference.com/cpp/types/climits.
    inline constexpr hmeigens::Scalar epsilon = std::numeric_limits<Scalar>::epsilon();

    // Tolerance for parsing.
    // The parsed value must be within 1 ULP of the input.
    // No arithmetic is performed on the value, so tolerance is the tightest each type allows.
    inline constexpr hmeigens::Scalar parseTolerance = epsilon;
}

#endif