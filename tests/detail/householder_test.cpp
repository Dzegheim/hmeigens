// Test file for householder.hpp/.cpp.
// Sections:
// - constants;
// - helpers;
// - test cases:
//      - hmeigens::detail::makeHouseholderInPlace;
//      - hmeigens::detail::applyLeftHouseholderInPlace and hmeigens::detail::applyRightHouseholderInPlace.
#include "hmeigens/detail/householder.hpp"
#include "hmeigens/detail/algebra_helpers.hpp"
#include "hmeigens/scalar.hpp"
#include "hmeigens/constants.hpp"
#include "hmeigens/tolerances.hpp"
#include "hmeigens/square_matrix.hpp"

#include "../comparison_helpers.hpp"

#include <catch2/catch_test_macros.hpp>

#include <cmath>     // For std::hypot
#include <complex>   // For std::conj
#include <cstddef>   // For std::size_t
#include <limits>    // For std::numeric_limits

// Readability of tests.
using hmeigens::test::checkRealULPs;
using hmeigens::test::checkComplexULPs;
using hmeigens::test::checkComplexAbs;
#define HMS HMEIGENS_MAKE_SCALAR

/* ----------------------------- */
/* --------- IMPORTANT --------- */
/* ----------------------------- */
//
// The names of the values cited in the comments (u, v, H, phi, etc...) are those of the variables used in the documentation of hmeigens::makeUpperHessenbergInPlace in eigenvalues_solver.hpp.

/* --------------------------------------------- */
/* ----------------- Constants ----------------- */
/* --------------------------------------------- */
//
// Base tolerance for the tests.
// Reference: Matrix Computations (Golub & Van Loan, 4th edition, 2013), section 5.1.5 Roundoff Properties.
// The error scales linearly with the 2-norm of the vector.
// The form for the error given is O(u*norm), where u is half of hmeigens::epsilon.
// The constant is never explicitly given.
// The value chosen here is somewhat arbitrary and generous. What the tests here check is not a tight behaviour, so this is fine.
// IMPORTANT: When the tests are not performed on a normalized vector, this value needs to be multiplied by the norm.
constexpr hmeigens::Scalar testTolerance = HMS(100.)*hmeigens::epsilon;

/* --------------------------------------------- */
/* ------------------ Helpers ------------------ */
/* --------------------------------------------- */
//

// This helper directly applies the reflector built from u to v, to check that it is correctly "squished" into the first component.
// Since its behaviour is not trivial, it is tested here in the same file.
static hmeigens::SquareMatrix::Container applyReflectorForTesting(const hmeigens::SquareMatrix::Container& v, const hmeigens::SquareMatrix::Container& u) {
    // Get a vector to apply the transformation to.
    auto applied = v;
    // Compute the scalar product of u^dagger*v.
    // Cannot overflow by construction as u is a unit vector.
    hmeigens::Complex sum{};
    for (std::size_t i = 0; i < v.size(); ++i) {
        sum += std::conj(u[i])*v[i];
    }
    // Apply the reflector in the most ignorant way possible.
    // If ||v|| is close to max, this may overflow.
    // The caller normalizes v to prevent this.
    for (std::size_t i = 0; i < v.size(); ++i) {
        applied[i] -= HMS(2.)*u[i]*sum;
    }
    return applied;
}

