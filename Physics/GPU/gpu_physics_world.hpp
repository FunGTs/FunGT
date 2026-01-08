#if !defined(_GPU_PHYSICS_WORLD)
#define _GPU_PHYSICS_WORLD
#include<funlib/funlib.hpp>

namespace gpu
{
    class PhysicsWorld{

        private:
            sycl::queue m_queue;
            float *x_pos, *y_pos, *z_pos; //position pointers:
            float *x_vel, *y_vel, *z_vel;
            float* orientW_gpu, * orientX_gpu, * orientY_gpu, * orientZ_gpu;

            // Shared SYCL-OpenGL buffer
            unsigned int m_modelMatrixSSBO;
            cl_mem m_modelMatrices_clMem;
            float* m_modelMatrices_gpu;

            int m_numBodies;

    };
    
} // namespace gpu



#endif // _GPU_PHYSICS_WORLD
