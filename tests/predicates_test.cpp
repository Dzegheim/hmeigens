// Test file for predicates.hpp/.cpp.
// Sections:
// - constants;
// - test cases:
//     - hmeigens::isFinite;
//     - hmeigens::getTolerance;
//     - hmeigens::isUpperHessenberg;
//     - hmeigens::isUpperTriangular.

#include "hmeigens/scalar.hpp"
#include "hmeigens/square_matrix.hpp"
#include "hmeigens/predicates.hpp"
#include "hmeigens/tolerances.hpp"

#include "comparison_helpers.hpp"

#include <limits>    // For std::numeric_limits
#include <cmath>     // For std::sqrt, std::nextafter
#include <complex>   // For std::abs

#include <catch2/catch_test_macros.hpp>

/* --------------------------------------------- */
/* ----------------- Constants ----------------- */
/* --------------------------------------------- */
//
// For readability of tests.
constexpr hmeigens::Scalar NaN = std::numeric_limits<hmeigens::Scalar>::quiet_NaN();
constexpr hmeigens::Scalar Inf = std::numeric_limits<hmeigens::Scalar>::infinity();
constexpr hmeigens::Scalar max = std::numeric_limits<hmeigens::Scalar>::max();
constexpr hmeigens::Scalar lowest = std::numeric_limits<hmeigens::Scalar>::lowest();

// Local brief alias for the conversion macro to improve legibility.
#define HMS HMEIGENS_MAKE_SCALAR

// Readability of tests.
using hmeigens::test::checkRealULPs;

/* ------------------------------------------------ */
/* ------------------ Test cases ------------------ */
/* ------------------------------------------------ */
//
/* ------------------------------------------------ */
/* -------------- hmeigens::isFinite -------------- */
/* ------------------------------------------------ */
//
TEST_CASE("Predicates test: a finite matrix is correctly reported as such.", "[predicates][isFinite]") {
    // GIVEN a matrix whose elements are all finite
    // WHEN  hmeigens::isFinite checks it
    // THEN  the function returns true
    //
    // Null matrix.
    CHECK(hmeigens::isFinite(hmeigens::SquareMatrix{1}));
    // Non-null matrix.
    const hmeigens::SquareMatrix testMatrix{
        {{HMS(1.), HMS(-1.)}, {HMS(2.), HMS(-2.)}, {HMS(3.), HMS(-3.)},
         {HMS(4.), HMS(-4.)}, {HMS(5.), HMS(-5.)}, {HMS(6.), HMS(-6.)},
         {HMS(7.), HMS(-7.)}, {HMS(8.), HMS(-8.)}, {HMS(9.), HMS(-9.)}}
    };
    CHECK(hmeigens::isFinite(testMatrix));
    // Matrix with huge values.
    CHECK(hmeigens::isFinite(hmeigens::SquareMatrix{{{max, lowest}}}));
}

TEST_CASE("Predicates test: a matrix with non-finite elements is correctly reported as such.", "[predicates][isFinite]") {
    // GIVEN a matrix whose elements are not all finite
    // WHEN  hmeigens::isFinite checks it
    // THEN  the function returns false
    //
    // Infinities.
    // Real positive.
    const hmeigens::SquareMatrix infinity1{
        {{Inf    , HMS(0.)}, {HMS(0.), HMS(0.)},
         {HMS(0.), HMS(0.)}, {HMS(0.), HMS(0.)}}
    };
    // Real negative.
    const hmeigens::SquareMatrix infinity2{
        {{HMS(0.), HMS(0.)}, {-Inf   , HMS(0.)},
         {HMS(0.), HMS(0.)}, {HMS(0.), HMS(0.)}}
    };
    // Imaginary positive.
    const hmeigens::SquareMatrix infinity3{
        {{HMS(0.), HMS(0.)}, {HMS(0.), HMS(0.)},
         {HMS(0.), Inf    }, {HMS(0.), HMS(0.)}}
    };
    // Imaginary negative.
    const hmeigens::SquareMatrix infinity4{
        {{HMS(0.), HMS(0.)}, {HMS(0.), HMS(0.)},
         {HMS(0.), HMS(0.)}, {HMS(0.), -Inf   }}
    };
    CHECK_FALSE(hmeigens::isFinite(infinity1));
    CHECK_FALSE(hmeigens::isFinite(infinity2));
    CHECK_FALSE(hmeigens::isFinite(infinity3));
    CHECK_FALSE(hmeigens::isFinite(infinity4));
    //
    // NaNs.
    // Real.
    const hmeigens::SquareMatrix nan1{
        {{HMS(0.), HMS(0.)}, {HMS(0.), HMS(0.)},
         {NaN    , HMS(0.)}, {HMS(0.), HMS(0.)}}
    };
    // Imaginary.
    const hmeigens::SquareMatrix nan2{
        {{HMS(0.), HMS(0.)}, {HMS(0.), HMS(0.)},
         {HMS(0.), HMS(0.)}, {HMS(0.), NaN   }}
    };
    CHECK_FALSE(hmeigens::isFinite(nan1));
    CHECK_FALSE(hmeigens::isFinite(nan2));
}

