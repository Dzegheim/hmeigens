// Test file for square_matrix.hpp/.cpp.
// Sections:
// - helpers;
// - test cases:
//      - hmeigens::CanBeSize;
//      - constructors hmeigens::SquareMatrix(Size) and hmeigens::SquareMatrix(hmeigens::SquareMatrix::Container&&);
//      - accessors hmeigens::SquareMatrix::operator() and hmeigens::SquareMatrix::at;
//      - hmeigens::SquareMatrix::identity;
//      - hmeigens::SquareMatrix::norm.
#include "hmeigens/tolerances.hpp"
#include "hmeigens/square_matrix.hpp"

#include <stdexcept>     // For std::out_of_range, std::length_error, std::invalid_argument
#include <cstddef>       // For std::size_t
#include <cstdint>       // For std::intmax_t, std::uintmax_t, std::uint64_t
#include <format>
#include <string_view>
#include <type_traits>   // For std::is_same, std::is_assignable_v, std::is_convertible_v, std::is_constructible_v
#include <tuple>
#include <cmath>         // For std::sqrt, std::isinf
#include <limits>        // For std::numeric_limits

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_template_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>
#include <catch2/matchers/catch_matchers_exception.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

// Local brief alias for the conversion macro to improve legibility.
#define HMS HMEIGENS_MAKE_SCALAR

/* --------------------------------------------- */
/* ------------------ Helpers ------------------ */
/* --------------------------------------------- */
//
// This helper checks that two real numbers are within the expected tolerance of one another.
static void checkReal(const hmeigens::Scalar toCheck, const hmeigens::Scalar expected, const std::uint64_t toleranceULPs) {
    CAPTURE(toCheck, expected);
    CHECK_THAT(
        toCheck,
        Catch::Matchers::WithinULP(expected, toleranceULPs)
    );
    return;
}

// This helper checks that two complex numbers passed to it are "equal enough", i.e. that their real and imaginary parts are both within toleranceULPs of each other.
static void checkComplex(const hmeigens::Complex& toCheck, const hmeigens::Complex& expected, const std::uint64_t toleranceULPs) {
    CAPTURE(toCheck, expected);
    // Real part check.
    checkReal(toCheck.real(), expected.real(), toleranceULPs);
    // Imaginary part check.
    checkReal(toCheck.imag(), expected.imag(), toleranceULPs);
    return;
}

// This helper checks that all elements of a matrix match the ones of a reference hmeigens::SquareMatrix::Container within toleranceULPs.
// It accesses the elements via hmeigens::SquareMatrix::operator() const.
static void checkAllElementsVersus(const hmeigens::SquareMatrix& toCheck, const hmeigens::SquareMatrix::Container& referenceBody, std::uint64_t toleranceULPs) {
    // If the sizes don't match, this test makes no sense.
    REQUIRE(toCheck.size() * toCheck.size() == referenceBody.size());
    for(std::size_t row = 0; row < toCheck.size(); ++row) {
        for(std::size_t col = 0; col < toCheck.size(); ++col) {
            CAPTURE(row, col);
            // Every number is verified against the reference body.
            // The container's indexing is the same as the private member hmeigens::SquareMatrix::getIndex().
            // Here it is written explicitly so that it is independent from the class.
            // This also enforces the row-major order of the elements. This helper IS MEANT TO break if the class' internal index computation changes, as the class is supposed to be row-major.
            checkComplex(
                toCheck(row, col),
                referenceBody[row * toCheck.size() + col],
                toleranceULPs
            );
        }
    }
    return;
}

/* --------------------------------------------------------------- */
/* ------------------------- Test cases -------------------------- */
/* --------------------------------------------------------------- */
//
/* --------------------------------------------------------------- */
/* --------------------- hmeigens::CanBeSize --------------------- */
/* --------------------------------------------------------------- */
//
TEST_CASE("Square matrix test: only the allowed types are accepted as sizes.", "[square_matrix][CanBeSize]") {
    // GIVEN a type
    // WHEN  checked against the allowed ones
    // THEN  the allowed ones are accepted, the others are not
    STATIC_REQUIRE(hmeigens::CanBeSize<short int>);
    STATIC_REQUIRE(hmeigens::CanBeSize<int>);
    STATIC_REQUIRE(hmeigens::CanBeSize<long int>);
    STATIC_REQUIRE(hmeigens::CanBeSize<long long int>);
    STATIC_REQUIRE(hmeigens::CanBeSize<unsigned short int>);
    STATIC_REQUIRE(hmeigens::CanBeSize<unsigned int>);
    STATIC_REQUIRE(hmeigens::CanBeSize<unsigned long int>);
    STATIC_REQUIRE(hmeigens::CanBeSize<unsigned long long int>);
    // Integers that are meaningless as sizes.
    STATIC_REQUIRE_FALSE(hmeigens::CanBeSize<bool>);
    STATIC_REQUIRE_FALSE(hmeigens::CanBeSize<char>);
    STATIC_REQUIRE_FALSE(hmeigens::CanBeSize<signed char>);
    STATIC_REQUIRE_FALSE(hmeigens::CanBeSize<unsigned char>);
    STATIC_REQUIRE_FALSE(hmeigens::CanBeSize<char8_t>);
    STATIC_REQUIRE_FALSE(hmeigens::CanBeSize<char16_t>);
    STATIC_REQUIRE_FALSE(hmeigens::CanBeSize<char32_t>);
    STATIC_REQUIRE_FALSE(hmeigens::CanBeSize<wchar_t>);
    // Floats.
    STATIC_REQUIRE_FALSE(hmeigens::CanBeSize<float>);
    STATIC_REQUIRE_FALSE(hmeigens::CanBeSize<double>);
    STATIC_REQUIRE_FALSE(hmeigens::CanBeSize<long double>);
    // Constness and volatility don't matter.
    STATIC_REQUIRE(hmeigens::CanBeSize<const volatile int>);
    STATIC_REQUIRE_FALSE(hmeigens::CanBeSize<const volatile float>);
    // The aliases used in the code work.
    STATIC_REQUIRE(hmeigens::CanBeSize<std::size_t>);
    STATIC_REQUIRE(hmeigens::CanBeSize<std::intmax_t>);
    STATIC_REQUIRE(hmeigens::CanBeSize<std::uintmax_t>);
}

