// Test file for eigenvalues_solver.hpp/.cpp.
// Sections:
// - helpers;
// - test cases:
//      - hmeigens::makeUpperHessenbergInPlace;
//      - hmeigens::makeUpperHessenberg.
#include "hmeigens/eigenvalues_solver.hpp"
#include "hmeigens/predicates.hpp"
#include "hmeigens/tolerances.hpp"
#include "hmeigens/square_matrix.hpp"

#include "comparison_helpers.hpp"

#include <cstddef>       // For std::size_t
#include <cstdint>       // For std::uint64_t

#include <catch2/catch_test_macros.hpp>

// Readability of tests.
using hmeigens::test::checkComplexULPs;
using hmeigens::test::checkComplexAbs;
using hmeigens::test::checkRealAbs;
#define HMS HMEIGENS_MAKE_SCALAR

/* --------------------------------------------- */
/* ------------------ Helpers ------------------ */
/* --------------------------------------------- */
//
// This helper checks that two matrices are element-by-element within toleranceULPs of each other.
static void checkMatrixULPs(const hmeigens::SquareMatrix& toCheck, const hmeigens::SquareMatrix& expected, const std::uint64_t toleranceULPs) {
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
TEST_CASE("Eigenvalues solver test: 1x1 and 2x2 matrices are untouched by the in-place upper Hessenberg transformation.", "[eigenvalues_solver][makeUpperHessenbergInPlace]") {
    // GIVEN a 1x1 or 2x2 matrix
    // WHEN  hmeigens::makeUpperHessenbergInPlace acts on it
    // THEN  the matrix is untouched
    //
    // 0 ULPs tolerance. The matrix has no work done on it.
    hmeigens::SquareMatrix testMatrix1{{{HMS(6.7), HMS(4.2)}}};
    const auto copyMatrix1 = testMatrix1;
    checkMatrixULPs(hmeigens::makeUpperHessenbergInPlace(testMatrix1), copyMatrix1, 0);
    hmeigens::SquareMatrix testMatrix2{
        {{HMS(6.), HMS(7.)}, {HMS(4.), HMS(2.)},
         {HMS(1000000.), HMS(0.)}, {HMS(1.e-5), HMS(-6.7)}}
    };
    const auto copyMatrix2 = testMatrix2;
    checkMatrixULPs(hmeigens::makeUpperHessenbergInPlace(testMatrix2), copyMatrix2, 0);
}

TEST_CASE("Eigenvalues solver test: upper Hessenberg matrices are untouched by the in-place upper Hessenberg transformation.", "[eigenvalues_solver][makeUpperHessenbergInPlace]") {
    // GIVEN an upper Hessenberg matrix
    // WHEN  hmeigens::makeUpperHessenbergInPlace acts on it
    // THEN  the matrix is untouched
    //
    // 0 ULPs tolerance. The matrix has no work done on it.
    hmeigens::SquareMatrix testMatrix{
        {{HMS(1.), HMS(1.)}, {HMS(2.), HMS(2.)},   {HMS(3.), HMS(3.)},   {HMS(4.), HMS(4.)},
         {HMS(5.), HMS(5.)}, {HMS(6.), HMS(6.)},   {HMS(7.), HMS(7.)},   {HMS(8.), HMS(8.)},
         {HMS(0.), HMS(0.)}, {HMS(10.), HMS(10.)}, {HMS(11.), HMS(11.)}, {HMS(12.), HMS(12.)},
         {HMS(0.), HMS(0.)}, {HMS(0.), HMS(0.)},   {HMS(15.), HMS(15.)}, {HMS(16.), HMS(16.)}}
    };
    const auto copyMatrix = testMatrix;
    checkMatrixULPs(hmeigens::makeUpperHessenbergInPlace(testMatrix), copyMatrix, 0);
}

TEST_CASE("Eigenvalues solver test: in-place upper Hessenberg transformation is the correct shape.", "[eigenvalues_solver][makeUpperHessenbergInPlace]") {
    // GIVEN a matrix
    // WHEN  hmeigens::makeUpperHessenbergInPlace acts on it
    // THEN  the matrix is exactly upper Hessenberg
    //
    // Tolerance here is 0 ULPs, as the function writes exact 0.s in the columns. The correctness of those zeroes is guaranteed by the tests of hmeigens::detail::makeHouseholderInPlace.
    //
    hmeigens::SquareMatrix testMatrix{
        {{HMS(-2.), HMS(3.)},  {HMS(.25), HMS(-.5)},  {HMS(4.), HMS(1.)},
         {HMS(3.), HMS(-1.)},  {HMS(-.375), HMS(2.)}, {HMS(.5), HMS(.75)},
         {HMS(-4.), HMS(-2.)}, {HMS(.875), HMS(1.)},  {HMS(2.), HMS(-.25)}}
    };
    CHECK(hmeigens::isUpperHessenberg(hmeigens::makeUpperHessenbergInPlace(testMatrix), 0));
    hmeigens::SquareMatrix testMatrixLarge{
        {{HMS(1.), HMS(-1.)}, {HMS(2.), HMS(-.02)}, {HMS(3.), HMS(-.3)}, {HMS(-4.), HMS(.4)}, {HMS(-5.), HMS(5.)}, {HMS(-6.), HMS(.06)},
         {HMS(4.), HMS(-.4)}, {HMS(5.), HMS(-5.)}, {HMS(16.), HMS(-1.06)}, {HMS(14.), HMS(-1.4)}, {HMS(15.), HMS(-15.)}, {HMS(16.), HMS(-1.06)},
         {HMS(7.), HMS(-.07)}, {HMS(8.), HMS(-.8)}, {HMS(9.), HMS(-9.)}, {HMS(-.07), HMS(7.)}, {HMS(-.8), HMS(8.)}, {HMS(-9.), HMS(9.)},
         {HMS(9.), HMS(-.8)}, {HMS(7.), HMS(-6.)}, {HMS(25.), HMS(-4.06)}, {HMS(13.), HMS(-2.2)}, {HMS(11.), HMS(-11.)}, {HMS(10.), HMS(-0.06)},
         {HMS(44.), HMS(-.44)}, {HMS(45.), HMS(-45.)}, {HMS(46.), HMS(-1.46)}, {HMS(14.4), HMS(-1.44)}, {HMS(15.4), HMS(-15.4)}, {HMS(16.4), HMS(-1.064)},
         {HMS(24.), HMS(-3.4)}, {HMS(15.), HMS(-45.)}, {HMS(66.), HMS(-7.06)}, {HMS(14.8), HMS(-1.49)}, {HMS(15.1), HMS(-15.2)}, {HMS(16.3), HMS(-1.006)}}
    };
    CHECK(hmeigens::isUpperHessenberg(hmeigens::makeUpperHessenbergInPlace(testMatrixLarge), 0));
    // A matrix with an already ok column, but not the others.
    testMatrixLarge = hmeigens::SquareMatrix{
        {{HMS(1.), HMS(-1.)}, {HMS(2.), HMS(-.02)}, {HMS(3.), HMS(-.3)}, {HMS(-4.), HMS(.4)}, {HMS(-5.), HMS(5.)}, {HMS(-6.), HMS(.06)},
         {HMS(0.), HMS(0.)}, {HMS(5.), HMS(-5.)}, {HMS(16.), HMS(-1.06)}, {HMS(14.), HMS(-1.4)}, {HMS(15.), HMS(-15.)}, {HMS(16.), HMS(-1.06)},
         {HMS(0.), HMS(0.)}, {HMS(8.), HMS(-.8)}, {HMS(9.), HMS(-9.)}, {HMS(-.07), HMS(7.)}, {HMS(-.8), HMS(8.)}, {HMS(-9.), HMS(9.)},
         {HMS(0.), HMS(0.)}, {HMS(7.), HMS(-6.)}, {HMS(25.), HMS(-4.06)}, {HMS(13.), HMS(-2.2)}, {HMS(11.), HMS(-11.)}, {HMS(10.), HMS(-0.06)},
         {HMS(0.), HMS(0.)}, {HMS(45.), HMS(-45.)}, {HMS(46.), HMS(-1.46)}, {HMS(14.4), HMS(-1.44)}, {HMS(15.4), HMS(-15.4)}, {HMS(16.4), HMS(-1.064)},
         {HMS(0.), HMS(0.)}, {HMS(15.), HMS(-45.)}, {HMS(66.), HMS(-7.06)}, {HMS(14.8), HMS(-1.49)}, {HMS(15.1), HMS(-15.2)}, {HMS(16.3), HMS(-1.006)}}
    };
    CHECK(hmeigens::isUpperHessenberg(hmeigens::makeUpperHessenbergInPlace(testMatrixLarge), 0));
}

TEST_CASE("Eigenvalues solver test: the in-place upper Hessenberg transformation returns the correct reference.", "[eigenvalues_solver][makeUpperHessenbergInPlace]") {
    // GIVEN a matrix
    // WHEN  hmeigens::makeUpperHessenbergInPlace acts on it
    // THEN  the returned reference matches the one of the matrix it was called on
    hmeigens::SquareMatrix testMatrix{
        {{HMS(-2.), HMS(3.)},  {HMS(.25), HMS(-.5)},  {HMS(4.), HMS(1.)},
         {HMS(3.), HMS(-1.)},  {HMS(-.375), HMS(2.)}, {HMS(.5), HMS(.75)},
         {HMS(-4.), HMS(-2.)}, {HMS(.875), HMS(1.)},  {HMS(2.), HMS(-.25)}}
    };
    CHECK(&testMatrix == &hmeigens::makeUpperHessenbergInPlace(testMatrix));
}

/* ----------------------------- */
/* --------- IMPORTANT --------- */
/* ----------------------------- */
//
// The two test cases below together verify that a matrix that has been processed by hmeigens::makeUpperHessenbergInPlace is similar to the input.
// The first check verifies that the transformation is unitary by checking that the norm is unchanged within tolerance.
// The second check verifies that the eigenvalues are the same via Newton's identities (https://en.wikipedia.org/wiki/Newton%27s_identities).
// In particular:
// "The eigenvalues of a power of a matrix are the corresponding power of the eigenvalues of the matrix."
//
TEST_CASE("Eigenvalues solver test: the in-place upper Hessenberg transformation preserves the norm.", "[eigenvalues_solver][makeUpperHessenbergInPlace]") {
    // GIVEN a matrix
    // WHEN  hmeigens::makeUpperHessenbergInPlace acts on it
    // THEN  the norm before and after the operation is the same within tolerance
    //
    // Smallest meaningful size.
    hmeigens::SquareMatrix testMatrix{
        {{HMS(-2.), HMS(3.)},  {HMS(.25), HMS(-.5)},  {HMS(4.), HMS(1.)},
         {HMS(3.), HMS(-1.)},  {HMS(-.375), HMS(2.)}, {HMS(.5), HMS(.75)},
         {HMS(-4.), HMS(-2.)}, {HMS(.875), HMS(1.)},  {HMS(2.), HMS(-.25)}}
    };
    const auto expectedNorm = testMatrix.norm();
    const auto tolerance = hmeigens::normPreservationTolerance(testMatrix);
    hmeigens::makeUpperHessenbergInPlace(testMatrix);
    checkRealAbs(
        testMatrix.norm(),
        expectedNorm,
        tolerance
    );
    // Larger size. Requires multiple reductions.
    hmeigens::SquareMatrix testMatrixLarge{
        {{HMS(1.), HMS(-1.)}, {HMS(2.), HMS(-.02)}, {HMS(3.), HMS(-.3)}, {HMS(-4.), HMS(.4)}, {HMS(-5.), HMS(5.)}, {HMS(-6.), HMS(.06)},
         {HMS(4.), HMS(-.4)}, {HMS(5.), HMS(-5.)}, {HMS(16.), HMS(-1.06)}, {HMS(14.), HMS(-1.4)}, {HMS(15.), HMS(-15.)}, {HMS(16.), HMS(-1.06)},
         {HMS(7.), HMS(-.07)}, {HMS(8.), HMS(-.8)}, {HMS(9.), HMS(-9.)}, {HMS(-.07), HMS(7.)}, {HMS(-.8), HMS(8.)}, {HMS(-9.), HMS(9.)},
         {HMS(9.), HMS(-.8)}, {HMS(7.), HMS(-6.)}, {HMS(25.), HMS(-4.06)}, {HMS(13.), HMS(-2.2)}, {HMS(11.), HMS(-11.)}, {HMS(10.), HMS(-0.06)},
         {HMS(44.), HMS(-.44)}, {HMS(45.), HMS(-45.)}, {HMS(46.), HMS(-1.46)}, {HMS(14.4), HMS(-1.44)}, {HMS(15.4), HMS(-15.4)}, {HMS(16.4), HMS(-1.064)},
         {HMS(24.), HMS(-3.4)}, {HMS(15.), HMS(-45.)}, {HMS(66.), HMS(-7.06)}, {HMS(14.8), HMS(-1.49)}, {HMS(15.1), HMS(-15.2)}, {HMS(16.3), HMS(-1.006)}}
    };
    const auto expectedNormLarge = testMatrixLarge.norm();
    const auto toleranceLarge = hmeigens::normPreservationTolerance(testMatrixLarge);
    hmeigens::makeUpperHessenbergInPlace(testMatrixLarge);
    checkRealAbs(
        testMatrixLarge.norm(),
        expectedNormLarge,
        toleranceLarge
    );
}

TEST_CASE("Eigenvalues solver test: the in-place upper Hessenberg transformation preserves the eigenvalues.", "[eigenvalues_solver][makeUpperHessenbergInPlace]") {
    SKIP("This case will be implemented soon.");
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

#undef HMS
