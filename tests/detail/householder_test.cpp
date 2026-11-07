// Test file for householder.hpp/.cpp.
// Sections:
// - constants;
// - helpers;
// - test cases:
//      - hmeigens::detail::makeHouseholderInPlace.
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

// Local brief alias for the conversion macro to improve legibility.
#define HMS HMEIGENS_MAKE_SCALAR

// Readability.
using hmeigens::test::checkRealULPs;
using hmeigens::test::checkComplexULPs;
using hmeigens::test::checkComplexAbs;

/* ----------------------------- */
/* --------- IMPORTANT --------- */
/* ----------------------------- */
//
// The names of the values cited in the comments (u, v, H, phi, etc...) are those of the variables used in the documentation of hmeigens::makeUpperHessenbergInPlace in eigenvalues_solver.hpp.

/* --------------------------------------------- */
/* ----------------- Constants ----------------- */
/* --------------------------------------------- */
//
// Base tolerance for the tests of hmeigens::detail::makeHouseholderInPlace.
// Reference: Matrix Computations (Golub & Van Loan, 4th edition, 2013), section 5.1.5 Roundoff Properties.
// The error scales linearly with the 2-norm of the vector.
// The form for the error given is O(u*norm), where u is half of hmeigens::epsilon.
// The constant is never explicitly given.
// The value chosen here is somewhat arbitrary and generous. What the tests here check is not a tight behaviour, so this is fine.
// IMPORTANT: When the tests are not performed on a normalized vector, this value needs to be multiplied by the vector's 2-norm.
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
    // Readability.
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

/* ------------------------------------------------ */
/* ------------------ Test cases ------------------ */
/* ------------------------------------------------ */
//
/* ------------------------------------------------ */
/* --- hmeigens::detail::makeHouseholderInPlace --- */
/* ------------------------------------------------ */
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

#undef HMS
