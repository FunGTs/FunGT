#if !defined(_GPU_PHYSICS_KERNEL_HPP_)
#define _GPU_PHYSICS_KERNEL_HPP_

#include <GL/glew.h>
#include <funlib/funlib.hpp>
#include <CL/opencl.h>
#include "gpu_device_data.hpp"
#include "gpu_memory_utils.hpp"
namespace gpu {

    class PhysicsKernel {
    private:
        sycl::queue m_queue;

        // Physics state (Structure of Arrays)
        DeviceData m_data;

        // Rendering (SYCL-OpenGL interop)
        unsigned int m_modelMatrixSSBO;

        // Metadata
        int m_numBodies;
        int m_capacity;
        float m_worldSize = 200.f;
        float m_cellSize = 5.0f;

    public:
        PhysicsKernel();
        ~PhysicsKernel();

        // Initialization
        void init(int maxBodies);
        void cleanup();

        // Body management
        int addSphere(float x, float y, float z, float radius, float mass);
        int addBox(float x, float y, float z, float width, float height, float depth, float mass);

        // Physics pipeline (no parameters needed - uses member data!)
        void applyForces(float dt);
        void integrate(float dt);
        void broadPhase();          // TODO: spatial hashing
        void narrowPhase();         // TODO: collision detection
        void solve();               // TODO: impulse solver
        void buildMatrices();       // Compute model matrices for rendering

        // Getters
        int getNumBodies() const { return m_numBodies; }
        unsigned int getModelMatrixSSBO() const { return m_modelMatrixSSBO; }
    };

} // namespace gpu

#endif // _GPU_PHYSICS_KERNEL_HPP_