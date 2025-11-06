#if !defined(_SYCL_HANDLER_H_)
#define _SYCL_HANDLER_H_
#include <sycl/sycl.hpp>
#include <GL/glx.h>
#include <CL/cl.h>
#include <CL/cl_gl.h>
#include <CL/cl_gl_ext.h>
#include <funlib/Tensor/tensor.hpp>
#include<algorithm>
namespace flib
{
    class sycl_handler {

        static sycl::device _device;
        static sycl::queue _queue;
        static sycl::platform _platform;
        static cl_context _clCtx;
        static sycl::context _syclCtx;
        static sycl::info::device_type device_type_from_string(const std::string& type_str);
    protected:
      
    public:

        
        template<typename T, typename Func>
        friend class ParticleSystem;

        
        friend class tensor_operations;

        static void select_device(std::string device_name);
        static void get_device_info();
        static void sys_info();
        static void get_platform_info();
        static void select_backend_device(const std::string& platform_filter,
                                      const std::string& device_type_filter);
        static void create_gl_interop_context();
        static sycl::queue get_queue();
        static cl_context get_clContext();
        static sycl::context get_sycl_context();
        
    
    };

} // namespace flib

#include <funlib/Tensor/tensor_operations.hpp>
#endif // _SYCL_HANDLER_H_