/* --------------------------------------------------------------- */
/* ------------------------ constructors ------------------------- */
/* -------------- hmeigens::SquareMatrix(Size) and --------------- */
/* - hmeigens::SquareMatrix(hmeigens::SquareMatrix::Container&&) - */
/* --------------------------------------------------------------- */
//
TEST_CASE("Square matrix test: the size parameter constructor accepts valid types and refuses invalid ones.", "[square_matrix][SquareMatrix][constructor][SquareMatrix(Size)]") {
    // GIVEN a type
    // WHEN  the compiler checks if it is valid
    // THEN  the allowed ones are accepted, the others are not
    STATIC_REQUIRE(std::is_constructible_v<hmeigens::SquareMatrix, int>);
    STATIC_REQUIRE_FALSE(std::is_constructible_v<hmeigens::SquareMatrix, double>);
}

TEST_CASE("Square matrix test: a matrix has the size it was constructed with.", "[square_matrix][SquareMatrix][constructor][SquareMatrix(Size)][SquareMatrix(Container)]") {
    // GIVEN a valid size for a matrix
    // WHEN  the matrix is constructed
    // THEN  the size of the matrix is the correct one
    //
    // Integer parameter constructor.
    // Minimum size. Both a signed and an unsigned value.
    CHECK(hmeigens::SquareMatrix{1}.size() == 1);
    CHECK(hmeigens::SquareMatrix{1u}.size() == 1);
    // Non-trivial size.
    CHECK(hmeigens::SquareMatrix{500}.size() == 500);
    CHECK(hmeigens::SquareMatrix{500u}.size() == 500);
    // Body parameter constructor.
    // Minimum size.
    CHECK(hmeigens::SquareMatrix{hmeigens::SquareMatrix::Container(1)}.size() == 1);
    // Non-trivial size.
    CHECK(hmeigens::SquareMatrix{hmeigens::SquareMatrix::Container(500*500)}.size() == 500);
}

TEST_CASE("Square matrix test: the constructors correctly report invalid sizes.", "[square_matrix][SquareMatrix][constructor][SquareMatrix(Size)][SquareMatrix(Container)]") {
    // GIVEN an invalid size
    // WHEN  the matrix is constructed
    // THEN  the correct exception is thrown
    //
    // The message is not checked here, as it is checked in tests/detail/square_matrix_helpers_test.cpp.
    // Size parameter constructor.
    // Size 0.
    CHECK_THROWS_AS(
        hmeigens::SquareMatrix(0),
        std::invalid_argument
    );
    // Size too large.
    CHECK_THROWS_AS(
        hmeigens::SquareMatrix(hmeigens::maxMatrixSize+1),
        std::length_error
    );
    // Negative sizes.
    CHECK_THROWS_AS(
        hmeigens::SquareMatrix(-1),
        std::invalid_argument
    );
    // In a previous version of the code this used to be a compile error, due to a narrowing conversion.
    // Both the () and {} cases are tested to ensure the validation works correctly.
    CHECK_THROWS_AS(
        hmeigens::SquareMatrix{-1},
        std::invalid_argument
    );
    // Body parameter constructor.
    // Container is empty.
    CHECK_THROWS_AS(
        hmeigens::SquareMatrix(hmeigens::SquareMatrix::Container(0)),
        std::invalid_argument
    );
    // Container cannot be interpreted as a square matrix.
    CHECK_THROWS_AS(
        hmeigens::SquareMatrix(hmeigens::SquareMatrix::Container(6)),
        std::invalid_argument
    );
    // NOTE: There is no meaningful way to test for the std::bad_alloc exception. That is generated when the checks pass on the size, but the container cannot allocate, and it's the standard library's job.
    // In the documentation there is still info that the constructor can throw, but testing for it means testing std::vector.
}

TEST_CASE("Square matrix test: the constructors are explicit.", "[square_matrix][SquareMatrix][constructor][SquareMatrix(Size)][SquareMatrix(Container)]") {
    // GIVEN a size
    // WHEN  an implicit conversion to a matrix is attempted
    // THEN  it is refused
    //
    // Size parameter constructor.
    STATIC_REQUIRE_FALSE(
        std::is_convertible_v<
            std::size_t,
            hmeigens::SquareMatrix
        >
    );
    // Body parameter constructor.
    STATIC_REQUIRE_FALSE(
        std::is_convertible_v<
            hmeigens::SquareMatrix::Container,
            hmeigens::SquareMatrix
        >
    );
}

