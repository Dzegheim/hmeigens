// File for function used to convert strings into floating-point numbers.

#ifndef HMEIGENS_DETAIL_TOSCALAR_HPP
#define HMEIGENS_DETAIL_TOSCALAR_HPP

#include "hmeigens/scalar.hpp"

#include <string_view>

namespace hmeigens::detail {

    // This function converts a string_view to a corresponding hmeigens::Scalar.
    // Since std::from_chars has no locale, there is no risk that someone changing the decimal separator affects the way the code reads the numbers.
    // The toConvert parameter is what will be turned into a floating-point.
    // The fullInput parameter is the entire string containing toConvert, and is only used to provide accurate diagnostic messages in case of errors.
    [[nodiscard]] Scalar toScalar (std::string_view toConvert, std::string_view fullInput);
}

#endif