// This helper takes a vector and verifies three behaviours of the function hmeigens::detail::makeHouseholderInPlace:
// (1) that the input it takes is transformed into a unit vector;
// (2) that the value it returns is the expected phi*||v||;
// (3) that the transformed unit vector is the correct Householder vector.
// The last point is verified indirectly, by calling the helper applyReflectorForTesting on a normalized version of v, and then by checking that its output is a vector whose first element is phi, while the other elements are all (0.; 0.) within tolerance.
// If the first and third point are both verified, then hmeigens::detail::makeHouseholderInPlace produces a correct unit Householder vector.
// Note: multiplying it by any z such that |z|=1. does not alter the result.
static void checkVector(const hmeigens::SquareMatrix::Container& vector) {
    // Build a copy of the input to work on.
    auto copyVector = vector;
    const auto& v0 = vector[0];
    const auto normV = hmeigens::detail::twoNorm(vector);
    // Call the function.
    const auto returnedValue = hmeigens::detail::makeHouseholderInPlace(copyVector);
    // Capture the values in case the test fails.
    CAPTURE(vector, copyVector, returnedValue);
    // (1) Check that the transformed vector is a unit vector.
    // The tolerance here is that of hmeigens::detail::twoNorm.
    checkRealULPs(
        hmeigens::detail::twoNorm(copyVector),
        HMS(1.),
        hmeigens::normTolerance(copyVector.size())
    );
    // (2) Check that the returned value is actually phi*||v||.
    // Here it is ok to check for (0.; 0.) with == because the only case in which phi is not computed is for exactly that value.
    // If phi is not computed, its chosen value is (-1.; 0.).
    const auto expectedValue =
        v0 == hmeigens::Complex{} ?
        hmeigens::Complex{-normV, HMS(0.)} :
        -v0/std::hypot(v0.real(), v0.imag())*normV;
    // Tolerance must be scaled by the norm here.
    checkComplexAbs(
        returnedValue,
        expectedValue,
        testTolerance*normV
    );
    // (3) Check that copyVector behaves as expected, i.e. that it is the Householder vector.
    // This means that:
    // H*v = v-2*u*u^dagger/(||u||^2)*v
    // is (phi*||v||, 0, ..., 0).
    // The reflection is a linear relationship, therefore this check can be done on a normalized vector without loss of generality. The matrices in this project are also scaled by a factor before working on them, so in practice elements will never be too large anyway.
    // The helper applyReflectorForTesting may otherwise cause an overflow for values too close to the maximum representable values allowed by the types, even if they are supposed to be perfectly valid inputs.
    // In terms of variables of this function, calling sum the scalar product of conj(copyVector) and normVector, then
    // normVector-2*copyVector*sum
    // must be (expectedValue/normV, 0., ..., 0.) within testTolerance.
    auto normVector = vector;
    for (auto& element : normVector) {
        element /= normV;
    }
    const auto applied = applyReflectorForTesting(normVector, copyVector);
    // Check that the first element is phi.
    checkComplexAbs(
        applied[0],
        // Testing on a normalized vector means this value needs to be scaled by the norm.
        expectedValue/normV,
        testTolerance
    );
    // All the other elements must be (0.; 0.).
    // If they are, within the tolerance, this justifies the manual rewrite to exactly (0.; 0.) done in the solver, as the residuals after the transformation are within the rounding error.
    for (std::size_t i = 1; i < applied.size(); ++i) {
        CAPTURE(i);
        checkComplexAbs(
            applied[i],
            hmeigens::Complex{HMS(0.), HMS(0.)},
            testTolerance
        );
    }
    return;
}

// This helper builds a full Householder matrix to test both the left and right application functions.
// The matrix is built with the mathematical definition:
// H = I-2*u*u^dagger.
// Note that here vector (i.e. u) is assumed to be unit-length.
// Here col is the column u was computed from.
static hmeigens::SquareMatrix buildHouseholderForTesting(const hmeigens::SquareMatrix::Container& vector, const std::size_t col) {
    hmeigens::SquareMatrix householder = hmeigens::SquareMatrix::identity(col+1+vector.size());
    // A part of the Householder matrix will remain the identity.
    // The only parts that need to be modified are those in the subblock past col.
    for (std::size_t i = 0; i < vector.size(); ++i) {
        for (std::size_t j = 0; j < vector.size(); ++j) {
            householder(col+1+i, col+1+j) -= HMS(2.)*vector[i]*std::conj(vector[j]);
        }
    }
    return householder;
}

// This enum is used to determine from which side the Householder matrix must be applied to the input matrix by checkHouseholder.
enum class Side {
    left,
    right
};