TEST_CASE("Square matrix test: a matrix constructed with the size parameter constructor is zero-filled.", "[square_matrix][SquareMatrix][constructor][SquareMatrix(Size)]") {
    // GIVEN a matrix of size n constructed with the size parameter constructor
    // WHEN  the matrix is read
    // THEN  there are nxn elements whose value is 0
    //
    // The constructor of hmeigens::SquareMatrix::Container used here fills it with zeroes. It's standard library.
    // The 0 ULP tolerance comes from the fact that copying or referencing an element of a matrix must not alter it. Since no arithmetic is performed, it must not be rounded or altered.
    // Trivial size.
    checkAllElementsVersus(hmeigens::SquareMatrix{1}, hmeigens::SquareMatrix::Container(1), 0);
    // Non-trivial size.
    checkAllElementsVersus(hmeigens::SquareMatrix{3}, hmeigens::SquareMatrix::Container(9), 0);
}

TEST_CASE("Square matrix test: a matrix constructed with the body parameter constructor is filled with the appropriate elements in row-major order.", "[square_matrix][SquareMatrix][constructor][SquareMatrix(Container)]") {
    // GIVEN a matrix of size n constructed with the body parameter constructor
    // WHEN  the matrix is read
    // THEN  there are nxn elements which correspond to the provided ones in row-major order
    //
    // The 0 ULP tolerance comes from the fact that copying or referencing an element of a matrix must not alter it. Since no arithmetic is performed, it must not be rounded or altered.
    // Trivial size.
    checkAllElementsVersus(
        hmeigens::SquareMatrix{{{HMS(6.7), HMS(4.2)}}},
        hmeigens::SquareMatrix::Container{{HMS(6.7), HMS(4.2)}},
        0
    );
    // Implicit conversion of int to hmeigens::Complex.
    // In a previous version hmeigens::SquareMatrix{{1}} would be interpreted as a 1x1 0-filled matrix. This case checks for that solved bug.
    checkAllElementsVersus(
        hmeigens::SquareMatrix{{1}},
        hmeigens::SquareMatrix::Container{{HMS(1.), HMS(0.)}},
        0
    );
    // Non-trivial size.
    checkAllElementsVersus(
        hmeigens::SquareMatrix{{{HMS(1.1), HMS(2.2)}, {HMS(3.3), HMS(4.4)}, {HMS(5.5), HMS(6.6)}, {HMS(7.7), HMS(8.8)}}},
        hmeigens::SquareMatrix::Container{{HMS(1.1), HMS(2.2)}, {HMS(3.3), HMS(4.4)}, {HMS(5.5), HMS(6.6)}, {HMS(7.7), HMS(8.8)}},
        0
    );
}

/* --------------------------------------------------------------- */
/* ------------------------- accessors --------------------------- */
/* ------------- hmeigens::SquareMatrix::operator() -------------- */
/* --------------- and hmeigens::SquareMatrix::at ---------------- */
/* --------------------------------------------------------------- */
//
// There is no independent Catch2 tag for the accessors because the only one ever tested alone is hmeigens::SquareMatrix::operator() const, which gets a specific tag used in its test case only.
// This is because it is the "trusted" accessor, i.e. it is used to test the others and in the checkAllElementsVersus helper, so the tag makes it runnable on its own.
//
// Anonymous namespace to make using aliases not global.
namespace {
    // One signed, one unsigned.
    using ValidTypes = std::tuple<
        std::uintmax_t,
        std::intmax_t
        >;
    // One integer, one floating point. Covers the two refusals.
    using InvalidTypes = std::tuple<
        bool,
        double
        >;

    TEMPLATE_LIST_TEST_CASE("Square matrix test: the accessors accept valid types.", "[square_matrix][SquareMatrix][accessors][template]", ValidTypes) {
        // GIVEN a list of valid types
        // WHEN  those types are used on the accessors
        // THEN  they are accepted
        //
        // These cases must be run one by one or any one failing would produce incomprehensible diagnostics. They could be shortened with concepts, but it would overcomplicate the testing suite.
        // The template parameter being named TestType is required by Catch2.
        // Matching types.
        STATIC_REQUIRE(
            requires(
                hmeigens::SquareMatrix matrix,
                TestType index
            ) {
                matrix(index, index);
            }
        );
        STATIC_REQUIRE(
            requires(
                const hmeigens::SquareMatrix matrix,
                TestType index
            ) {
                matrix(index, index);
            }
        );
        STATIC_REQUIRE(
            requires(
                hmeigens::SquareMatrix matrix,
                TestType index
            ) {
                matrix.at(index, index);
            }
        );
        STATIC_REQUIRE(
            requires(
                const hmeigens::SquareMatrix matrix,
                TestType index
            ) {
                matrix.at(index, index);
            }
        );
        // Mismatching types. A short int is deliberately used to avoid it being any of the max_ts.
        STATIC_REQUIRE(
            requires(hmeigens::SquareMatrix matrix,
                TestType index,
                short int otherIndex
            ) {
                matrix(index, otherIndex);
            }
        );
        STATIC_REQUIRE(
            requires(
                const hmeigens::SquareMatrix matrix,
                TestType index,
                short int otherIndex
            ) {
                matrix(index, otherIndex);
            }
        );
        STATIC_REQUIRE(
            requires(
                hmeigens::SquareMatrix matrix,
                TestType index,
                short int otherIndex) {
                    matrix.at(index, otherIndex);
                }
            );
        STATIC_REQUIRE(
            requires(
                const hmeigens::SquareMatrix matrix,
                TestType index,
                short int otherIndex
            ) {
                matrix.at(index, otherIndex);
            }
        );
    }

