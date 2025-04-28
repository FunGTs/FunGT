#if !defined(_SYCL_HANDLER_H_)
#define _SYCL_HANDLER_H_
#include <sycl/sycl.hpp>
#include "matrix.hpp"
#include "vector.hpp"
namespace flib
{
    class sycl_handler {

        static sycl::device _device;
        static sycl::queue _queue;
    protected:
        static sycl::queue get_queue();
       
    public:

        
        template<typename T, typename Func>
        friend class ParticleSystem;

        static void select_device(std::string device_name);
        static void get_device_info();
        static void sys_info();
        template<typename T>
        friend Matrix<T> gemm(const Matrix<T> &A, const Matrix<T> &B);
        template <typename T>
        friend Vector<T> prod(const Matrix<T>& A, const Vector<T>& v);
   
        
    
    
    };

} // namespace flib


#include "matrix_operations.hpp"
#include "ParticleSystem.hpp"
#endif // _SYCL_HANDLER_H_