/* ------------------------------------------------ */
/* ------------ hmeigens::getTolerance ------------ */
/* ------------------------------------------------ */
//
TEST_CASE("Predicates test: tolerance is computed as expected.", "[predicates][getTolerance]") {
    // GIVEN a matrix
    // WHEN  hmeigens::getTolerance computes its tolerance
    // THEN  it returns the expected value within its own tolerance
    //
    // Null matrix.
    checkRealULPs(
        hmeigens::getTolerance(hmeigens::SquareMatrix{
            {{HMS(0.), HMS(0.)}, {HMS(0.), HMS(0.)}, {HMS(0.), HMS(0.)},
             {HMS(0.), HMS(0.)}, {HMS(0.), HMS(0.)}, {HMS(0.), HMS(0.)},
             {HMS(0.), HMS(0.)}, {HMS(0.), HMS(0.)}, {HMS(0.), HMS(0.)}}
        }),
        HMS(0.),
        // Tolerance is 0, as this needs to be exact no matter the size.
        0
    );
    // Matrix with size that is a power of 2, and the only non-zero element that is a power of 2.
    checkRealULPs(
        hmeigens::getTolerance(hmeigens::SquareMatrix{
            {{HMS(2.), HMS(0.)}, {HMS(0.), HMS(0.)},
             {HMS(0.), HMS(0.)}, {HMS(0.), HMS(0.)}}
        }),
        // Here size^2*norm = 4.*2. = 8.
        hmeigens::epsilon*hmeigens::toleranceK*HMS(8.),
        // Tolerance here needs no extra, as the multiplications all include a power of 2.
        hmeigens::toleranceTolerance(2)
    );
    // Generic 3x3 matrix.
    checkRealULPs(
        hmeigens::getTolerance(hmeigens::SquareMatrix{
            {{HMS(1.), HMS(0.)}, {HMS(0.), HMS(2.)}, {HMS(0.), HMS(3.)},
             {HMS(4.), HMS(4.)}, {HMS(5.), HMS(.5)}, {HMS(-6.), HMS(-6.)},
             {HMS(7.), HMS(0.)}, {HMS(0.), HMS(8.)}, {HMS(-9.), HMS(0.)}}
        }),
        // Here size^2 = 9 and norm = sqrt(337.25).
        hmeigens::epsilon*hmeigens::toleranceK*HMS(9.)*std::sqrt(HMS(337.25)),
        // Tolerance here needs extra = 3, from 2 multiplications and 1 use of std::sqrt.
        hmeigens::toleranceTolerance(3, 3)
    );
}

