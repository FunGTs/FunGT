#if !defined(_TENSOR_OPERATIONS_HPP_)
#define _TENSOR_OPERATIONS_HPP_
#include <funlib/Tensor/tensor.hpp>
namespace flib{

    
    class tensor_operations{

        class sycl_handler;

        //This class is used to perform operations on the Tensor class
        //It is a friend class of the Tensor class
        //It is used to perform operations on the Tensor class
        //It is a friend class of the sycl_handler class
        //It is used to perform operations on the Tensor class
        template<typename T>
        static Tensor<T> gemm(const Tensor<T>& A, const Tensor<T>& B);
        template<typename T>
        static Tensor<T> matXvec(const Tensor<T>& A, const Tensor<T>& B);
        public:
            template<typename T>
            static Tensor<T> prod(const Tensor<T>& A, const Tensor<T>& B);
        
            //dot product of two vectors or two one dimensional sets
            template<typename T>
            static T dot(const Tensor<T>& A, const Tensor<T>& B);
            template<typename T>
            static T reduction(const Tensor<T>& A);
           
    
    };
    //class sycl_handler;
    //Matrix times a vector Ax = b or Matrix times a matrix AB = C
   



};


#endif // _TENSOR_OPERATIONS_HPP_