    TEMPLATE_LIST_TEST_CASE("Square matrix test: the accessors refuse invalid types.", "[square_matrix][SquareMatrix][accessors][template]", InvalidTypes) {
        // GIVEN a list of invalid types
        // WHEN  those types are used on the accessors
        // THEN  they are refused
        //
        // These cases must be run one by one or any one failing would produce incomprehensible diagnostics. They could be shortened with concepts, but it would overcomplicate the testing suite.
        // Also, yes, this case is basically a duplication of the previous one with STATIC_REQUIRE_FALSE. This is unfortunately necessary for the same reason.
        // The template parameter being named TestType is required by Catch2.
        // Matching types.
        STATIC_REQUIRE_FALSE(
            requires(
                hmeigens::SquareMatrix matrix,
                TestType index
            ) {
                matrix(index, index);
            }
        );
        STATIC_REQUIRE_FALSE(
            requires(
                const hmeigens::SquareMatrix matrix,
                TestType index
            ) {
                matrix(index, index);
            }
        );
        STATIC_REQUIRE_FALSE(
            requires(
                hmeigens::SquareMatrix matrix,
                TestType index
            ) {
                matrix.at(index, index);
            }
        );
        STATIC_REQUIRE_FALSE(
            requires(
                const hmeigens::SquareMatrix matrix,
                TestType index
            ) {
                matrix.at(index, index);
            }
        );
        // Mismatching types. A short int is deliberately used to avoid it being any of the max_ts.
        STATIC_REQUIRE_FALSE(
            requires(hmeigens::SquareMatrix matrix,
                TestType index,
                short int otherIndex
            ) {
                matrix(index, otherIndex);
            }
        );
        STATIC_REQUIRE_FALSE(
            requires(
                const hmeigens::SquareMatrix matrix,
                TestType index,
                short int otherIndex
            ) {
                matrix(index, otherIndex);
            }
        );
        STATIC_REQUIRE_FALSE(
            requires(
                hmeigens::SquareMatrix matrix,
                TestType index,
                short int otherIndex) {
                    matrix.at(index, otherIndex);
                }
            );
        STATIC_REQUIRE_FALSE(
            requires(
                const hmeigens::SquareMatrix matrix,
                TestType index,
                short int otherIndex
            ) {
                matrix.at(index, otherIndex);
            }
        );
    }
}

TEST_CASE("Square matrix test: accessors return the correct type.", "[square_matrix][SquareMatrix][accessors]") {
    // GIVEN a matrix
    // WHEN  the accessors' return type is checked
    // THEN  the returned type is correctly qualified
    hmeigens::SquareMatrix testMatrix{2};
    const hmeigens::SquareMatrix testMatrixConst{2};
    // (0,0) is arbitrary and resolved by decltype without needing an actual element.
    STATIC_REQUIRE(
        std::is_same_v<
            decltype(testMatrixConst(0,0)),
            const hmeigens::Complex&
        >
    );
    STATIC_REQUIRE(
        std::is_same_v<
            decltype(testMatrixConst.at(0,0)),
            const hmeigens::Complex&
        >
    );
    STATIC_REQUIRE(
        std::is_same_v<
            decltype(testMatrix(0,0)),
            hmeigens::Complex&
        >
    );
    STATIC_REQUIRE(
        std::is_same_v<
            decltype(testMatrix.at(0,0)),
            hmeigens::Complex&
        >
    );
}

TEST_CASE("Square matrix test: accessor operator() const returns the correct element.", "[square_matrix][SquareMatrix][accessors][operator()_const]") {
    // GIVEN a valid row-column position pair
    // WHEN  hmeigens::SquareMatrix::operator() const is called
    // THEN  the correct element is returned
    //
    // Note: this test requires trusting that the constructor hmeigens::SquareMatrix::SquareMatrix(hmeigens::SquareMatrix::Container&& body) works as intended.
    // The constructor is tested in another case, and its tests require trusting that hmeigens::SquareMatrix::operator() const works as intended.
    // Due to encapsulation it is not possible to fully separate their behaviour: either there is a matrix to read, or nothing can be read at all.
    // The test here is performed versus values independent of the class, via testBody.
    // The other behaviours of the constructor (size, exceptions) can be and are tested separately.
    const hmeigens::SquareMatrix::Container testBody{
        {HMS(1.), HMS(1.)}, {HMS(2.), HMS(2.)}, {HMS(3.), HMS(3.)},
        {HMS(4.), HMS(4.)}, {HMS(5.), HMS(5.)}, {HMS(6.), HMS(6.)},
        {HMS(7.), HMS(7.)}, {HMS(8.), HMS(8.)}, {HMS(9.), HMS(9.)}
    };
    const hmeigens::SquareMatrix testMatrixConst{
        // Needs a copy of testBody, as the constructor moves it.
        hmeigens::SquareMatrix::Container{testBody}
    };
    // The comparison is performed versus testBody.
    // The 0 ULP tolerance comes from the fact that copying or referencing an element of a matrix must not alter it. Since no arithmetic is performed, it must not be rounded or altered.
    checkAllElementsVersus(testMatrixConst, testBody, 0);
}

