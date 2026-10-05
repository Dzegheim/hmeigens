/// @file
/// @brief A class for square matrices with complex elements.
#ifndef HMEIGENS_SQUARE_MATRIX_HPP
#define HMEIGENS_SQUARE_MATRIX_HPP

#include "hmeigens/scalar.hpp"
#include "hmeigens/constants.hpp"
#include "hmeigens/detail/isqrt.hpp"
#include "hmeigens/detail/square_matrix_helpers.hpp"

#include <vector>
#include <cstddef>      // For std::size_t
#include <type_traits>  // For std::is_arithmetic_v
#include <algorithm>    // For std::min

namespace hmeigens {

    /// @brief A `concept` holding a list of types that are allowed to become a size.
    ///
    /// This is to avoid something like a `bool` or a `wchar_t` being interpreted as a size. If that is meant to be one, it must be explicitly converted, because hmeigens::SquareMatrix('w') is not meaningful.
    /// The types allowed to be sizes are:
    /// - `short int`;
    /// - `int`;
    /// - `long int`;
    /// - `long long int`;
    /// - `unsigned short int`;
    /// - `unsigned int`;
    /// - `unsigned long int`;
    /// - `unsigned long long int`.
    /// @note All the types defined in `<cstddef>`, `<cstdint>`, etc... work, as long as the compiler used maps them to any of the types above.
    template<typename Candidate>
    // A Candidate is checked against the allowlist by hmeigens::detail::IsItAllowed.
    concept CanBeSize = detail::IsItAllowed<
        Candidate,
        // Allowed types.
        short int,
        int,
        long int,
        long long int,
        unsigned short int,
        unsigned int,
        unsigned long int,
        unsigned long long int
        >;

    /// @brief Square matrix with complex elements.
    ///
    /// The size is fixed at construction and cannot be changed.
    /// The elements are stored in row-major order, so `hmeigens::SquareMatrix::Container` `{1,2,3,4,5,6,7,8,9}` represents the matrix
    /// ```
    /// | 1   2   3 |
    /// | 4   5   6 |
    /// | 7   8   9 |
    /// ```
    /// with a size of `3`.
    /// @note The size of a matrix is a **positive** number. A size `0` matrix **cannot** be constructed.
    class SquareMatrix {
        public:
        /// @brief Alias for the data container used to represent the matrix's elements.
        ///
        /// At the moment, it is `std::vector<hmeigens::Complex>`.
        using Container = std::vector<Complex>;

        private:
        // Size of the matrix.
        // A square matrix is a size_ * size_ table of numbers.
        std::size_t size_;
        // The content of the matrix, stored row-major.
        // IMPORTANT: This MUST live after size_ or:
        // - the constructor could try to allocate an invalid size;
        // - the constructor could try to read the size of a container after it has already been moved into body_.
        Container body_;

        public:
        /// @brief Constructor for a zero-filled `size * size` matrix.
        ///
        /// The size of the matrix is validated at creation, and cannot be altered afterwards. A matrix cannot have a size:
        /// - <=0 (meaningless);
        /// - so large that `size * size` cannot be represented;
        /// - so large that it cannot be stored on the machine.
        ///
        /// Example:
        /// ```cpp
        /// #include "hmeigens/square_matrix.hpp"
        ///
        /// using hmeigens::SquareMatrix;
        ///
        /// int main() {
        ///     // A 1x1 zero-filled matrix.
        ///     // Note that this is different from SquareMatrix A{{1}}, as that is a 1x1 matrix with (1,0) as its only element.
        ///     SquareMatrix A{1};
        ///     // A 3x3 zero-filled matrix.
        ///     SquareMatrix B{3};
        /// }
        /// ```
        /// @param size The size for the matrix.
        /// @throws std::invalid_argument if `size` is `<=0`.
        /// @throws std::length_error if `size > hmeigens::maxMatrixSize`.
        /// @throws std::bad_alloc if the memory could not be allocated on the machine.
        /// @sa SquareMatrix(Container&&)
        template<CanBeSize Size>
        explicit SquareMatrix(Size size);

