// This benchmark exists to test whether, and by how much, performing the complex multiplication explicitly improves the performance versus using the std::operator* provided by <complex>.
// In this code the guard against NaN or infinity is not needed. The matrix multiplication requires finite elements to be meaningful.
// Since the checks may be present in the operator provided by the standard header, skipping them saves time.
#include "hmeigens/scalar.hpp"
#include "hmeigens/square_matrix.hpp"

#include "benchmark_helpers.hpp"

#include <cstddef>       // for std::size_t
#include <complex>
#include <random>
#include <string>

#include <catch2/catch_test_macros.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/generators/catch_generators.hpp>

// Readability
using hmeigens::SquareMatrix;
using hmeigens::Complex;
using hmeigens::benchmark::randomMatrix;
using hmeigens::benchmark::requireEqualMatrices;

// This version uses std::operator* from <complex>.
static SquareMatrix stdComplexMultLoop(const SquareMatrix& leftHand, const SquareMatrix& rightHand) {
    const std::size_t size = leftHand.size();
    // Skip size validation.
    SquareMatrix product{size};
    for (std::size_t i = 0; i < size; ++i) {
        for (std::size_t k = 0; k < size; ++k) {
            const auto temp = leftHand(i,k);
            for (std::size_t j = 0; j < size; ++j) {
                product(i,j)+=temp*rightHand(k,j);
            }
        }
    }
    return product;
}

// This version uses the manual explicit complex product:
// (a+ib)*(c+id) = (ac-db)+(ad+bc)i.
static SquareMatrix manualMultLoop(const SquareMatrix& leftHand, const SquareMatrix& rightHand) {
    const std::size_t size = leftHand.size();
    // Skip size validation.
    SquareMatrix product{size};
    for (std::size_t i = 0; i < size; ++i) {
        for (std::size_t k = 0; k < size; ++k) {
            const auto lTempRe = leftHand(i,k).real();
            const auto lTempIm = leftHand(i,k).imag();
            for (std::size_t j = 0; j < size; ++j) {
                const auto rTempRe = rightHand(k,j).real();
                const auto rTempIm = rightHand(k,j).imag();
                product(i,j)+=Complex{
                    lTempRe*rTempRe-lTempIm*rTempIm,
                    lTempRe*rTempIm+lTempIm*rTempRe
                };
            }
        }
    }
    return product;
}

TEST_CASE("Complex multiplication benchmark.", "[benchmark][complex_mult]") {
    const std::size_t size = GENERATE(as<std::size_t>{}, 10, 50, 200);
    // Seed fixed to default standard to ensure reproducibility.
    std::mt19937_64 rng{std::mt19937_64::default_seed};
    // Get two random matrices.
    const SquareMatrix left = randomMatrix(size, rng);
    const SquareMatrix right = randomMatrix(size, rng);
    // This benchmark only makes sense if the two ways of computing the product produce identical results.
    // Compute the product in the two ways and REQUIRE they are identical value-by-value.
    requireEqualMatrices(stdComplexMultLoop(left, right), manualMultLoop(left, right));
    // Begin benchmark.
    BENCHMARK("Standard product, size = " + std::to_string(size)) {
        return stdComplexMultLoop(left, right);
    };
    BENCHMARK("Manual product, size = " + std::to_string(size)) {
        return manualMultLoop(left, right);
    };
}
