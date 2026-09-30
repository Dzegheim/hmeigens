// File for the integer square root function.
#ifndef HMEIGENS_DETAIL_ISQRT_HPP
#define HMEIGENS_DETAIL_ISQRT_HPP

#include <cstddef>  // For std::size_t

namespace hmeigens::detail {
    // This function computes the integer square root of a given value, i.e. the largest integer root such that
    // root * root <= value.
    [[nodiscard]] std::size_t isqrt (std::size_t value);
}

#endif