/* ------------------------------------------------ */
/* ---------- hmeigens::isUpperHessenberg --------- */
/* ------------------------------------------------ */
//
TEST_CASE("Predicates test: an upper Hessenberg matrix is correctly identified.", "[predicates][isUpperHessenberg]") {
    // GIVEN a matrix in upper Hessenberg form
    // WHEN  hmeigens::isUpperHessenberg checks it
    // THEN  it returns true
    //
    // Trivially Hessenberg matrices.
    // Null matrix.
    CHECK(
        hmeigens::isUpperHessenberg(hmeigens::SquareMatrix{
            {{HMS(0.), HMS(0.)}, {HMS(0.), HMS(0.)}, {HMS(0.), HMS(0.)},
             {HMS(0.), HMS(0.)}, {HMS(0.), HMS(0.)}, {HMS(0.), HMS(0.)},
             {HMS(0.), HMS(0.)}, {HMS(0.), HMS(0.)}, {HMS(0.), HMS(0.)}}
        },
        // Tolerance is 0., as this needs to be exact no matter the size.
        HMS(0.)
    ));
    // For 1x1 and 2x2 matrices the implementation does not perform any checks, so even with large elements and a 0. tolerance the check must pass.
    // A 1x1 matrix.
    CHECK(
        hmeigens::isUpperHessenberg(hmeigens::SquareMatrix{
            {{HMS(1.2345), HMS(67.89)}}
        },
        HMS(0.)
    ));
    // A 2x2 matrix.
    CHECK(
        hmeigens::isUpperHessenberg(hmeigens::SquareMatrix{
            {{HMS(1e6), HMS(1e7)},   {HMS(1e11), HMS(1e12)},
             {HMS(-1e6), HMS(-1e7)}, {HMS(-1e11), HMS(-1e12)}}
        },
        HMS(0.)
    ));
    // Non-trivially Hessenberg matrices.
    // An exact 0. in the required spot.
    const hmeigens::SquareMatrix testMatrix1{
        {{HMS(1.), HMS(1.)}, {HMS(2.), HMS(2.)}, {HMS(3.), HMS(3.)},
         {HMS(4.), HMS(4.)}, {HMS(5.), HMS(5.)}, {HMS(6.), HMS(6.)},
         {HMS(0.), HMS(0.)}, {HMS(8.), HMS(8.)}, {HMS(9.), HMS(9.)}}
    };
    CHECK(hmeigens::isUpperHessenberg(
        testMatrix1,
        hmeigens::getTolerance(testMatrix1)
    ));
    // Exactly the tolerance in the required spot.
    const hmeigens::SquareMatrix testMatrix2{
        {{HMS(1.), HMS(1.)}, {HMS(2.), HMS(2.)}, {HMS(3.), HMS(3.)},
         {HMS(4.), HMS(4.)}, {HMS(5.), HMS(5.)}, {HMS(6.), HMS(6.)},
         {HMS(7.), HMS(7.)}, {HMS(8.), HMS(8.)}, {HMS(9.), HMS(9.)}}
    };
    // This check verifies that a number exactly equal to the tolerance counts as 0., no matter how large its modulus is.
    // In this specific case it is larger than that of most other elements of the matrix itself.
    CHECK(hmeigens::isUpperHessenberg(
        testMatrix2,
        std::abs(testMatrix2(2,0))
    ));
    // A larger matrix with "small enough to be zero" values in the required spots.
    const hmeigens::SquareMatrix testMatrix3{
        {{HMS(1.), HMS(1.)},    {HMS(2.), HMS(2.)},       {HMS(3.), HMS(3.)},   {HMS(4.), HMS(4.)},
         {HMS(5.), HMS(5.)},    {HMS(6.), HMS(6.)},       {HMS(7.), HMS(7.)},   {HMS(8.), HMS(8.)},
         {HMS(1e-20), HMS(0.)}, {HMS(10.), HMS(10.)},     {HMS(11.), HMS(11.)}, {HMS(12.), HMS(12.)},
         {HMS(0.), HMS(1e-20)}, {HMS(1e-20), HMS(1e-20)}, {HMS(15.), HMS(15.)}, {HMS(16.), HMS(16.)}}
    };
    CHECK(hmeigens::isUpperHessenberg(
        testMatrix3,
        hmeigens::getTolerance(testMatrix3)
    ));
}

