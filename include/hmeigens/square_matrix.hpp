/// @file
/// @brief A class for square matrices with complex elements.
#ifndef HMEIGENS_SQUARE_MATRIX_HPP
#define HMEIGENS_SQUARE_MATRIX_HPP

#include "hmeigens/scalar.hpp"

#include <vector>

namespace hmeigens {

    /// @brief Square matrix with complex elements.
    ///
    /// The size is fixed at construction and cannot be changed.
    /// @note The size of a matrix is a **positive** number. A size `0` matrix **cannot** be constructed.
    class SquareMatrix {
        public:
        /// @brief Alias for the data container used to represent the matrix's `body_`. At the moment, it is `std::vector<Complex>`.
        using Container = std::vector<Complex>;
        
        private:
        // Size of the matrix.
        // A square matrix is a size_ * size_ table of numbers.
        std::size_t size_;
        // The content of the matrix, stored row major.
        // The body_ (1,2,3,4) represents the matrix:
        // | 1  2 |
        // | 3  4 |
        // IMPORTANT: This MUST live after size_, or the constructor could try to allocate an invalid size.
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
        explicit SquareMatrix (std::size_t size);

        /// @brief A getter for the size of the matrix.
        ///
        /// The size is the number of rows/columns of the matrix.
        /// @return The size of the matrix.
        std::size_t size() const {
            return size_;
        }
    };
}

#endif