TEST_CASE("Square matrix test: accessors except operator() const return the correct element.", "[square_matrix][SquareMatrix][accessors]") {
    // GIVEN a valid row-column position pair
    // WHEN  any accessor except hmeigens::SquareMatrix::operator() const is called
    // THEN  the correct element is returned
    //
    // Note: this test requires trusting that hmeigens::SquareMatrix::operator()(Row row, Col col) const works as intended.
    // The operator is tested above, independently of all these accessors.
    // Why SECTIONs here?
    // Each check calls the helper checkComplex. Without SECTIONs a failed test does not say which overload it was.
    // The alternative was using either member-pointers or lambdas. That would be like shooting a fly with a bazooka, so a simple copy-paste here works.
    // The checkAllElementsVersus helper uses only the trusted hmeigens::SquareMatrix::operator() const, so here it would not do what is needed.
    // The 0 ULP tolerance comes from the fact that copying or referencing an element of a matrix must not alter it. Since no arithmetic is performed, it must not be rounded or altered.
    hmeigens::SquareMatrix testMatrix{
        {{HMS(1.), HMS(1.)}, {HMS(2.), HMS(2.)}, {HMS(3.), HMS(3.)},
         {HMS(4.), HMS(4.)}, {HMS(5.), HMS(5.)}, {HMS(6.), HMS(6.)},
         {HMS(7.), HMS(7.)}, {HMS(8.), HMS(8.)}, {HMS(9.), HMS(9.)}}
    };
    // Need both a non const and a const one for the test.
    const hmeigens::SquareMatrix& testMatrixConst = testMatrix;
    SECTION("hmeigens::SquareMatrix::at() const") {
        for(std::size_t row = 0; row < testMatrix.size(); ++row) {
            for(std::size_t col = 0; col < testMatrix.size(); ++col) {
                CAPTURE(row, col);
                checkComplex(testMatrixConst.at(row, col), testMatrixConst(row, col), 0);
            }
        }
    }
    SECTION("hmeigens::SquareMatrix::at()") {
        for(std::size_t row = 0; row < testMatrix.size(); ++row) {
            for(std::size_t col = 0; col < testMatrix.size(); ++col) {
                CAPTURE(row, col);
                checkComplex(testMatrix.at(row, col), testMatrixConst(row, col), 0);
            }
        }
    }
    SECTION("hmeigens::SquareMatrix::operator()") {
        for(std::size_t row = 0; row < testMatrix.size(); ++row) {
            for(std::size_t col = 0; col < testMatrix.size(); ++col) {
                CAPTURE(row, col);
                checkComplex(testMatrix(row, col), testMatrixConst(row, col), 0);
            }
        }
    }
}

TEST_CASE("Square matrix test: const accessors do not allow editing, non-consts do.", "[square_matrix][SquareMatrix][accessors]") {
    // GIVEN a matrix
    // WHEN  the accessors' return type is checked
    // THEN  the const accessors return uneditable references, the non const ones return editable ones
    hmeigens::SquareMatrix testMatrix{2};
    const hmeigens::SquareMatrix& testMatrixConst = testMatrix;
    // (0,0) is arbitrary and resolved by decltype without needing an actual element.
    STATIC_REQUIRE_FALSE(
        std::is_assignable_v<
            decltype(testMatrixConst(0,0)),
            hmeigens::Complex
        >
    );
    STATIC_REQUIRE(
        std::is_assignable_v<
            decltype(testMatrix(0,0)),
            hmeigens::Complex
        >
    );
    STATIC_REQUIRE_FALSE(
        std::is_assignable_v<
            decltype(testMatrixConst.at(0,0)),
            hmeigens::Complex
        >
    );
    STATIC_REQUIRE(
        std::is_assignable_v<
            decltype(testMatrix.at(0,0)),
            hmeigens::Complex
        >
    );
}

TEST_CASE("Square matrix test: edited values persist and are in the right place.", "[square_matrix][SquareMatrix][accessors]") {
    // GIVEN a matrix
    // WHEN  an edit of a value is attempted through the accessors
    // THEN  the correct element is edited, and the correct value is stored afterwards
    //
    // Why SECTIONs here?
    // Each SECTION produces a new matrix. Since this is an editing test, the test is performed on a fresh matrix.
    // The alternative could be, for example, writing different values. A new matrix is better, as if the accessors fuck something up, the tests still remain truly independent of each other.
    hmeigens::SquareMatrix testMatrix{
        {{HMS(1.), HMS(1.)}, {HMS(2.), HMS(2.)}, {HMS(3.), HMS(3.)},
         {HMS(4.), HMS(4.)}, {HMS(5.), HMS(5.)}, {HMS(6.), HMS(6.)},
         {HMS(7.), HMS(7.)}, {HMS(8.), HMS(8.)}, {HMS(9.), HMS(9.)}}
    };
    // A const reference version is needed, as the trusted function for checks is hmeigens::SquareMatrix::operator() const.
    const hmeigens::SquareMatrix& testMatrixConst = testMatrix;
    // The 0 ULP tolerance comes from the fact that copying or referencing an element of a matrix must not alter it. Since no arithmetic is performed, it must not be rounded or altered.
    SECTION("hmeigens::SquareMatrix::at()") {
        // Testing both a diagonal and an off-diagonal element.
        // Edit the elements, then see if their value was updated.
        testMatrix.at(0,0) = hmeigens::Complex{HMS(6.), HMS(7.)};
        testMatrix.at(1,2) = hmeigens::Complex{HMS(4.), HMS(2.)};
        checkComplex(testMatrixConst(0,0), hmeigens::Complex {HMS(6.), HMS(7.)}, 0);
        checkComplex(testMatrixConst(1,2), hmeigens::Complex {HMS(4.), HMS(2.)}, 0);
    }
    SECTION("hmeigens::SquareMatrix::operator()") {
        // Testing both a diagonal and an off-diagonal element.
        // Edit the elements, then see if their value was updated.
        testMatrix(0,0) = hmeigens::Complex{HMS(6.), HMS(7.)};
        testMatrix(1,2) = hmeigens::Complex{HMS(4.), HMS(2.)};
        checkComplex(testMatrixConst(0,0), hmeigens::Complex {HMS(6.), HMS(7.)}, 0);
        checkComplex(testMatrixConst(1,2), hmeigens::Complex {HMS(4.), HMS(2.)}, 0);
    }
}