TEST_CASE("Predicates test: a non-upper Hessenberg matrix is correctly identified.", "[predicates][isUpperHessenberg]") {
    // GIVEN a matrix not in upper Hessenberg form
    // WHEN  hmeigens::isUpperHessenberg checks it
    // THEN  it returns false
    //
    // Clearly not Hessenberg (with the appropriate tolerance).
    const hmeigens::SquareMatrix testMatrix1{
        {{HMS(1.), HMS(1.)}, {HMS(2.), HMS(2.)}, {HMS(3.), HMS(3.)},
         {HMS(4.), HMS(4.)}, {HMS(5.), HMS(5.)}, {HMS(6.), HMS(6.)},
         {HMS(7.), HMS(7.)}, {HMS(8.), HMS(8.)}, {HMS(9.), HMS(9.)}}
    };
    CHECK_FALSE(hmeigens::isUpperHessenberg(
        testMatrix1,
        hmeigens::getTolerance(testMatrix1)
    ));
    // An element that is just above the tolerance.
    CHECK_FALSE(hmeigens::isUpperHessenberg(
        testMatrix1,
        // Tolerance is the number that is 1 ULP smaller than the element in the required position.
        std::nextafter(std::abs(testMatrix1(2,0)), HMS(0.))
    ));
    // An element which is small relative to the others, but still above the tolerance.
    const hmeigens::SquareMatrix testMatrix2{
        {{HMS(1.), HMS(1.)},       {HMS(2.), HMS(2.)}, {HMS(3.), HMS(3.)},
         {HMS(4.), HMS(4.)},       {HMS(5.), HMS(5.)}, {HMS(6.), HMS(6.)},
         {HMS(7.e-4), HMS(7.e-4)}, {HMS(8.), HMS(8.)}, {HMS(9.), HMS(9.)}}
    };
    CHECK_FALSE(hmeigens::isUpperHessenberg(
        testMatrix2,
        hmeigens::getTolerance(testMatrix2)
    ));
    // A 5x5 matrix with "bad" elements in different places each time.
    // This verifies that the position of the "bad" element does not matter.
    hmeigens::SquareMatrix testMatrix3 = hmeigens::SquareMatrix::identity(5);
    // Position (2,0).
    testMatrix3(2,0) = testMatrix3(0,0); // Value of (1.; 0.).
    // Tolerance does not change as the "bad" value always has the same modulus.
    const hmeigens::Scalar tolerance3 = hmeigens::getTolerance(testMatrix3);
    CHECK_FALSE(hmeigens::isUpperHessenberg(testMatrix3, tolerance3));
    // Position (4,0).
    testMatrix3(4,0) = testMatrix3(0,0); // Value of (1.; 0.).
    testMatrix3(2,0) = testMatrix3(1,0); // Value of (0.; 0.).
    CHECK_FALSE(hmeigens::isUpperHessenberg(testMatrix3, tolerance3));
    // Position (3,1).
    testMatrix3(3,1) = testMatrix3(0,0); // Value of (1.; 0.).
    testMatrix3(4,0) = testMatrix3(1,0); // Value of (0.; 0.).
    CHECK_FALSE(hmeigens::isUpperHessenberg(testMatrix3, tolerance3));
}

/* ------------------------------------------------ */
/* ---------- hmeigens::isUpperTriangular --------- */
/* ------------------------------------------------ */
//
TEST_CASE("Predicates test: an upper triangular matrix is correctly identified.", "[predicates][isUpperTriangular]") {
    // GIVEN a matrix in upper triangular form
    // WHEN  hmeigens::isUpperTriangular checks it
    // THEN  it returns true
    //
    // Trivially triangular matrices.
    // Null matrix.
    CHECK(
        hmeigens::isUpperTriangular(hmeigens::SquareMatrix{
            {{HMS(0.), HMS(0.)}, {HMS(0.), HMS(0.)}, {HMS(0.), HMS(0.)},
             {HMS(0.), HMS(0.)}, {HMS(0.), HMS(0.)}, {HMS(0.), HMS(0.)},
             {HMS(0.), HMS(0.)}, {HMS(0.), HMS(0.)}, {HMS(0.), HMS(0.)}}
        },
        // Tolerance is 0., as this needs to be exact no matter the size.
        HMS(0.)
    ));
    // For 1x1 matrices the implementation does not perform any checks, so even with large elements and a 0. tolerance the check must pass.
    CHECK(
        hmeigens::isUpperTriangular(hmeigens::SquareMatrix{
            {{HMS(1.2345), HMS(67.89)}}
        },
        HMS(0.)
    ));
    // Non-trivially triangular matrices.
    // An exact 0. in the required spot.
    const hmeigens::SquareMatrix testMatrix1{
        {{HMS(1.), HMS(1.)}, {HMS(2.), HMS(2.)},
         {HMS(0.), HMS(0.)}, {HMS(4.), HMS(4.)}}
    };
    CHECK(hmeigens::isUpperTriangular(
        testMatrix1,
        hmeigens::getTolerance(testMatrix1)
    ));
    // Exactly the tolerance in the required spot.
    const hmeigens::SquareMatrix testMatrix2{
        {{HMS(1.), HMS(1.)}, {HMS(2.), HMS(2.)},
         {HMS(3.), HMS(3.)}, {HMS(4.), HMS(4.)}}
    };
    // This check verifies that a number exactly equal to the tolerance counts as 0., no matter how large its modulus is.
    // In this specific case it is larger than that of most other elements of the matrix itself.
    CHECK(hmeigens::isUpperTriangular(
        testMatrix2,
        std::abs(testMatrix2(1,0))
    ));
    // A larger matrix with "small enough to be zero" values in the required spots.
    const hmeigens::SquareMatrix testMatrix3{
        {{HMS(1.), HMS(1.)},    {HMS(2.), HMS(2.)},       {HMS(3.), HMS(3.)},
         {HMS(1e-20), HMS(0.)}, {HMS(5.), HMS(5.)},       {HMS(6.), HMS(6.)},
         {HMS(0.), HMS(1e-20)}, {HMS(1e-20), HMS(1e-20)}, {HMS(9.), HMS(9.)}}
    };
    CHECK(hmeigens::isUpperTriangular(
        testMatrix3,
        hmeigens::getTolerance(testMatrix3)
    ));
}

