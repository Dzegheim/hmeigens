/// @file
/// @brief A class for square matrices with complex elements.
#ifndef HMEIGENS_SQUARE_MATRIX_HPP
#define HMEIGENS_SQUARE_MATRIX_HPP

#include "hmeigens/scalar.hpp"

#include <vector>
#include <cstddef>  // For std::size_t

namespace hmeigens {

    /// @brief Square matrix with complex elements.
    ///
    /// The size is fixed at construction and cannot be changed.
    /// The `hmeigens::SquareMatrix::Container` `{1,2,3,4,5,6,7,8,9}` represents the matrix:
    /// ```
    /// | 1   2   3 |
    /// | 4   5   6 |
    /// | 7   8   9 |
    /// ```
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
        // The content of the matrix, stored row major.
        // IMPORTANT: This MUST live after size_ or:
        // - the constructor could try to allocate an invalid size;
        // - the constructor could try to read the size of a container after it has already been moved into body_.
        Container body_; 

        public:       
        /// @brief Constructor for a zero-filled `size * size` matrix.
        ///
        /// The size of the matrix is validated at creation, and cannot be altered afterwards. A matrix cannot have a size:
        /// - 0 (meaningless);
        /// - so large that `size * size` cannot be represented;
        /// - so large that it cannot be stored on the machine.
        /// @param size The size for the matrix.
        /// @throws std::invalid_argument if `size` is `0`.
        /// @throws std::length_error if `size * size` cannot be represented within `std::size_t`.
        /// @throws std::length_error if `size * size` exceeds the maximum allowed number of elements.
        /// @throws std::bad_alloc if the memory could not be allocated on the machine.
        /// @sa SquareMatrix(std::size_t, Container&&).
        explicit SquareMatrix (std::size_t size);

        /// @brief Constructor for a `size * size` matrix with an already known `body`.
        ///
        /// The size of the matrix is validated at creation, and cannot be altered afterwards. A matrix cannot have a size:
        /// - `0` (meaningless);
        /// - so large that `size * size` cannot be represented;
        /// - so large that it cannot be stored on the machine.
        /// @param size The size of the matrix.
        /// @param body The elements of the matrix, row major. The parameter must have `size * size` elements, and it must be an rvalue, as it is not copied but moved.
        /// @throws std::invalid_argument if `size` is `0`.
        /// @throws std::length_error if `size * size` cannot be represented within `std::size_t`.
        /// @throws std::length_error if `size * size` exceeds the maximum allowed number of elements.
        /// @throws std::invalid_argument if there is a mismatch between `size * size` and `body`'s length. This *may* be due to `body`'s length not being a perfect square, i.e. if the body does not map to a square matrix, or it may be due to a simple mismatch in the values.
        /// @sa SquareMatrix(std::size_t).
        explicit SquareMatrix (std::size_t size, Container&& body);

        /// @brief A getter for the size of the matrix.
        ///
        /// The size is the number of rows/columns of the matrix.
        /// @return The size of the matrix.
        [[nodiscard]] std::size_t size() const {
            return size_;
        }

        /// @brief Read access operator for the element in position (`row`, `col`), `0`-indexed.
        ///
        /// @param row The row of the element to access.
        /// @param col The column of the element to access.
        /// @return A copy of the element in position (`row`, `col`).
        /// @pre row < size() and col < size()
        /// @warning This operator does not perform out-of-bound checks in release mode. For a range safe version, see `hmeigens::SquareMatrix::at()`.
        /// In debug mode (i.e. if `NDEBUG` is not defined) the operator behaves and throws exactly like `hmeigens::SquareMatrix::at()`.
        /// @sa operator()(std::size_t, std::size_t)
        /// @sa at(std::size_t, std::size_t) const
        /// @sa at(std::size_t, std::size_t)
        [[nodiscard]] Complex operator() (std::size_t row, std::size_t col) const {
            #ifndef NDEBUG
            checkIndex (row, col);
            #endif
            return body_[getIndex(row, col)];
        }

        /// @brief Write access operator for the element in position (`row`, `col`), `0`-indexed.
        ///
        /// @param row The row of the element to access.
        /// @param col The column of the element to access.
        /// @return A reference to the element in position (`row`, `col`).
        /// @pre row < size() and col < size()
        /// @warning This operator does not perform out-of-bound checks in release mode. For a range safe version, see `hmeigens::SquareMatrix::at()`.
        /// In debug mode (i.e. if `NDEBUG` is not defined) the operator behaves and throws exactly like `hmeigens::SquareMatrix::at()`.
        /// @sa operator()(std::size_t, std::size_t) const
        /// @sa at(std::size_t, std::size_t) const
        /// @sa at(std::size_t, std::size_t)
        [[nodiscard]] Complex& operator() (std::size_t row, std::size_t col) {
            #ifndef NDEBUG
            checkIndex (row, col);
            #endif
            return body_[getIndex(row, col)];
        }

        /// @brief Out-of-bound safe read access for the element in position (`row`, `col`), `0`-indexed.
        ///
        /// @param row The row of the element to access.
        /// @param col The column of the element to access.
        /// @return A copy of the element in position (`row`, `col`).
        /// @throws std::out_of_range if `row >= size()` or `col >= size()`.
        /// @sa operator()(std::size_t, std::size_t) const
        /// @sa operator()(std::size_t, std::size_t)
        /// @sa at(std::size_t, std::size_t)
        [[nodiscard]] Complex at(std::size_t row, std::size_t col) const {
            checkIndex (row, col);
            return body_[getIndex(row, col)];
        }

        /// @brief Out-of-bound safe write access for the element in position (`row`, `col`), `0`-indexed.
        ///
        /// @param row The row of the element to access.
        /// @param col The column of the element to access.
        /// @return A reference to the element in position (`row`, `col`).
        /// @throws std::out_of_range if `row >= size()` or `col >= size()`.
        /// @sa operator()(std::size_t, std::size_t) const
        /// @sa operator()(std::size_t, std::size_t)
        /// @sa at(std::size_t, std::size_t) const
        [[nodiscard]] Complex& at(std::size_t row, std::size_t col) {
            checkIndex (row, col);
            return body_[getIndex(row, col)];
        }

        private:
        // Member function for computing the index of an element given the row and col (column).
        // All in a single place, so it doesn't need to be repeated every time an index is needed.
        [[nodiscard]] std::size_t getIndex (std::size_t row, std::size_t col) const {
            return row * size_ + col;
        }

        // Member function to verify that an index is within the confines of the matrix.
        // If either coordinate is greater or equal to size, it throws an std::out_of_range exception.
        void checkIndex (std::size_t row, std::size_t col) const;
    };
}

#endif