TEST_CASE("Square matrix test: the overloads of at() are index safe.", "[square_matrix][SquareMatrix][accessors]") {
    // GIVEN a matrix
    // WHEN  an out-of-range row column index pair is given to hmeigens::SquareMatrix::at()
    // THEN  an exception is thrown with the correct type and message
    hmeigens::SquareMatrix testMatrix{3};
    // A const reference version is needed to check both overloads.
    const hmeigens::SquareMatrix& testMatrixConst = testMatrix;
    // The string used by std::format to construct the error message.
    constexpr std::string_view indexErrorString = "Invalid index ({0},{1}) for {2}x{2} matrix. Please note matrices are 0-indexed.";
    // This variable is just for readability of the loops below.
    // The cast is safe because the size is small.
    const std::intmax_t size = static_cast<std::intmax_t>(testMatrixConst.size());
    // 3 cases are relevant:
    // - row out, column ok;
    // - row ok, column out;
    // - row out, column out.
    // The loop below tests each overload on a negative value, a valid one, and one past the end.
    for(const std::intmax_t row : {std::intmax_t{-1}, std::intmax_t{0}, size}) {
        for(const std::intmax_t col : {std::intmax_t{-1}, std::intmax_t{0}, size}) {
            if (row == 0 and col == 0) {
                // The (0,0) case of the loop needs to be skipped, as it is not an invalid index.
                continue;
            }
            CAPTURE(row, col);
            // For hmeigens::SquareMatrix::at().
            CHECK_THROWS_MATCHES(
                testMatrix.at(row, col),
                std::out_of_range,
                Catch::Matchers::Message(std::format(indexErrorString, row, col, size))
            );
            // For hmeigens::SquareMatrix::at() const.
            CHECK_THROWS_MATCHES(
                testMatrixConst.at(row, col),
                std::out_of_range,
                Catch::Matchers::Message(std::format(indexErrorString, row, col, size))
            );
        }
    }
}

TEST_CASE("Square matrix test: the overloads of operator() are index safe.", "[square_matrix][SquareMatrix][accessors][may_be_skipped][debug_only]") {
    // GIVEN a matrix
    // WHEN  an out-of-range row column index pair is given to hmeigens::SquareMatrix::operator() IN DEBUG MODE
    // THEN  an exception is thrown with the correct type and message
    //
    // In release mode this test makes no sense, and must be skipped, as the operators do not perform any checks.
    #ifdef NDEBUG
    SKIP("The overloads of hmeigens::SquareMatrix::operator() perform no out-of-range checks in release mode.");
    #endif
    // Yes, the code in this CASE is a duplication of the hmeigens::SquareMatrix::at CASE. This is deliberate.
    // This is because Catch2, if a test is skipped even for a single SECTION, reports it entirely as skipped.
    // The choice made here is that, in this specific instance, code duplication is more acceptable than a less clear test result.
    hmeigens::SquareMatrix testMatrix{3};
    // A const reference version is needed to check both overloads.
    const hmeigens::SquareMatrix& testMatrixConst = testMatrix;
    // The string used by std::format to construct the error message.
    constexpr std::string_view indexErrorString = "Invalid index ({0},{1}) for {2}x{2} matrix. Please note matrices are 0-indexed.";
    // This variable is just for readability of the loops below.
    // The cast is safe because the size is small.
    const std::intmax_t size = static_cast<std::intmax_t>(testMatrixConst.size());
    // 3 cases are relevant:
    // - row out, column ok;
    // - row ok, column out;
    // - row out, column out.
    // The loop below tests each overload on a negative value, a valid one, and one past the end.
    for(const std::intmax_t row : {std::intmax_t{-1}, std::intmax_t{0}, size}) {
        for(const std::intmax_t col : {std::intmax_t{-1}, std::intmax_t{0}, size}) {
            if (row == 0 and col == 0) {
                // The (0,0) case of the loop needs to be skipped, as it is not an invalid index.
                continue;
            }
            CAPTURE(row, col);
            // For hmeigens::SquareMatrix::operator().
            CHECK_THROWS_MATCHES(
                testMatrix(row, col),
                std::out_of_range,
                Catch::Matchers::Message(std::format(indexErrorString, row, col, size))
            );
            // For hmeigens::SquareMatrix::operator() const.
            CHECK_THROWS_MATCHES(
                testMatrixConst(row, col),
                std::out_of_range,
                Catch::Matchers::Message(std::format(indexErrorString, row, col, size))
            );
        }
    }
}