// This helper compares the explicit application of a Householder matrix to the input matrix against the fast application performed by hmeigens::detail::applyLeftHouseholderInPlace or hmeigens::detail::applyRightHouseholderInPlace, chosen with side.
// Both the Householder matrix and the Householder vector are built from column col.
// The tolerance for this check is testTolerance multiplied by the Frobenius norm of matrix.
// Note: it must hold that col+1 < matrix.size().
static void checkHouseholder(const hmeigens::SquareMatrix& matrix, const std::size_t col, const Side side) {
    const std::size_t amount = matrix.size()-col-1;
    hmeigens::SquareMatrix::Container vector(amount);
    for (std::size_t i = 0; i < amount; ++i) {
        vector[i] = matrix(col+1+i, col);
    }
    // The function is [[nodiscard]]. This prevents the compiler warning, as the returned value is not needed here.
    static_cast<void>(hmeigens::detail::makeHouseholderInPlace(vector));
    const hmeigens::SquareMatrix householder = buildHouseholderForTesting(vector, col);
    const auto expected =
        side == Side::left ?
        householder*matrix :
        matrix*householder;
    auto toCheck = matrix;
    if (side == Side::left) {
        hmeigens::detail::applyLeftHouseholderInPlace(toCheck, vector, col);
    }
    else {
        hmeigens::detail::applyRightHouseholderInPlace(toCheck, vector, col);
    }
    const auto tolerance = testTolerance*matrix.norm();
    for (std::size_t i = 0; i < matrix.size(); ++i) {
        for (std::size_t j = 0; j < matrix.size(); ++j) {
            CAPTURE(i, j);
            // Up to col: toCheck must be completely unchanged.
            if (j <= col) {
                checkComplexULPs(
                    toCheck(i, j),
                    matrix(i, j),
                    0
                );
            }
            // After col: verify against expected within tolerance.
            else {
                checkComplexAbs(
                    toCheck(i, j),
                    expected(i, j),
                    tolerance
                );
            }
        }
    }
    return;
}

