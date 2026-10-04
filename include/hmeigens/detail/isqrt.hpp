// File for the integer square root function.
#ifndef HMEIGENS_DETAIL_ISQRT_HPP
#define HMEIGENS_DETAIL_ISQRT_HPP

#include <cstddef>  // For std::size_t

namespace hmeigens::detail {

    // This function computes the integer square root of a given value, i.e. the largest integer root such that
    // root * root <= value.
    [[nodiscard]] constexpr std::size_t isqrt(std::size_t value);
}

/* ------------------------------*/
/* -------- Definitions -------- */
/* ------------------------------*/
//
// This implementation of the integer square root is based on the digit-by-digit algorithm.
// Main source was https://en.wikipedia.org/wiki/Integer_square_root#Using_bitwise_operations, accessed on 2026-09-30.
// This is nothing more than a C++ translation of the recursive pseudocode presented on the page.
// Quoting the pseudocode for reference:
// > def isqrt_recursive(n: int) -> int:
// >    assert n >= 0, "n must be a non-negative integer"
// >    if n < 2: return n
// >
// >    # Recursive call:
// >    small_cand = isqrt_recursive(n >> 2) << 1 # same as 2 * isqrt(n // 4)
// >    large_cand = small_cand + 1
// >    if large_cand * large_cand > n: return small_cand
// >    else: return large_cand
constexpr std::size_t hmeigens::detail::isqrt(std::size_t value) {
    // The pseudocode guards against negative values. Using std::size_t makes this unnecessary.
    // 0 and 1 are fine as is.
    if (value < 2) {
        return value;
    }
    // The small candidate is double the root of the quarter, so first divide by 4, then multiply by 2.
    // A quarter of the value has half the root.
    const std::size_t smallCandidate = hmeigens::detail::isqrt(value >> 2) << 1;
    // But the right bitwise shifts lose the tail, so it could also be this.
    const std::size_t largeCandidate = smallCandidate + 1;
    // Decide which candidate is good. If the larger one is over the value, return the small one.
    // Computing largeCandidate * largeCandidate is always a safe operation.
    // Since smallCandidate is even by construction, largeCandidate is odd.
    // Say the maximum std::size_t is 111111(2) (odd).
    // Its isqrt is 000111(2) (odd), which squares to 110001(2) (odd).
    // In this case smallCandidate would be 000110(2), i.e. the largest possible smallCandidate, and thus no value of largeCandidate can overflow.
    // This holds for an arbitrary number of bits.
    return largeCandidate * largeCandidate > value ? smallCandidate : largeCandidate;
}

#endif