/* --------------------------------------------------------------- */
/* --------------- hmeigens::SquareMatrix::identity -------------- */
/* --------------------------------------------------------------- */
//
TEST_CASE("Square matrix test: the identity matrix has the correct form.", "[square_matrix][SquareMatrix][identity]") {
    // GIVEN a valid size
    // WHEN  hmeigens::SquareMatrix::identity attempts to build an identity matrix of that size
    // THEN  the matrix exists, has 1 on the diagonal, 0 everywhere else
    //
    // The 0 ULP tolerance comes from the fact that copying or referencing an element of a matrix must not alter it. Since no arithmetic is performed, it must not be rounded or altered.
    checkAllElementsVersus(
        hmeigens::SquareMatrix::identity(1),
        hmeigens::SquareMatrix::Container{{{HMS(1.), HMS(0.)}}},
        0
    );
    checkAllElementsVersus(
        hmeigens::SquareMatrix::identity(3),
        hmeigens::SquareMatrix::Container{
            {{HMS(1.), HMS(0.)}, {HMS(0.), HMS(0.)}, {HMS(0.), HMS(0.)},
             {HMS(0.), HMS(0.)}, {HMS(1.), HMS(0.)}, {HMS(0.), HMS(0.)},
             {HMS(0.), HMS(0.)}, {HMS(0.), HMS(0.)}, {HMS(1.), HMS(0.)}}
        },
        0
    );
}

TEST_CASE("Square matrix test: identity throws as expected for invalid sizes.", "[square_matrix][SquareMatrix][identity]") {
    // GIVEN an invalid size
    // WHEN  hmeigens::SquareMatrix::identity attempts to build an identity matrix of that size
    // THEN  it throws exactly like hmeigens::SquareMatrix(Size)
    //
    // Size 0.
    CHECK_THROWS_AS(
        hmeigens::SquareMatrix::identity(0),
        std::invalid_argument
    );
    // Size too large.
    CHECK_THROWS_AS(
        hmeigens::SquareMatrix::identity(hmeigens::maxMatrixSize+1),
        std::length_error
    );
    // Negative sizes.
    CHECK_THROWS_AS(
        hmeigens::SquareMatrix::identity(-1),
        std::invalid_argument
    );
}


/* --------------------------------------------------------------- */
/* ----------------- hmeigens::SquareMatrix::norm ---------------- */
/* --------------------------------------------------------------- */
//
// The tolerance for the tests is variable due to limitations of std::hypot. The precision is implementation defined. For an extensive discussion see the documentation of hmeigens::normTolerance in tolerances.hpp.
//
TEST_CASE("Square matrix test: the Frobenius norm gives the expected values.", "[square_matrix][SquareMatrix][norm]") {
    // GIVEN a matrix
    // WHEN  hmeigens::SquareMatrix::norm() computes the norm
    // THEN  the value returned is the expected one
    //
    // Null matrix: the norm must be exactly 0.
    checkReal(hmeigens::SquareMatrix{1}.norm(), HMS(0.), 0);
    checkReal(hmeigens::SquareMatrix{100}.norm(), HMS(0.), 0);
    // Identity: the norm must be sqrt(size).
    // Note that std::sqrt() is REQUIRED to be precise by the (IEEE) standard. See https://en.cppreference.com/cpp/numeric/math/sqrt.
    checkReal(hmeigens::SquareMatrix::identity(1).norm(), HMS(1.), hmeigens::normTolerance(1));
    checkReal(hmeigens::SquareMatrix::identity(3).norm(), std::sqrt(HMS(3.)), hmeigens::normTolerance(3));
    checkReal(hmeigens::SquareMatrix::identity(10).norm(), std::sqrt(HMS(10.)), hmeigens::normTolerance(10));
    // Arbitrary matrices.
    // The following norms have been computed by hand and then double checked on WolframAlpha.
    checkReal(hmeigens::SquareMatrix{{HMS(4.)}}.norm(), HMS(4.), hmeigens::normTolerance(1));
    checkReal(
        hmeigens::SquareMatrix{
            {{HMS(1.), HMS(1.)}, {HMS(2.), HMS(2.)}, {HMS(3.), HMS(3.)},
             {HMS(4.), HMS(4.)}, {HMS(5.), HMS(5.)}, {HMS(6.), HMS(6.)},
             {HMS(7.), HMS(7.)}, {HMS(8.), HMS(8.)}, {HMS(9.), HMS(9.)}}
        }.norm(),
        std::sqrt(HMS(570.)),
        hmeigens::normTolerance(3)
    );
    checkReal(
        hmeigens::SquareMatrix{
            {{HMS(3.), HMS(-1.)},  {HMS(0.), HMS(1.4)},       {HMS(0.), HMS(0.)},
             {HMS(0.), HMS(-1.)},  {HMS(11.999999), HMS(0.)}, {HMS(23.), HMS(-12.)},
             {HMS(0.006), HMS(7.)},{HMS(0.000005), HMS(0.)},  {HMS(-90.), HMS(0.)}}
        }.norm(),
        std::sqrt(HMS(8978.960012000026)),
        hmeigens::normTolerance(3)
    );
    checkReal(
        hmeigens::SquareMatrix{
            {{HMS(1e14), HMS(0.)},    {HMS(2e15), HMS(0.)},
             {HMS(3333e13), HMS(0.)}, {HMS(-444e16), HMS(0.)}}
        }.norm(),
        std::sqrt(HMS(197147148989e26)),
        hmeigens::normTolerance(2)
    );
    // Matrices whose elements squared over/underflow. The norm is still a valid number in range. Using std::hypot guarantees the result cannot overflow in the middle of calculations.
    // Value whose square always overflows.
    checkReal(
        hmeigens::SquareMatrix{
            {std::numeric_limits<hmeigens::Scalar>::max(), HMS(0.),
             HMS(0.),                                        HMS(0.)}
        }.norm(),
        std::numeric_limits<hmeigens::Scalar>::max(),
        // In Catch2 Infinity is 1 ULP more than FLT_MAX/DBL_MAX, so this relation must be exact to check it doesn't overflow.
        // Since only 1 element of the matrix is nonzero, std::hypot must be exact, since per IEEE it behaves like std::fabs, which is exact.
        0
    );
    // Value whose square always underflows.
    checkReal(
        hmeigens::SquareMatrix{
            {std::numeric_limits<hmeigens::Scalar>::min(), HMS(0.),
             HMS(0.),                                        HMS(0.)}
        }.norm(),
        std::numeric_limits<hmeigens::Scalar>::min(),
        0
    );
    // Overflow is inf.
    CHECK(
        std::isinf(hmeigens::SquareMatrix{
            {std::numeric_limits<hmeigens::Scalar>::max(), HMS(0.),
             std::numeric_limits<hmeigens::Scalar>::max(), HMS(0.)}
        }.norm())
    );
}