TEST_CASE("Predicates test: a non-upper triangular matrix is correctly identified.", "[predicates][isUpperTriangular]") {
    // GIVEN a matrix not in upper triangular form
    // WHEN  hmeigens::isUpperTriangular checks it
    // THEN  it returns false
    //
    // Clearly not triangular (with the appropriate tolerance).
    const hmeigens::SquareMatrix testMatrix1{
        {{HMS(1.), HMS(1.)}, {HMS(2.), HMS(2.)},
         {HMS(3.), HMS(3.)}, {HMS(4.), HMS(4.)}}
    };
    CHECK_FALSE(hmeigens::isUpperTriangular(
        testMatrix1,
        hmeigens::getTolerance(testMatrix1)
    ));
    // An element that is just above the tolerance.
    CHECK_FALSE(hmeigens::isUpperTriangular(
        testMatrix1,
        // Tolerance is the number that is 1 ULP smaller than the element in the required position.
        std::nextafter(std::abs(testMatrix1(1,0)), HMS(0.))
    ));
    // An element which is small relative to the others, but still above the tolerance.
    const hmeigens::SquareMatrix testMatrix2{
        {{HMS(1.), HMS(1.)},       {HMS(2.), HMS(2.)},
         {HMS(3.e-4), HMS(3.e-4)}, {HMS(4.), HMS(4.)}}
    };
    CHECK_FALSE(hmeigens::isUpperTriangular(
        testMatrix2,
        hmeigens::getTolerance(testMatrix2)
    ));
    // A 4x4 matrix with "bad" elements in different places each time.
    // This verifies that the position of the "bad" element does not matter.
    hmeigens::SquareMatrix testMatrix3 = hmeigens::SquareMatrix::identity(4);
    // Position (1,0).
    testMatrix3(1,0) = testMatrix3(0,0); // Value of (1.; 0.).
    // Tolerance does not change as the "bad" value always has the same modulus.
    const hmeigens::Scalar tolerance3 = hmeigens::getTolerance(testMatrix3);
    CHECK_FALSE(hmeigens::isUpperTriangular(testMatrix3, tolerance3));
    // Position (3,0).
    testMatrix3(3,0) = testMatrix3(0,0); // Value of (1.; 0.).
    testMatrix3(1,0) = testMatrix3(0,1); // Value of (0.; 0.).
    CHECK_FALSE(hmeigens::isUpperTriangular(testMatrix3, tolerance3));
    // Position (3,2).
    testMatrix3(3,2) = testMatrix3(0,0); // Value of (1.; 0.).
    testMatrix3(3,0) = testMatrix3(0,1); // Value of (0.; 0.).
    CHECK_FALSE(hmeigens::isUpperTriangular(testMatrix3, tolerance3));
}

#undef HMS
