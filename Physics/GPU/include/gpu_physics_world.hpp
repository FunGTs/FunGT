#if !defined(_GPU_PHYSICS_WORLD)
#define _GPU_PHYSICS_WORLD

#include <funlib/funlib.hpp>
#include "gpu_integration_kernel.hpp"
namespace gpu {
    class PhysicsWorld {
    private:
        sycl::queue m_queue;
        float* x_pos, * y_pos, * z_pos;
        float* x_vel, * y_vel, * z_vel;
        float* orientW_gpu, * orientX_gpu, * orientY_gpu, * orientZ_gpu;
        // Mass properties
        float* invMass_gpu;
        // Shared SYCL-OpenGL buffer
        unsigned int m_modelMatrixSSBO;
        cl_mem m_modelMatrices_clMem;
        float* m_modelMatrices_gpu;

        int m_numBodies;
        int m_capacity;  // ADD THIS - max bodies

    public:
        PhysicsWorld();
        ~PhysicsWorld();  // ADD destructor (to free GPU memory)

        void init(int maxBodies);
        void update(float dt);
        void runColliders();

        // ADD these helper methods
        int getNumBodies() const { return m_numBodies; }
        unsigned int getModelMatrixSSBO() const { return m_modelMatrixSSBO; }
    };
} // namespace gpu

#endif // _GPU_PHYSICS_WORLD