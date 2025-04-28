#if !defined(_MATRIX_OP_H_)
#define _MATRIX_OP_H_
#include "matrix.hpp"
#include "vector.hpp"

namespace flib{

    class sycl_handler;
    

    //Matrix times a vector Ax = b

    template <typename T>
    Vector<T> prod(const Matrix<T>& A, const Vector<T>& v){

        size_t rowsA = A.getRows();
        size_t colsA = A.getCols();
        if(colsA != v.size()){
            throw std::invalid_argument("Matrix and vector dimensions do not match for multiplication");
        }
        Vector<T> result(v.size());
        sycl::queue Q = flib::sycl_handler::get_queue();

        {
            sycl::buffer<T, 1> buffv = v.to_sycl_buffer();
            sycl::buffer<T, 1> buffa = A.to_sycl_buffer();
            sycl::buffer<T, 1> buffr = result.to_sycl_buffer();
            Q.submit([&](sycl::handler &cgh){
                auto acc_matA = buffa.template get_access<sycl::access::mode::read>(cgh);
                auto acc_vecV = buffv.template get_access<sycl::access::mode::read>(cgh);
                auto acc_vecR = buffr.template get_access<sycl::access::mode::write>(cgh);
                cgh.parallel_for(sycl::range<1>(rowsA),[=](sycl::item<1> item){
                    const int i = item.get_id(0); // is like: for (int i = 0; i < rowsA; i++)
                    T sum = 0.0; 
                    for (int j = 0; j < colsA; j++)
                    {
                        sum += acc_matA[i*colsA + j]*acc_vecV[j];
                    }
                    acc_vecR[i] = sum;
                });
            });
        }

        return result;


    }

    // Other matrix operations can be defined similarly...
    template <typename T>
    Matrix<T> gemm(const Matrix<T> &A, const Matrix<T> &B)
    {
        size_t colsA = A.getCols();
        size_t rowsA = A.getRows();
        size_t colsB = B.getCols();
        size_t rowsB = B.getRows();
        size_t colsC = colsB;
        size_t rowsC = rowsA;
        Matrix<T> C(rowsC, colsC);
        if (colsA != rowsB)
        {
            //for matrix multiplication, the number of columns in A must be equal to the number of rows in B
            //because tjhe result matrix will have the same number of rows as A and the same number of columns as B
            throw std::invalid_argument("Matrix dimensions do not match for multiplication");
        }
        sycl::queue Q = flib::sycl_handler::get_queue();
           
        { //Sycl scope
            sycl::buffer<T, 1> buffc  = C.to_sycl_buffer();
            sycl::buffer<T, 1> buffa  = A.to_sycl_buffer();
            sycl::buffer<T, 1> buffb  = B.to_sycl_buffer();
            Q.submit([&](sycl::handler &cgh){
                
                auto acc_matC = buffc.template get_access<sycl::access::mode::write>(cgh);
                auto acc_matA = buffa.template get_access<sycl::access::mode::read>(cgh);
                auto acc_matB = buffb.template get_access<sycl::access::mode::read>(cgh);
                cgh.parallel_for(sycl::range<2>(sycl::range<2> {static_cast<size_t>(rowsC),static_cast<size_t>(colsC)}),[=](sycl::item<2> item){
                    const int i = item.get_id(0); // is like: for (int i = 0; i < rowsA; i++)
                    const int j = item.get_id(1); // is like: for (int j = 0; j < colsB; j++)
                    T sum = 0.0; 
                    for (int k = 0; k < colsA; k++)
                    {
                        sum += acc_matA[i*colsA + k]*acc_matB[j + colsB*k];
                    }
                    acc_matC[i*colsB+j] = sum;
                });
            });
        }
       
       return  C;
    }


}

#endif // _MATRIX_OP_H_
