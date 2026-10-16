// Header-only collection of functions used for benchmarking.
// Contents:
// - hmeigens::benchmark::randomMatrix;
// - hmeigens::benchmark::requireEqualMatrices.
#ifndef HMEIGENS_BENCHMARK_HELPERS_HPP
#define HMEIGENS_BENCHMARK_HELPERS_HPP

#include "hmeigens/scalar.hpp"
#include "hmeigens/square_matrix.hpp"

#include <cstddef>       // for std::size_t
#include <random>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

namespace hmeigens::benchmark {
    // This helper builds a random matrix with a given size.
    [[nodiscard]] SquareMatrix randomMatrix(std::size_t size, std::mt19937_64& rng);

    // This helper verifies via the REQUIRE and REQUIRE_THAT macros that two matrices are "equal", i.e. that they match element-by-element at 0 ULPs.
    void requireEqualMatrices(const SquareMatrix& firstMatrix, const SquareMatrix& secondMatrix);
}

/* ----------------------------- */
/* -------- Definitions -------- */
/* ----------------------------- */
//
// See the declaration for all relevant information.
inline hmeigens::SquareMatrix hmeigens::benchmark::randomMatrix(std::size_t size, std::mt19937_64& rng) {
    hmeigens::SquareMatrix matrix{size};
    std::uniform_real_distribution<hmeigens::Scalar> dist(HMEIGENS_MAKE_SCALAR(-1.), HMEIGENS_MAKE_SCALAR(1.));
    for (std::size_t i = 0; i < size; ++i) {
        for (std::size_t j = 0; j < size; ++j) {
            matrix(i,j) = hmeigens::Complex{dist(rng), dist(rng)};
        }
    }
    return matrix;
}

// See the declaration for all relevant information.
inline void hmeigens::benchmark::requireEqualMatrices(const hmeigens::SquareMatrix& firstMatrix, const hmeigens::SquareMatrix& secondMatrix) {
    const std::size_t size = firstMatrix.size();
    // Sizes must match.
    REQUIRE(size == secondMatrix.size());
    // Run over each matrix, and require both the real and imaginary part of each element match at 0 ULPs.
    for (std::size_t i = 0; i < size; ++i) {
        for (std::size_t j = 0; j < size; ++j) {
            CAPTURE(i, j, firstMatrix(i, j), secondMatrix(i, j));
            REQUIRE_THAT(
                firstMatrix(i, j).real(),
                Catch::Matchers::WithinULP(secondMatrix(i, j).real(), 0)
            );
            REQUIRE_THAT(
                firstMatrix(i, j).imag(),
                Catch::Matchers::WithinULP(secondMatrix(i, j).imag(), 0)
            );
        }
    }
    return;
}

#endif