/* -------------------------------------------------- */
/* ------------------- Test cases ------------------- */
/* -------------------------------------------------- */
//
/* -------------------------------------------------- */
/* ---- hmeigens::detail::makeHouseholderInPlace ---- */
/* -------------------------------------------------- */
//
TEST_CASE("Householder vector computation test: the testing helper behaves as expected.", "[householder][detail][makeHouseholderInPlace][helper]") {
    // GIVEN a vector v and a unit vector u
    // WHEN  the helper applyReflectorForTesting applies the reflector built from u to v
    // THEN  the output is reflected along the expected direction
    //
    // The tolerance here is 0 ULPs, as all operations below are correctly rounded, performed on exactly representable values, and have exactly representable results.
    const hmeigens::SquareMatrix::Container v{
        {HMS(3.), HMS(0.)},
        {HMS(4.), HMS(0.)}
    };
    // Notation: [x; y] is a vector holding 2 complex elements, to distinguish it from (x; y), which is a complex number with x, y being the real and imaginary part respectively.
    // First reflection: along the first axis.
    // u*(u^dagger*v) = [1; 0]*([1; 0]*[3; 4]) = [3; 0],
    // applied = [3; 4] - 2*[3; 0] = [-3; 4].
    hmeigens::SquareMatrix::Container u{
        {HMS(1.), HMS(0.)},
        {HMS(0.), HMS(0.)}
    };
    hmeigens::SquareMatrix::Container expected{
        {HMS(-3.), HMS(0.)},
        {HMS(4.), HMS(0.)}
    };
    auto applied = applyReflectorForTesting(v, u);
    for (std::size_t i = 0; i < v.size(); ++i) {
        CAPTURE(i);
        checkComplexULPs(
            applied[i],
            expected[i],
            0
        );
    }
    // Second reflection: along the second axis.
    // u*(u^dagger*v) = [0; 1]*([0; 1]*[3; 4]) = [0; 4],
    // applied = [3; 4] - 2*[0; 4] = [3; -4].
    u = hmeigens::SquareMatrix::Container{
        {HMS(0.), HMS(0.)},
        {HMS(1.), HMS(0.)}
    };
    expected = hmeigens::SquareMatrix::Container{
        {HMS(3.), HMS(0.)},
        {HMS(-4.), HMS(0.)}
    };
    applied = applyReflectorForTesting(v, u);
    for (std::size_t i = 0; i < v.size(); ++i) {
        CAPTURE(i);
        checkComplexULPs(
            applied[i],
            expected[i],
            0
        );
    }
    // Third reflection: along the first axis, imaginary entry.
    // Must be the same as the first case, but the conjugation does something.
    // u*(u^dagger*v) = [i; 0]*([-i; 0]*[3; 4]) = i*[-3i; 0] = [3; 0],
    // applied = [3; 4] - 2*[3; 0] = [-3; 4].
    u = hmeigens::SquareMatrix::Container{
        {HMS(0.), HMS(1.)},
        {HMS(0.), HMS(0.)}
    };
    expected = hmeigens::SquareMatrix::Container{
        {HMS(-3.), HMS(0.)},
        {HMS(4.), HMS(0.)}
    };
    applied = applyReflectorForTesting(v, u);
    for (std::size_t i = 0; i < v.size(); ++i) {
        CAPTURE(i);
        checkComplexULPs(
            applied[i],
            expected[i],
            0
        );
    }
    // Fourth reflection: mixed components.
    // u*(u^dagger*v) = [.5+.5i; .5-.5i]*([.5-.5i; .5+.5i]*[3; 4]) =
    // [.5+.5i; .5-.5i]*(1.5-1.5i+2+2i) =
    // [.5+.5i; .5-.5i]*(3.5+.5i) =
    // [(.5+.5i)*(3.5+.5i); (.5-.5i)*(3.5+.5i)] =
    // [1.75+.25i+1.75i-.25; 1.75-1.75i+.25i+.25] =
    // [1.5+2i; 2-1.5i],
    // applied = [3; 4] - 2*[1.5+2i; 2-1.5i] =
    // [3-3-4i; 4-4+3i] = [-4i; 3i].
    // ||u|| = sqrt(|.5+.5i|^2+|.5-.5i|^2) =
    // sqrt(.5^2+.5^2+.5^2+.5^2) =
    // sqrt(4*.25) =
    // sqrt(1) = 1.
    u = hmeigens::SquareMatrix::Container{
        {HMS(.5), HMS(.5)},
        {HMS(.5), HMS(-.5)}
    };
    expected = hmeigens::SquareMatrix::Container{
        {HMS(0.), HMS(-4.)},
        {HMS(0.), HMS(3.)}
    };
    applied = applyReflectorForTesting(v, u);
    for (std::size_t i = 0; i < v.size(); ++i) {
        CAPTURE(i);
        checkComplexULPs(
            applied[i],
            expected[i],
            0
        );
    }
    // And to you, dear reader who may be checking this test case and wonder "why did you copy-paste the for loops when they could have been a helper?", I will reply: I'm not writing a helper to test a helper that runs inside another helper that tests a function in fucking hmeigens::detail.
}

