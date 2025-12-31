#if !defined(_PARTICLE_SYSTEM_H)
#define _PARTICLE_SYSTEM_H
#include <funlib/sycl/sycl_handler.hpp>
#include <vector>
#include <functional>
#include <cmath>
#include <funlib/ParticleSet/particle_set.hpp>
#include <CL/cl_gl.h>
namespace flib{
   
    template <typename T, typename Func>
    class ParticleSystem
    {
        class sycl_handler; // Forward declaration of sycl_handler
    public:
        void static update(int numOfParticles, unsigned int vbo, Func update_function, T dt){
            std::size_t n = static_cast<std::size_t>(numOfParticles);
            std::size_t xdim = static_cast<std::size_t>(std::ceil(std::sqrt(n)));
            std::size_t ydim = xdim; 
            // Get the SYCL queue from the sycl_handler   
            sycl::queue Q = flib::sycl_handler::get_queue();
            //OpeCL interop for updating particles
            cl_context clcontext = flib::sycl_handler::get_clContext();
                
            cl_command_queue clqueue = sycl::get_native<sycl::backend::opencl>(Q);

            cl_mem clbuffer = clCreateFromGLBuffer(clcontext, CL_MEM_READ_WRITE, vbo, NULL);
            if (clbuffer == NULL) {
                throw std::runtime_error("Failed to create OpenCL buffer from GL buffer");
            }
            sycl::context syclCtx = flib::sycl_handler::get_sycl_context();
            // CRITICAL: Finish all pending OpenGL operations first
            glFinish();
            cl_event acquire_event;
            clEnqueueAcquireGLObjects(clqueue, 1, &clbuffer, 0, NULL, &acquire_event);
            clWaitForEvents(1, &acquire_event);  // Wait for acquisition to complete   
            {
                sycl::buffer<Particle<T>> buf =
                sycl::make_buffer<sycl::backend::opencl, Particle<T>>(clbuffer, syclCtx);

                Q.submit([&](sycl::handler &cgh) {
                    auto acc = buf.template get_access<sycl::access::mode::read_write>(cgh);
                    cgh.parallel_for(sycl::range<2>(sycl::range<2>{static_cast<size_t>(xdim),static_cast<size_t>(ydim)}),
                        [=](sycl::item<2> item){
                             //user defined lambda function
                            std::size_t index = item[0] * ydim + item[1];
                            if (index < n) {
                                update_function(acc[index], dt);
                            }
                    });

                });

                 Q.wait();
            }
             clFinish(clqueue);
            //Release OpenCL buffer
            cl_event release_event;
            clEnqueueReleaseGLObjects(clqueue, 1, &clbuffer, 0, NULL, &release_event);
            clWaitForEvents(1, &release_event);  // Wait for release to complete

            glMemoryBarrier(GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT | GL_BUFFER_UPDATE_BARRIER_BIT);
            clReleaseMemObject(clbuffer);
        }
    
    };
    
}

#endif // _PARTICLE_SYSTEM_H
