#include "matrix.hpp"
namespace flib{

    template <typename T>
    Matrix<T>::Matrix() : rows(0), cols(0), data(nullptr) {
        // Default constructor
    }
    template <typename T>
    Matrix<T>::Matrix(std::size_t rows, std::size_t cols) : rows(rows), cols(cols) {
        data = std::make_unique<T[]>(rows * cols);
        for (std::size_t i = 0; i < rows * cols; ++i) {
            data[i] = T(0); // Initialize with default value
        }
    }
    template <typename T>
    Matrix<T>::Matrix(std::size_t rows, std::size_t cols, T* value) : rows(rows), cols(cols) {
        data = std::make_unique<T[]>(rows * cols);
        for (std::size_t i = 0; i < rows * cols; ++i) {
            data[i] = value[i]; // Initialize with provided values
        }
    }
    // Removed incorrect member operator+ definition
    // template <typename T>
    // Matrix<T>::Matrix(sycl::queue Q, std::size_t rows, std::size_t cols, T value) : rows(rows), cols(cols) {
    //     data = std::make_unique<T[]>(rows * cols);
    //     for (std::size_t i = 0; i < rows * cols; ++i) {
    //         data[i] = value; // Initialize with provided values
    //     }
    // }
   
}