TEST_CASE("Householder vector computation test: the Householder vector is correct.", "[householder][detail][makeHouseholderInPlace]") {
    // GIVEN a vector v
    // WHEN  the corresponding Householder vector u is computed by hmeigens::detail::makeHouseholderInPlace
    // THEN  ||u|| is 1., the returned value is the expected element for the subdiagonal entry, and applying the reflector built from u to v transforms v as expected
    //
    // This test covers all three behaviours expected of the function in one case, by using the helper checkVector, as all three of those behaviours come from the same exact fact: u is what it is.
    // Those three facts are inseparable from each other, and are simply different facets of the same computation.
    //
    // Tolerance: see testTolerance.
    //
    // Simple cases. Pythagorean triple.
    // Real vector. Same sign.
    hmeigens::SquareMatrix::Container v{
        {HMS(3.), HMS(0.)},
        {HMS(4.), HMS(0.)}
    };
    checkVector(v);
    // Real vector. Opposite signs.
    v = hmeigens::SquareMatrix::Container{
        {HMS(3.), HMS(0.)},
        {HMS(-4.), HMS(0.)}
    };
    checkVector(v);
    // Imaginary vector. Same sign.
    v = hmeigens::SquareMatrix::Container{
        {HMS(0.), HMS(3.)},
        {HMS(0.), HMS(4.)}
    };
    checkVector(v);
    // Imaginary vector. Opposite signs.
    v = hmeigens::SquareMatrix::Container{
        {HMS(0.), HMS(-3.)},
        {HMS(0.), HMS(4.)}
    };
    checkVector(v);
    // Already correct vector.
    v = hmeigens::SquareMatrix::Container{
        {HMS(1.), HMS(2.)},
        {HMS(0.), HMS(0.)},
        {HMS(0.), HMS(0.)},
        {HMS(0.), HMS(0.)},
        {HMS(0.), HMS(0.)},
        {HMS(0.), HMS(0.)},
        {HMS(0.), HMS(0.)}
    };
    checkVector(v);
    // Null first component.
    v = hmeigens::SquareMatrix::Container{
        {HMS(0.), HMS(0.)},
        {HMS(0.), HMS(5.)},
        {HMS(5.), HMS(0.)}
    };
    checkVector(v);
    // One non-null component somewhere in the middle.
    v = hmeigens::SquareMatrix::Container{
        {HMS(0.), HMS(0.)},
        {HMS(0.), HMS(0.)},
        {HMS(0.), HMS(0.)},
        {HMS(0.), HMS(0.)},
        {HMS(6.), HMS(7.)},
        {HMS(0.), HMS(0.)},
        {HMS(0.), HMS(0.)},
        {HMS(0.), HMS(0.)}
    };
    checkVector(v);
    // Extreme values.
    // This check is to ensure that very large values are not overflowing the sum vector[0] += minusPhi.
    const auto almostMax = std::numeric_limits<hmeigens::Scalar>::max()*HMS(.65);
    v = hmeigens::SquareMatrix::Container{
        {almostMax, HMS(0.)},
        {HMS(0.), almostMax}
    };
    checkVector(v);
    // Small values as well.
    const auto min = std::numeric_limits<hmeigens::Scalar>::min();
    v = hmeigens::SquareMatrix::Container{
        {min, HMS(0.)},
        {HMS(0.), min}
    };
    checkVector(v);
    // Large disparity in values.
    // This check is extremely important, as phi is chosen to be opposite to v_0.
    // Were it not the case, even if the result is equivalent algebraically, in such cases the result would lose too much precision.
    // See section 5.1.3 Computing the Householder Vector of Matrix Computations (Golub & Van Loan, 4th edition, 2013), about the severe cancellation issue.
    v = hmeigens::SquareMatrix::Container{
        {HMS(0.), HMS(1.e20)},
        {HMS(0.), HMS(1.e-20)}
    };
    checkVector(v);
    // Arbitrary vector.
    v = hmeigens::SquareMatrix::Container{
        {HMS(1.), HMS(-1.)},
        {HMS(10.), HMS(.234)},
        {HMS(.7), HMS(9.6333)},
        {HMS(6.), HMS(0.)},
        {HMS(0.), HMS(7.)},
        {HMS(6.), HMS(7.)},
        {HMS(7.), HMS(6.)},
        {HMS(-33.), HMS(120.)},
        {HMS(-1.), HMS(12.)},
        {HMS(14.5), HMS(-20.)},
        {HMS(22.), HMS(-.761)},
        {HMS(0.), HMS(1.e-7)}
    };
    checkVector(v);
}

