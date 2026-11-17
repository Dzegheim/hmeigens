// Test file for eigenvalues_solver.hpp/.cpp.
// Sections:
// - helpers;
// - test cases:
//      - hmeigens::makeUpperHessenbergInPlace;
//      - hmeigens::makeUpperHessenberg.
#include "hmeigens/eigenvalues_solver.hpp"
#include "hmeigens/tolerances.hpp"
#include "hmeigens/square_matrix.hpp"

#include "comparison_helpers.hpp"

#include <cstddef>       // For std::size_t
#include <cstdint>       // For std::uint64_t

#include <catch2/catch_test_macros.hpp>

// Readability of tests.
using hmeigens::test::checkRealULPs;
using hmeigens::test::checkComplexULPs;
#define HMS HMEIGENS_MAKE_SCALAR

/* --------------------------------------------- */
/* ------------------ Helpers ------------------ */
/* --------------------------------------------- */
//
// This helper checks that two matrices are element-by-element within toleranceULPs of each other.
static void checkMatrixULPs(const hmeigens::SquareMatrix& toCheck, const hmeigens::SquareMatrix& expected, std::uint64_t toleranceULPs) {
    REQUIRE(toCheck.size() == expected.size());
    for (std::size_t row = 0; row < toCheck.size(); ++row) {
        for (std::size_t col = 0; col < toCheck.size(); ++col) {
            CAPTURE(row, col);
            checkComplexULPs(
                toCheck(row, col),
                expected(row, col),
                toleranceULPs
            );
        }
    }
    return;
}

/* --------------------------------------------- */
/* ---------------- Test cases ----------------- */
/* --------------------------------------------- */
//
/* --------------------------------------------- */
/* --- hmeigens::makeUpperHessenbergInPlace ---- */
/* --------------------------------------------- */
//
TEST_CASE("Eigenvalues solver test: in place upper Hessenberg transformation is the correct shape.", "[eigenvalues_solver][makeUpperHessenbergInPlace]") {
    SKIP("This test will be implemented soon.");
}

/* --------------------------------------------- */
/* ------- hmeigens::makeUpperHessenberg ------- */
/* --------------------------------------------- */
//
TEST_CASE("Eigenvalues solver test: the copying upper Hessenberg transformation behaves exactly like the in place version without altering the input matrix.", "[eigenvalues_solver][makeUpperHessenberg]") {
    // GIVEN a matrix
    // WHEN  hmeigens::makeUpperHessenberg acts on it
    // THEN  the returned matrix is identical to what would be produced by hmeigens::makeUpperHessenbergInPlace and the input is unchanged
    //
    // Tolerance is 0 ULPs on both checks:
    // - the first is to ensure the input is completely unchanged;
    // - the second is to ensure the behaviour is identical.
    //
    hmeigens::SquareMatrix testMatrix{
        {{HMS(-2.), HMS(3.)},  {HMS(.25), HMS(-.5)},  {HMS(4.), HMS(1.)},
         {HMS(3.), HMS(-1.)},  {HMS(-.375), HMS(2.)}, {HMS(.5), HMS(.75)},
         {HMS(-4.), HMS(-2.)}, {HMS(.875), HMS(1.)},  {HMS(2.), HMS(-.25)}}
    };
    const auto copyMatrix = testMatrix;
    const auto result = hmeigens::makeUpperHessenberg(testMatrix);
    // Check that the input is unchanged.
    checkMatrixULPs(testMatrix, copyMatrix, 0);
    // Now testMatrix can be changed, and recycling it spares a copy.
    hmeigens::makeUpperHessenbergInPlace(testMatrix);
    checkMatrixULPs(result, testMatrix, 0);
}
