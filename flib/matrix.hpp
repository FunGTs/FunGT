#if !defined(_MATRIX_H_)
#define _MATRIX_H_
#include <memory>
#include <sycl/sycl.hpp>
#include <iostream>
#include <iomanip>
namespace flib{
    template <typename T>
    class Matrix { 
    
        std::size_t rows;
        std::size_t cols;
        std::unique_ptr<T[]> data;

    
    
    public:
        Matrix(); 
        Matrix(std::size_t rows, std::size_t cols);
        Matrix(std::size_t rows, std::size_t cols, T* value);
        //Matrix(sycl::queue Q,  std::size_t rows, std::size_t cols, T value);
        
       
        std::size_t getRows() const { return rows; }
        std::size_t getCols() const { return cols; }

        T& operator()(std::size_t row, std::size_t col) {
            //used to modify the matrix like: mat(i,j) = 5;
            if(row >= rows || col >= cols) {
                throw std::out_of_range("Index out of range");
            }
            return data[row * cols + col];
        }
        const T& operator()(std::size_t row, std::size_t col) const {
            //used to read the matrix like: val = mat(i,j);
            if(row >= rows || col >= cols) {
                throw std::out_of_range("Index out of range");
            }
            return data[row * cols + col];
        }
        const T& operator[](std::size_t index) const {
            //used to read the matrix like: val = mat[i];
            if(index >= rows * cols) {
                throw std::out_of_range("Index out of range");
            }
            return data[index];
        }
        T& operator[](std::size_t index) {
            //used to modify the matrix like: mat[i] = 5;
            if(index >= rows * cols) {
                throw std::out_of_range("Index out of range");
            }
            return data[index];
        }

        sycl::buffer<T, 1> to_sycl_buffer() const {
            return sycl::buffer<T, 1>(data.get(), sycl::range<1>(rows * cols));
        }
        void print() const {
            for (std::size_t i = 0; i < rows; ++i) {
                for (std::size_t j = 0; j < cols; ++j) {
                    std::cout << std::fixed << std::setprecision(4) << data[i * cols + j] << " ";
                }
                std::cout << std::endl;
            }
        }
    
    }; 
}
#include "matrix_impl.hpp"
#endif // _MATRIX_H_