/* -------------------------------------------------- */
/* -- hmeigens::detail::applyLeftHouseholderInPlace - */
/* - hmeigens::detail::applyRightHouseholderInPlace - */
/* -------------------------------------------------- */
//
// Since these two functions are tested by the same helper with the exception of an enum parameter, they are grouped together.
//
TEST_CASE("Householder application test: the testing helper behaves as expected.", "[householder][detail][applyLeftHouseholderInPlace][applyRightHouseholderInPlace][helper]") {
    // GIVEN a unit-length Householder vector and a column number
    // WHEN  the corresponding Householder matrix is built
    // THEN  it is the expected matrix
    //
    // The tolerance here is 0 ULPs, as all operations below are correctly rounded, performed on exactly representable values, and have exactly representable results.
    //
    // Assuming u is the subdiagonal second column of a 4x4 matrix, the relevant components of the Householder matrix are (counting from 0):
    // H(2, 2) = 1-2*(.5+.5i)*(.5-.5i)
    //         = 1-2*(.25+.25)
    //         = 1-1
    //         = 0;
    // H(2, 3) = -2*(.5+.5i)*(.5+.5i)
    //         = -2*(.25-.25+.5i)
    //         = -2*.5i
    //         = -i;
    // H(3, 2) = -2*(.5-.5i)*(.5-.5i)
    //         = -2*(.25-.25-.5i)
    //         = -2*-.5i
    //         = i;
    // H(3, 3) = 1-2*(.5-.5i)*(.5+.5i)
    //         = H(2, 2).
    // The rest is simply the identity matrix.
    const hmeigens::SquareMatrix::Container u{
        {HMS(.5), HMS(.5)},
        {HMS(.5), HMS(-.5)}
    };
    const hmeigens::SquareMatrix expected{
        {{HMS(1.), HMS(0.)}, {HMS(0.), HMS(0.)}, {HMS(0.), HMS(0.)}, {HMS(0.), HMS(0.)},
         {HMS(0.), HMS(0.)}, {HMS(1.), HMS(0.)}, {HMS(0.), HMS(0.)}, {HMS(0.), HMS(0.)},
         {HMS(0.), HMS(0.)}, {HMS(0.), HMS(0.)}, {HMS(0.), HMS(0.)}, {HMS(0.), HMS(-1.)},
         {HMS(0.), HMS(0.)}, {HMS(0.), HMS(0.)}, {HMS(0.), HMS(1.)}, {HMS(0.), HMS(0.)}}
    };
    // Here col must be 1 since u has 2 elements, otherwise the built reflector would not be a 4x4.
    const auto toCheck = buildHouseholderForTesting(u, 1);
    REQUIRE(toCheck.size() == expected.size());
    for (std::size_t i = 0; i < toCheck.size(); ++i) {
        for (std::size_t j = 0; j < toCheck.size(); ++j) {
            CAPTURE(i, j);
            checkComplexULPs(
                toCheck(i, j),
                expected(i, j),
                0
            );
        }
    }
}

TEST_CASE("Left Householder application test: the application is correct.", "[householder][detail][applyLeftHouseholderInPlace]") {
    // GIVEN a matrix and an appropriate unit-length Householder vector built from its column col
    // WHEN  hmeigens::detail::applyLeftHouseholderInPlace applies the reflector to matrix
    // THEN  the matrix is unchanged up to column col, while after it, it is the same as the direct left application of the Householder matrix
    //
    // The minimum meaningful size is 3, as this function is used in the process of turning a matrix into upper Hessenberg form, and 2x2/1x1 matrices are already such by definition.
    const hmeigens::SquareMatrix testMatrix{
        {{HMS(1.), HMS(-1.)}, {HMS(2.), HMS(-.02)}, {HMS(3.), HMS(-.3)},
         {HMS(4.), HMS(-.4)}, {HMS(5.), HMS(-5.)}, {HMS(6.), HMS(-.06)},
         {HMS(7.), HMS(-.07)}, {HMS(8.), HMS(-.8)}, {HMS(9.), HMS(-9.)}}
    };
    checkHouseholder(testMatrix, 0, Side::left);
    // Larger size, various values of col.
    const hmeigens::SquareMatrix testMatrixLarge{
        {{HMS(1.), HMS(-1.)}, {HMS(2.), HMS(-.02)}, {HMS(3.), HMS(-.3)}, {HMS(-4.), HMS(.4)}, {HMS(-5.), HMS(5.)}, {HMS(-6.), HMS(.06)},
         {HMS(4.), HMS(-.4)}, {HMS(5.), HMS(-5.)}, {HMS(16.), HMS(-1.06)}, {HMS(14.), HMS(-1.4)}, {HMS(15.), HMS(-15.)}, {HMS(16.), HMS(-1.06)},
         {HMS(7.), HMS(-.07)}, {HMS(8.), HMS(-.8)}, {HMS(9.), HMS(-9.)}, {HMS(-.07), HMS(7.)}, {HMS(-.8), HMS(8.)}, {HMS(-9.), HMS(9.)},
         {HMS(9.), HMS(-.8)}, {HMS(7.), HMS(-6.)}, {HMS(25.), HMS(-4.06)}, {HMS(13.), HMS(-2.2)}, {HMS(11.), HMS(-11.)}, {HMS(10.), HMS(-0.06)},
         {HMS(44.), HMS(-.44)}, {HMS(45.), HMS(-45.)}, {HMS(46.), HMS(-1.46)}, {HMS(14.4), HMS(-1.44)}, {HMS(15.4), HMS(-15.4)}, {HMS(16.4), HMS(-1.064)},
         {HMS(24.), HMS(-3.4)}, {HMS(15.), HMS(-45.)}, {HMS(66.), HMS(-7.06)}, {HMS(14.8), HMS(-1.49)}, {HMS(15.1), HMS(-15.2)}, {HMS(16.3), HMS(-1.006)}}
    };
    // Values up to size-3 are ok to pass, size-2 is meaningless, size-1 is invalid.
    checkHouseholder(testMatrixLarge, 0, Side::left);
    checkHouseholder(testMatrixLarge, 1, Side::left);
    checkHouseholder(testMatrixLarge, 2, Side::left);
    checkHouseholder(testMatrixLarge, 3, Side::left);
}

