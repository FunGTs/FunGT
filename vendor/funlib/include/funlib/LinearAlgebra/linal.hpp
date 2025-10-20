#if !defined(MACRO)
#define MACRO
#include <funlib/Tensor/tensor.hpp>
#include <funlib/Tensor/tensor_operations.hpp>
#include <cmath>
namespace flib
{
    namespace linal
    {

        template<typename T>
        void conjugate_grad(const Tensor<T>& A, const Tensor<T> & b, Tensor<T> & x, int max_iter = 1000, T tol = 1e-10);
        

        
    } // namespace linal
    
} // namespace flib


#endif // MACRO