TEST_CASE("Square matrix test: the Frobenius norm behaves like a norm.", "[square_matrix][SquareMatrix][norm]") {
    // GIVEN a matrix M
    // WHEN  the norm axioms are tested for
    // THEN  they are satisfied
    //
    // These tests can never be done in general, but seeing that they are satisfied even for a single case is still useful to verify the norm behaves as intended.
    //
    // Axiom 1: positive definiteness.
    // The case norm(O)=0, with O null matrix, is tested for in the "expected values" case.
    // The case norm(A)>0 for A!=O is indirectly tested for in the "expected values" case, as all expected values are positive.
    // The value that is closest to 0 still gives a non-zero norm.
    CHECK(hmeigens::SquareMatrix{{std::numeric_limits<hmeigens::Scalar>::denorm_min()}}.norm()>HMS(0.));
    // Axiom 2: absolute homogeneity.
    // Given a scalar chosen as 3-4i, its norm is 5.
    // The second matrix is the first multiplied by 3-4i. Computation was done by hand and double-checked with WolframAlpha.
    // One norm is multiplied by 5. The multiplication is correctly rounded, as per IEEE, so it adds no extra tolerance.
    // The value of the norm is however already rounded, and multiplying it at most doubles its rounding error in ULPs. This means the tolerance on the first parameter must be doubled.
    // Since norm() is used twice, the second parameter's tolerance must also be added to the first one, giving a total of 2+1=3 tolerance.
    checkReal(
        HMS(5.)*
        hmeigens::SquareMatrix{
            {{HMS(1.), HMS(1.5)},   {HMS(2.), HMS(-1.)},
             {HMS(1.25), HMS(-1.)}, {HMS(0.), HMS(0.)}}
        }.norm(),
        hmeigens::SquareMatrix{
            {{HMS(9.), HMS(0.5)},   {HMS(2.), HMS(-11.)},
             {HMS(-.25), HMS(-8.)}, {HMS(0.), HMS(0.)}}
        }.norm(),
        3*hmeigens::normTolerance(2)
    );
    // Axiom 3: triangle inequality.
    // The third matrix is the sum of the first two. Computation was done by hand and double-checked with WolframAlpha.
    // Tolerance here is meaningless as the values are very far apart.
    CHECK(
        hmeigens::SquareMatrix{
            {{HMS(1.), HMS(0.)},  {HMS(2.), HMS(0.)},
             {HMS(-2.), HMS(0.)}, {HMS(9.), HMS(0.)}}
        }.norm() +
        hmeigens::SquareMatrix{
            {{HMS(-1.), HMS(3.)},   {HMS(2.), HMS(-2.)},
             {HMS(-1.3), HMS(7.6)}, {HMS(9.), HMS(9.)}}
        }.norm() >=
        hmeigens::SquareMatrix{
            {{HMS(0.), HMS(3.)},    {HMS(4.), HMS(-2.)},
             {HMS(-3.3), HMS(7.6)}, {HMS(18.), HMS(9.)}}
        }.norm()
    );
}

TEST_CASE("Square matrix test: the Frobenius norm is submultiplicative.", "[square_matrix][SquareMatrix][norm]") {
    // GIVEN two matrices A, B
    // WHEN  the equality norm(A*B)<=norm(A)*norm(B) is tested for
    // THEN  it holds
    SKIP("The member hmeigens::SquareMatrix::operator* is not implemented yet.");
}

TEST_CASE("Square matrix test: the Frobenius norm satisfies the trace identity.", "[square_matrix][SquareMatrix][norm]") {
    // GIVEN a matrix M and its adjoint M_h
    // WHEN  the equality norm(M) =  sqrt(trace(M_h * M)) is tested for
    // THEN  it holds
    SKIP("The members hmeigens::SquareMatrix::adjoint, hmeigens::SquareMatrix::operator*, and hmeigens::SquareMatrix::trace are not implemented yet.");
}

TEST_CASE("Square matrix test: the Frobenius norm is invariant under unitary similarity transformations.", "[square_matrix][SquareMatrix][norm]") {
    // GIVEN a matrix M, a unitary matrix U, and its adjoint U_h
    // WHEN  the equality norm(M) = norm(U*M*U_h) is tested for
    // THEN  it holds
    SKIP("The members hmeigens::SquareMatrix::adjoint and hmeigens::SquareMatrix::operator* are not implemented yet.");
}

#undef HMS