TEST_CASE("Right Householder application test: the application is correct.", "[householder][detail][applyRightHouseholderInPlace]") {
    // GIVEN a matrix and an appropriate unit-length Householder vector built from its column col
    // WHEN  hmeigens::detail::applyRightHouseholderInPlace applies the reflector to matrix
    // THEN  the matrix is unchanged up to column col, while after it, it is the same as the direct right application of the Householder matrix
    //
    // The minimum meaningful size is 3, see the previous case.
    const hmeigens::SquareMatrix testMatrix{
        {{HMS(1.), HMS(-1.)}, {HMS(2.), HMS(-.02)}, {HMS(3.), HMS(-.3)},
         {HMS(4.), HMS(-.4)}, {HMS(5.), HMS(-5.)}, {HMS(6.), HMS(-.06)},
         {HMS(7.), HMS(-.07)}, {HMS(8.), HMS(-.8)}, {HMS(9.), HMS(-9.)}}
    };
    checkHouseholder(testMatrix, 0, Side::right);
    // Larger size, various values of col.
    const hmeigens::SquareMatrix testMatrixLarge{
        {{HMS(1.), HMS(-1.)}, {HMS(2.), HMS(-.02)}, {HMS(3.), HMS(-.3)}, {HMS(-4.), HMS(.4)}, {HMS(-5.), HMS(5.)}, {HMS(-6.), HMS(.06)},
         {HMS(4.), HMS(-.4)}, {HMS(5.), HMS(-5.)}, {HMS(16.), HMS(-1.06)}, {HMS(14.), HMS(-1.4)}, {HMS(15.), HMS(-15.)}, {HMS(16.), HMS(-1.06)},
         {HMS(7.), HMS(-.07)}, {HMS(8.), HMS(-.8)}, {HMS(9.), HMS(-9.)}, {HMS(-.07), HMS(7.)}, {HMS(-.8), HMS(8.)}, {HMS(-9.), HMS(9.)},
         {HMS(9.), HMS(-.8)}, {HMS(7.), HMS(-6.)}, {HMS(25.), HMS(-4.06)}, {HMS(13.), HMS(-2.2)}, {HMS(11.), HMS(-11.)}, {HMS(10.), HMS(-0.06)},
         {HMS(44.), HMS(-.44)}, {HMS(45.), HMS(-45.)}, {HMS(46.), HMS(-1.46)}, {HMS(14.4), HMS(-1.44)}, {HMS(15.4), HMS(-15.4)}, {HMS(16.4), HMS(-1.064)},
         {HMS(24.), HMS(-3.4)}, {HMS(15.), HMS(-45.)}, {HMS(66.), HMS(-7.06)}, {HMS(14.8), HMS(-1.49)}, {HMS(15.1), HMS(-15.2)}, {HMS(16.3), HMS(-1.006)}}
    };
    // See previous case.
    checkHouseholder(testMatrixLarge, 0, Side::right);
    checkHouseholder(testMatrixLarge, 1, Side::right);
    checkHouseholder(testMatrixLarge, 2, Side::right);
    checkHouseholder(testMatrixLarge, 3, Side::right);
}

#undef HMS
