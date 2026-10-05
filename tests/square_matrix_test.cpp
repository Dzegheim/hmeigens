#include "hmeigens/constants.hpp"
#include "hmeigens/square_matrix.hpp"

using hmeigens::operator""_hs;

#include <stdexcept>     // For std::out_of_range, std::length_error, std::invalid_argument
#include <cstddef>       // For std::size_t
#include <cstdint>       // For std::intmax_t, std::uintmax_t
#include <format>
#include <string_view>
#include <type_traits>   // For std::is_same, std::is_assignable_v, std::is_convertible_v, std::is_constructible_v

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>
#include <catch2/matchers/catch_matchers_exception.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

// This helper checks that two complex numbers passed to it are "equal", i.e. that their real and imaginary parts are both within 0 ULPs of each other.
// The 0 ULP tolerance comes from the fact that copying or referencing an element of a matrix must not alter it.
// Since no arithmetic is performed, it must not be rounded or altered.
static void checkComplex(const hmeigens::Complex& toCheck, const hmeigens::Complex& expected) {
    CAPTURE(toCheck, expected);
    CHECK_THAT(
        // Real part check.
        toCheck.real(),
        Catch::Matchers::WithinULP(expected.real(), 0)
    );
    CHECK_THAT(
        // Imaginary part check.
        toCheck.imag(),
        Catch::Matchers::WithinULP(expected.imag(), 0)
    );
    return;
}

// This helper checks that all elements of a matrix match the ones of a reference hmeigens::SquareMatrix::Container.
// It accesses the elements via hmeigens::SquareMatrix::operator() const.
static void checkAllElementsVersus(const hmeigens::SquareMatrix& toCheck, const hmeigens::SquareMatrix::Container& referenceBody) {
    // If the sizes don't match, this test makes no sense.
    REQUIRE(toCheck.size() * toCheck.size() == referenceBody.size());
    for (std::size_t row = 0; row < toCheck.size(); ++row) {
        for (std::size_t col = 0; col < toCheck.size(); ++col) {
            CAPTURE(row, col);
            // Every number is verified against the reference body.
            // The container's indexing is the same as the private member hmeigens::SquareMatrix::getIndex().
            // Here it is written explicitly so that it is independent from the class.
            // This also enforces the row-major order of the elements. This helper IS MEANT TO break if the class' internal index computation changes, as the class is supposed to be row-major.
            checkComplex(
                toCheck(row, col),
                referenceBody[row * toCheck.size() + col]
            );
        }
    }
    return;
}