        /// @brief Constructor for a matrix with an already known `body`.
        ///
        /// The size of the matrix is validated at creation, and cannot be altered afterwards. A matrix cannot be built from a `body` that:
        /// - is empty, as it would have `.size()` 0 (meaningless);
        /// - has a number of elements that is not a perfect square, as it cannot be interpreted as a square matrix.
        ///
        /// Example:
        /// ```cpp
        /// #include "hmeigens/square_matrix.hpp"
        /// #include "hmeigens/scalar.hpp"
        ///
        /// using hmeigens::SquareMatrix;
        /// using hmeigens::operator""_hs;
        ///
        /// int main() {
        ///     // A 1x1 matrix with (4.2, 6.7) as its only element.
        ///     SquareMatrix A{{{4.2_hs, 6.7_hs}}};
        ///     // A 1x1 matrix with (1,0) as its only element.
        ///     // Note that this is different from SquareMatrix B{1}, as that is a 1x1 zero-filled matrix.
        ///     SquareMatrix B{{1}};
        ///     // A 3x3 matrix whose members are, in order, the pairs (1,1) to (9,9).
        ///     SquareMatrix C {
        ///         {{1._hs, 1._hs}, {2._hs, 2._hs}, {3._hs, 3._hs},
        ///          {4._hs, 4._hs}, {5._hs, 5._hs}, {6._hs, 6._hs},
        ///          {7._hs, 7._hs}, {8._hs, 8._hs}, {9._hs, 9._hs}}
        ///     };
        /// }
        /// ```
        /// @param body The elements of the matrix, row-major. The parameter must be able to represent a square matrix, i.e. the number of its elements must be a perfect square. The square root of the number of elements will be the `size` of the matrix.
        /// @throws std::invalid_argument if `body` is empty or not a perfect square.
        /// @note The parameter `body` is moved into the matrix. It must be an rvalue, and it will not be valid after the operation.
        /// @sa SquareMatrix(Size)
        explicit SquareMatrix(Container&& body);

        /// @brief Family of deleted constructors.
        ///
        /// Any type that satisfies `std::is_arithmetic_v`, i.e. integers and floating-points, but is not a type belonging to `hmeigens::CanBeSize`, is not valid as a size.
        /// @sa SquareMatrix(Size)
        template<typename Rejected>
        requires(
            std::is_arithmetic_v<Rejected> and not CanBeSize<Rejected>
        )
        explicit SquareMatrix(Rejected) = delete;

        /// @brief A getter for the size of the matrix.
        ///
        /// The size is the number of rows/columns of the matrix.
        /// @return The size of the matrix.
        [[nodiscard]] std::size_t size() const;

        /// @brief Read access operator for the element in position (`row`, `col`), `0`-indexed.
        ///
        /// @param row The row of the element to access.
        /// @param col The column of the element to access.
        /// @return A read-only reference to the element in position (`row`, `col`).
        /// @warning The returned reference is only valid as long as the matrix also is.
        /// @pre row < size() and col < size()
        /// @warning This operator does not perform out-of-bound checks in release mode. For a range safe version, see `hmeigens::SquareMatrix::at()`.
        /// In debug mode (i.e. if `NDEBUG` is not defined) the operator behaves and throws exactly like `hmeigens::SquareMatrix::at()`.
        /// @sa operator()(std::size_t, std::size_t)
        /// @sa at(std::size_t, std::size_t) const
        /// @sa at(std::size_t, std::size_t)
        [[nodiscard]] const Complex& operator()(std::size_t row, std::size_t col) const;

        /// @brief Write access operator for the element in position (`row`, `col`), `0`-indexed.
        ///
        /// @param row The row of the element to access.
        /// @param col The column of the element to access.
        /// @return A reference to the element in position (`row`, `col`).
        /// @warning The returned reference is only valid as long as the matrix also is.
        /// @pre row < size() and col < size()
        /// @warning This operator does not perform out-of-bound checks in release mode. For a range safe version, see `hmeigens::SquareMatrix::at()`.
        /// In debug mode (i.e. if `NDEBUG` is not defined) the operator behaves and throws exactly like `hmeigens::SquareMatrix::at()`.
        /// @sa operator()(std::size_t, std::size_t) const
        /// @sa at(std::size_t, std::size_t) const
        /// @sa at(std::size_t, std::size_t)
        [[nodiscard]] Complex& operator()(std::size_t row, std::size_t col);

        /// @brief Out-of-bound safe read access for the element in position (`row`, `col`), `0`-indexed.
        ///
        /// @param row The row of the element to access.
        /// @param col The column of the element to access.
        /// @return A read-only reference to the element in position (`row`, `col`).
        /// @warning The returned reference is only valid as long as the matrix also is.
        /// @throws std::out_of_range if `row >= size()` or `col >= size()`.
        /// @sa operator()(std::size_t, std::size_t) const
        /// @sa operator()(std::size_t, std::size_t)
        /// @sa at(std::size_t, std::size_t)
        [[nodiscard]] const Complex& at(std::size_t row, std::size_t col) const;

