// This benchmark exists to test which of the three matrix multiplication approaches performs best.
// All 3 approaches produce identical matrices. This is guaranteed by the REQUIRE checks.
#include "hmeigens/scalar.hpp"
#include "hmeigens/square_matrix.hpp"
#include "hmeigens/detail/square_matrix_helpers.hpp"

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
using hmeigens::detail::checkSizeMatch;
using hmeigens::benchmark::randomMatrix;
using hmeigens::benchmark::requireEqualMatrices;

// NOTE: In all cases below the complex product is written in the explicit form. See complex_multiplication_bench.cpp.

// This is the classic approach to the matrix multiplication. It simply takes the definition and turns it into code.
// Given P = L*R, then
// P(i,j)=sum_k L(i,k)R(k,j),
// where i, j, k run in the range [0,size[.
// This translates to the loops
// for(i in [0, size[)
//     for(j [0, size[)
//         for(k in [0, size[) P(i,j)+=L(i,k)*R(k,j).
static SquareMatrix classicMult(const SquareMatrix& leftHand, const SquareMatrix& rightHand) {
    const std::size_t size = leftHand.size();
    // Size validation.
    SquareMatrix product{checkSizeMatch(size, rightHand.size(), "matrix multiplication")};
    for (std::size_t i = 0; i < size; ++i) {
        for (std::size_t j = 0; j < size; ++j) {
            for (std::size_t k = 0; k < size; ++k) {
                // Store the values to access only four times rather than eight.
                const auto lTempRe = leftHand(i,k).real();
                const auto lTempIm = leftHand(i,k).imag();
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

// This is an approach to the matrix multiplication optimized for cache preservation and vectorization. The two inner loops are swapped with respect to the classic approach.
// It allows for storing the left-hand matrix's element once per middle-level loop.
// Given P = L*R, then
// P(i,j)=sum_k L(i,k)R(k,j),
// where i, j, k run in the range [0,size[.
// This translates to the loops
// for(i in [0, size[)
//     for(k [0, size[) Temp = L(i,k)
//         for(j in [0, size[) P(i,j)+=Temp*R(k,j).
static SquareMatrix rearrangedMult(const SquareMatrix& leftHand, const SquareMatrix& rightHand) {
    const std::size_t size = leftHand.size();
    // Size validation.
    SquareMatrix product{checkSizeMatch(size, rightHand.size(), "matrix multiplication")};
    for (std::size_t i = 0; i < size; ++i) {
        for (std::size_t k = 0; k < size; ++k) {
            // Store the value of the left-hand matrix.
            // Two accesses per value of k, instead of 2*size ones if done inside the inner loop.
            const auto lTempRe = leftHand(i,k).real();
            const auto lTempIm = leftHand(i,k).imag();
            for (std::size_t j = 0; j < size; ++j) {
                // Store the values to access only two times rather than four.
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

// This is a helper that transposes a matrix in-place. It is used in the third approach.
// Since the third approach was not chosen, this function is not useful in the main project, and thus exists only here.
static SquareMatrix& transposeInPlace(SquareMatrix& matrix) {
    for (std::size_t i = 0; i < matrix.size(); ++i) {
        // Pass through all subdiagonal elements.
        for (std::size_t j = 0; j < i; ++j) {
            // Store the first one.
            auto temp = matrix(i, j);
            // Overwrite the first one with the second one.
            matrix(i, j) = matrix(j, i);
            // Overwrite the second one with the stored temporary.
            matrix(j, i) = temp;
        }
    }
    return matrix;
}

// This is an optimized approach to the matrix multiplication for cache preservation. The right-hand side matrix is transposed, so that the two can be run through in parallel.
// Given P = L*R, T = R transposed, then
// P(i,j)=sum_k L(i,k)R(k,j),
// where i, j, k run in the range [0,size[.
// This translates to the loops
// for(i in [0, size[)
//     for(j [0, size[)
//         for(k in [0, size[) P(i,j)+=L(i,k)*T(j,k).
static SquareMatrix transposeMult(const SquareMatrix& leftHand, const SquareMatrix& rightHand) {
    const std::size_t size = leftHand.size();
    // Size validation.
    SquareMatrix product{checkSizeMatch(size, rightHand.size(), "matrix multiplication")};
    // Copy the right hand, then transpose it.
    SquareMatrix rightHandT = rightHand;
    transposeInPlace(rightHandT);
    for (std::size_t i = 0; i < size; ++i) {
        for (std::size_t j = 0; j < size; ++j) {
            for (std::size_t k = 0; k < size; ++k) {
                // Store the values to access only four times rather than eight.
                const auto lTempRe = leftHand(i,k).real();
                const auto lTempIm = leftHand(i,k).imag();
                const auto rTempRe = rightHandT(j,k).real();
                const auto rTempIm = rightHandT(j,k).imag();
                product(i,j)+=Complex{
                    lTempRe*rTempRe-lTempIm*rTempIm,
                    lTempRe*rTempIm+lTempIm*rTempRe
                };
            }
        }
    }
    return product;
}

static void runMatrixMultBench(std::size_t size) {
    // Seed fixed to default standard to ensure reproducibility.
    std::mt19937_64 rng{std::mt19937_64::default_seed};
    // Get two random matrices.
    const SquareMatrix left = randomMatrix(size, rng);
    const SquareMatrix right = randomMatrix(size, rng);
    // This benchmark only makes sense if the three ways of computing the product produce identical results.
    // Compute the product in the three ways and REQUIRE they are identical value-by-value.
    requireEqualMatrices(classicMult(left, right), rearrangedMult(left, right));
    requireEqualMatrices(transposeMult(left, right), rearrangedMult(left, right));
    // Begin benchmark.
    BENCHMARK("Classic product, size = " + std::to_string(size)) {
        return classicMult(left, right);
    };
    BENCHMARK("Rearranged product, size = " + std::to_string(size)) {
        return rearrangedMult(left, right);
    };
    BENCHMARK("Transpose product, size = " + std::to_string(size)) {
        return transposeMult(left, right);
    };
    return;
}

TEST_CASE("Matrix multiplication benchmark.", "[benchmark][matrix_mult]") {
    runMatrixMultBench(GENERATE(as<std::size_t>{}, 10, 50, 200));
}

TEST_CASE("Matrix multiplication benchmark, large size.", "[.][benchmark][matrix_mult_l]") {
    runMatrixMultBench(GENERATE(as<std::size_t>{}, 1000));
}

TEST_CASE("Matrix multiplication benchmark, very large size.", "[.][benchmark][matrix_mult_xl]") {
    runMatrixMultBench(GENERATE(as<std::size_t>{}, 2000));
}