TEST_CASE("Square matrix test: only the allowed types are accepted as sizes.", "[square_matrix]") {
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

TEST_CASE("Square matrix test: the size parameter constructor accepts valid types and refuses invalid ones.", "[square_matrix]") {
    // GIVEN a type
    // WHEN  the compiler checks if it is valid
    // THEN  the allowed ones are accepted, the others are not
    STATIC_REQUIRE(std::is_constructible_v<hmeigens::SquareMatrix, int>);
    STATIC_REQUIRE_FALSE(std::is_constructible_v<hmeigens::SquareMatrix, double>);
}

TEST_CASE("Square matrix test: a matrix has the size it was constructed with.", "[square_matrix]") {
    // GIVEN a valid size for a matrix
    // WHEN  the matrix is constructed
    // THEN  the size of the matrix is the correct one
    //
    // Integer parameter constructor.
    // Minimum size. Both a signed and an unsigned value.
    CHECK(hmeigens::SquareMatrix{1}.size() == 1);
    CHECK(hmeigens::SquareMatrix{1u}.size() == 1);
    // Non trivial size.
    CHECK(hmeigens::SquareMatrix{500}.size() == 500);
    CHECK(hmeigens::SquareMatrix{500u}.size() == 500);
    // Body parameter constructor.
    // Minimum size.
    CHECK(hmeigens::SquareMatrix{hmeigens::SquareMatrix::Container(1)}.size() == 1);
    // Non trivial size.
    CHECK(hmeigens::SquareMatrix{hmeigens::SquareMatrix::Container(500*500)}.size() == 500);
}

TEST_CASE("Square matrix test: invalid sizes are correctly reported.", "[square_matrix]") {
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

TEST_CASE("Square matrix test: the constructors are explicit.", "[square_matrix]") {
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

TEST_CASE("Square matrix test: accessors return the correct type.", "[square_matrix]") {
    // GIVEN a matrix
    // WHEN  the accessors' return type is checked
    // THEN  the returned type is correctly qualified
    hmeigens::SquareMatrix testMatrix{2};
    const hmeigens::SquareMatrix testMatrixConst{2};
    // (0,0) is arbitrary and resolved by decltype without needing an actual element.
    STATIC_REQUIRE(
        std::is_same_v<
            decltype(testMatrixConst(0, 0)),
            const hmeigens::Complex&
        >
    );
    STATIC_REQUIRE(
        std::is_same_v<
            decltype(testMatrixConst.at(0, 0)),
            const hmeigens::Complex&
        >
    );
    STATIC_REQUIRE(
        std::is_same_v<
            decltype(testMatrix(0, 0)),
            hmeigens::Complex&
        >
    );
    STATIC_REQUIRE(
        std::is_same_v<
            decltype(testMatrix.at(0, 0)),
            hmeigens::Complex&
        >
    );
}

TEST_CASE("Square matrix test: accessor operator() const returns the correct element.", "[square_matrix]") {
    // GIVEN a valid row-column position pair
    // WHEN  hmeigens::SquareMatrix::operator() const is called
    // THEN  the correct element is returned
    //
    // Note: this test requires trusting that the constructor hmeigens::SquareMatrix::SquareMatrix (std::size_t size, hmeigens::SquareMatrix::Container&& body) works as intended.
    // The constructor is tested below, and its tests require trusting that hmeigens::SquareMatrix::operator() const works as intended.
    // Due to encapsulation it is not possible to fully separate their behaviour: either there is a matrix to read, or nothing can be read at all.
    // The test here is performed versus values independent of the class, via testBody.
    // The other behaviours of the constructor (size, exceptions) can and are tested separately.
    const hmeigens::SquareMatrix::Container testBody{
        {1._hs, 1._hs}, {2._hs, 2._hs}, {3._hs, 3._hs},
        {4._hs, 4._hs}, {5._hs, 5._hs}, {6._hs, 6._hs},
        {7._hs, 7._hs}, {8._hs, 8._hs}, {9._hs, 9._hs}
    };
    const hmeigens::SquareMatrix testMatrixConst{
        // Needs a copy of testBody, as the constructor moves it.
        hmeigens::SquareMatrix::Container{testBody}
    };
    // The comparison is performed versus testBody.
    checkAllElementsVersus(testMatrixConst, testBody);
}

TEST_CASE("Square matrix test: accessors except operator() const return the correct element.", "[square_matrix]") {
    // GIVEN a valid row-column position pair
    // WHEN  any accessor except hmeigens::SquareMatrix::operator() const is called
    // THEN  the correct element is returned
    //
    // Note: this test requires trusting that hmeigens::SquareMatrix::operator() (std::size_t row, std::size_t col) const works as intended.
    // The operator is tested above, independently of all these accessors.
    // Why SECTIONs here?
    // Each check calls the helper checkComplex. Without SECTIONs a failed test does not say which overload it was.
    // The alternative was using either member-pointers or lambdas. That would be like shooting a fly with a bazooka, so a simple copy-paste here works.
    // The checkAllElementsVersus helper uses only the trusted hmeigens::SquareMatrix::operator() const, so here it would not do what is needed.
    hmeigens::SquareMatrix testMatrix{
        {{1._hs, 1._hs}, {2._hs, 2._hs}, {3._hs, 3._hs},
         {4._hs, 4._hs}, {5._hs, 5._hs}, {6._hs, 6._hs},
         {7._hs, 7._hs}, {8._hs, 8._hs}, {9._hs, 9._hs}}
    };
    // Need both a non const and a const one for the test.
    const hmeigens::SquareMatrix& testMatrixConst = testMatrix;
    SECTION("hmeigens::SquareMatrix::at() const") {
        for (std::size_t row = 0; row < testMatrix.size(); ++row) {
            for (std::size_t col = 0; col < testMatrix.size(); ++col) {
                CAPTURE(row, col);
                checkComplex(testMatrixConst.at(row, col), testMatrixConst(row, col));
            }
        }
    }
    SECTION("hmeigens::SquareMatrix::at()") {
        for (std::size_t row = 0; row < testMatrix.size(); ++row) {
            for (std::size_t col = 0; col < testMatrix.size(); ++col) {
                CAPTURE(row, col);
                checkComplex(testMatrix.at(row, col), testMatrixConst(row, col));
            }
        }
    }
    SECTION("hmeigens::SquareMatrix::operator()") {
        for (std::size_t row = 0; row < testMatrix.size(); ++row) {
            for (std::size_t col = 0; col < testMatrix.size(); ++col) {
                CAPTURE(row, col);
                checkComplex(testMatrix(row, col), testMatrixConst(row, col));
            }
        }
    }
}

TEST_CASE("Square matrix test: const accessors do not allow editing, non-consts do.", "[square_matrix]") {
    // GIVEN a matrix
    // WHEN  the accessors' return type is checked
    // THEN  the const accessors return uneditable references, the non const ones return editable ones
    hmeigens::SquareMatrix testMatrix{2};
    const hmeigens::SquareMatrix& testMatrixConst = testMatrix;
    // (0,0) is arbitrary and resolved by decltype without needing an actual element.
    STATIC_REQUIRE_FALSE(
        std::is_assignable_v<
            decltype(testMatrixConst(0, 0)),
            hmeigens::Complex
        >
    );
    STATIC_REQUIRE(
        std::is_assignable_v<
            decltype(testMatrix(0, 0)),
            hmeigens::Complex
        >
    );
    STATIC_REQUIRE_FALSE(
        std::is_assignable_v<
            decltype(testMatrixConst.at(0, 0)),
            hmeigens::Complex
        >
    );
    STATIC_REQUIRE(
        std::is_assignable_v<
            decltype(testMatrix.at(0, 0)),
            hmeigens::Complex
        >
    );
}

TEST_CASE("Square matrix test: edited values persist and are in the right place.", "[square_matrix]") {
    // GIVEN a matrix
    // WHEN  an edit of a value is attempted through the accessors
    // THEN  the correct element is edited, and the correct value is stored afterwards
    //
    // Why SECTIONs here?
    // Each SECTION produces a new matrix. Since this is an editing test, the test is performed on a fresh matrix.
    // The alternative could be, for example, writing different values. A new matrix is better, as if the accessors fuck something up, the tests still remain truly independent of each other.
    hmeigens::SquareMatrix testMatrix{
        {{1._hs, 1._hs}, {2._hs, 2._hs}, {3._hs, 3._hs},
         {4._hs, 4._hs}, {5._hs, 5._hs}, {6._hs, 6._hs},
         {7._hs, 7._hs}, {8._hs, 8._hs}, {9._hs, 9._hs}}
    };
    // A const reference version is needed, as the trusted function for checks is hmeigens::SquareMatrix::operator() const.
    const hmeigens::SquareMatrix& testMatrixConst = testMatrix;
    SECTION("hmeigens::SquareMatrix::at()") {
        // Testing both a diagonal and an off-diagonal element.
        // Edit the elements, then see if their value was updated.
        testMatrix.at(0,0) = hmeigens::Complex{6._hs, 7._hs};
        testMatrix.at(1,2) = hmeigens::Complex{4._hs, 2._hs};
        checkComplex(testMatrixConst(0,0), hmeigens::Complex {6._hs, 7._hs});
        checkComplex(testMatrixConst(1,2), hmeigens::Complex {4._hs, 2._hs});
    }
    SECTION("hmeigens::SquareMatrix::operator()") {
        // Testing both a diagonal and an off-diagonal element.
        // Edit the elements, then see if their value was updated.
        testMatrix(0,0) = hmeigens::Complex{6._hs, 7._hs};
        testMatrix(1,2) = hmeigens::Complex{4._hs, 2._hs};
        checkComplex(testMatrixConst(0,0), hmeigens::Complex {6._hs, 7._hs});
        checkComplex(testMatrixConst(1,2), hmeigens::Complex {4._hs, 2._hs});
    }
}

TEST_CASE("Square matrix test: the overloads of at() are index safe.", "[square_matrix]") {
    // GIVEN a matrix
    // WHEN  an out-of-range row column index pair is given to hmeigens::SquareMatrix::at()
    // THEN  an exception is thrown with the correct type and message
    hmeigens::SquareMatrix testMatrix{
        {{1._hs, 1._hs}, {2._hs, 2._hs}, {3._hs, 3._hs},
         {4._hs, 4._hs}, {5._hs, 5._hs}, {6._hs, 6._hs},
         {7._hs, 7._hs}, {8._hs, 8._hs}, {9._hs, 9._hs}}
    };
    // A const reference version is needed to check both overloads.
    const hmeigens::SquareMatrix& testMatrixConst = testMatrix;
    // The string used by std::format to construct the error message.
    constexpr std::string_view indexErrorString = "Invalid index ({0},{1}) for {2}x{2} matrix. Please note matrices are 0-indexed.";
    // This variable is just for readability of the loops below.
    const std::size_t size = testMatrixConst.size();
    // 3 cases are relevant:
    // - row out, column ok;
    // - row ok, column out;
    // - row out, column out.
    // The loop below tests each overload on values one past the end.
    for (const std::size_t row : {std::size_t{0}, size}) {
        for (const std::size_t col : {std::size_t{0}, size}) {
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

TEST_CASE("Square matrix test: the overloads of operator() are index safe.", "[square_matrix][may_be_skipped][debug_only]") {
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
    hmeigens::SquareMatrix testMatrix{
        {{1._hs, 1._hs}, {2._hs, 2._hs}, {3._hs, 3._hs},
         {4._hs, 4._hs}, {5._hs, 5._hs}, {6._hs, 6._hs},
         {7._hs, 7._hs}, {8._hs, 8._hs}, {9._hs, 9._hs}}
    };
    // A const reference version is needed to check both overloads.
    const hmeigens::SquareMatrix& testMatrixConst = testMatrix;
    // The string used by std::format to construct the error message.
    constexpr std::string_view indexErrorString = "Invalid index ({0},{1}) for {2}x{2} matrix. Please note matrices are 0-indexed.";
    // This variable is just for readability of the loops below.
    const std::size_t size = testMatrixConst.size();
    // 3 cases are relevant:
    // - row out, column ok;
    // - row ok, column out;
    // - row out, column out.
    // The loop below tests each overload on values one past the end.
    for (const std::size_t row : {std::size_t{0}, size}) {
        for (const std::size_t col : {std::size_t{0}, size}) {
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

TEST_CASE("Square matrix test: a matrix constructed with the size parameter constructor is zero-filled.", "[square_matrix]") {
    // GIVEN a matrix of size n constructed with the size parameter constructor
    // WHEN  the matrix is read
    // THEN  there are nxn elements whose value is 0.
    //
    // The constructor of hmeigens::SquareMatrix::Container used here fills it with zeroes. It's standard library.
    // Trivial size.
    checkAllElementsVersus(hmeigens::SquareMatrix{1}, hmeigens::SquareMatrix::Container(1));
    // Non trivial size.
    checkAllElementsVersus(hmeigens::SquareMatrix{3}, hmeigens::SquareMatrix::Container(9));
}

TEST_CASE("Square matrix test: a matrix constructed with the body parameter constructor is filled with the appropriate elements in row-major order.", "[square_matrix]") {
    // GIVEN a matrix of size n constructed with the body parameter constructor
    // WHEN  the matrix is read
    // THEN  there are nxn elements which correspond to the provided ones in row-major order.
    //
    // Trivial size.
    checkAllElementsVersus(
        hmeigens::SquareMatrix{{{6.7_hs, 4.2_hs}}},
        hmeigens::SquareMatrix::Container{{6.7_hs, 4.2_hs}}
    );
    // Implicit conversion of int to hmeigens::Complex.
    // In a previous version hmeigens::SquareMatrix{{1}} would be interpreted as a 1x1 0-filled matrix. This case checks for that solved bug.
    checkAllElementsVersus(
        hmeigens::SquareMatrix{{1}},
        hmeigens::SquareMatrix::Container{{1.0_hs, 0.0_hs}}
    );
    // Non trivial size.
    checkAllElementsVersus(
        hmeigens::SquareMatrix{{{1.1_hs, 2.2_hs}, {3.3_hs, 4.4_hs}, {5.5_hs, 6.6_hs}, {7.7_hs, 8.8_hs}}},
        hmeigens::SquareMatrix::Container{{1.1_hs, 2.2_hs}, {3.3_hs, 4.4_hs}, {5.5_hs, 6.6_hs}, {7.7_hs, 8.8_hs}}
    );
}

TEST_CASE("Square matrix test: the identity matrix has the correct form.", "[square_matrix]") {
    // GIVEN a valid size
    // WHEN  hmeigens::SquareMatrix::identity attempts to build an identity matrix of that size
    // THEN  the matrix exists, has 1 on the diagonal, 0 everywhere else
    checkAllElementsVersus(
        hmeigens::SquareMatrix::identity(1),
        hmeigens::SquareMatrix::Container{{{1._hs, 0._hs}}}
    );
    checkAllElementsVersus(
        hmeigens::SquareMatrix::identity(3),
        hmeigens::SquareMatrix::Container{
            {{1._hs, 0._hs}, {0._hs, 0._hs}, {0._hs, 0._hs},
             {0._hs, 0._hs}, {1._hs, 0._hs}, {0._hs, 0._hs},
             {0._hs, 0._hs}, {0._hs, 0._hs}, {1._hs, 0._hs}}
        }
    );
}

TEST_CASE("Square matrix test: identity throws as expected for invalid sizes.", "[square_matrix]") {
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