        /// @brief Out-of-bound safe write access for the element in position (`row`, `col`), `0`-indexed.
        ///
        /// @param row The row of the element to access.
        /// @param col The column of the element to access.
        /// @return A reference to the element in position (`row`, `col`).
        /// @warning The returned reference is only valid as long as the matrix also is.
        /// @throws std::out_of_range if `row >= size()` or `col >= size()`.
        /// @sa operator()(std::size_t, std::size_t) const
        /// @sa operator()(std::size_t, std::size_t)
        /// @sa at(std::size_t, std::size_t) const
        [[nodiscard]] Complex& at(std::size_t row, std::size_t col);

        /// @brief Static member function that constructs an identity matrix of the given `size`.
        ///
        /// @param size The size of the requested identity matrix.
        /// @return An `hmeigens::SquareMatrix` object of the requested `size` whose body is the corresponding identity matrix.
        /// @throws std::invalid_argument if `size` is `<=0`.
        /// @throws std::length_error if `size > hmeigens::maxMatrixSize`.
        /// @throws std::bad_alloc if the memory could not be allocated on the machine.
        /// @sa SquareMatrix(Size)
        template<CanBeSize Size>
        [[nodiscard]] static SquareMatrix identity(Size size);

        private:
        // Member function for computing the index of an element given the row and col (column).
        // All in a single place, so it doesn't need to be repeated every time an index is needed.
        [[nodiscard]] std::size_t getIndex(std::size_t row, std::size_t col) const;

        // Member function to verify that an index is within the confines of the matrix.
        // If either coordinate is greater or equal to size, it throws an std::out_of_range exception.
        void checkIndex(std::size_t row, std::size_t col) const;
    };

    /// @brief The maximum size that a square matrix can have.
    ///
    /// The maximum size must account for the fact that a matrix contains `size * size` elements. The value `size` is verified against this constant, so if `size  > hmeigens::maxMatrixSize` either:
    /// - its square would overflow std::size_t;
    /// - the number of elements cannot be held within hmeigens::SquareMatrix::Container.
    /// @note This is the only upper bound on the matrix size enforced by this code. If a `1'000'000`x`1'000'000` matrix is created, the user is expected to know what they're doing by creating such a large table of numbers. Who even needs such a large matrix? (:
    inline constexpr std::size_t maxMatrixSize = std::min(
        maxSquarableSize,
        detail::isqrt(
            hmeigens::SquareMatrix::Container{}.max_size()
        )
    );
    // These two assertions verify that the operation above behaved properly.
    static_assert(maxMatrixSize <= maxSquarableSize, "The variable hmeigens::maxMatrixSize cannot overflow when squared.");
    static_assert(maxMatrixSize * maxMatrixSize <= SquareMatrix::Container{}.max_size(), "The variable hmeigens::maxMatrixSize squared must represent a valid number of elements for hmeigens::SquareMatrix::Container.");
}

/* ------------------------------*/
/* -------- Definitions -------- */
/* ------------------------------*/
//
// Size parameter constructor.
// Takes the size as a parameter, validates it via helper, then if everything's fine it initializes the matrix as a 0 filled hmeigens::SquareMatrix::Container whose length is size*size.
// If there is a problem with the size, the helper function throws.
// Fully documented at the declaration.
template<hmeigens::CanBeSize Size>
hmeigens::SquareMatrix::SquareMatrix(Size size) :
    // Validating HERE is important, because if it's done later, there could be an attempt to make a hmeigens::SquareMatrix::Container with an invalid size.
    size_(hmeigens::detail::validateSize(size)),
    // Using size_ for initialization makes it so that if the members are somehow swapped in the header, -Wuninitialized (i. e. -Wall) would complain.
    // The container may generate a std::bad_alloc. That is deliberately not handled here.
    body_(size_*size_) {}

/// @cond
// Doxygen 1.18.0 (used to write the documentation for this project) sometimes has issues with template member functions defined outside of their namespace.
// Issue opened at https://github.com/doxygen/doxygen/issues/12379 as no relevant duplicate was found.
// It thinks this is another function. This conditional prevents it from showing up as an undocumented duplicate.
//
// Creates a size * size identity matrix if size is valid.
// Throws (via the constructor) otherwise.
// Fully documented at the declaration.
template<hmeigens::CanBeSize Size>
hmeigens::SquareMatrix hmeigens::SquareMatrix::identity(Size size) {
    hmeigens::SquareMatrix id{size};
    // The value of size could be signed or a different unsigned type.
    // The loop must be checked against id.size().
    for (std::size_t row = 0; row < id.size(); ++row) {
        id(row, row) = {{1.0_hs, 0.0_hs}};
    }
    return id;
}
/// @endcond

#endif
