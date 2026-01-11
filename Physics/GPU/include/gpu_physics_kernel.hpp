#if !defined(_GPU_PHYSICS_KERNEL_HPP_)
#define _GPU_PHYSICS_KERNEL_HPP_

#include <funlib/funlib.hpp>
#include <GL/glew.h>
#include <CL/opencl.h>

namespace gpu {

    class PhysicsKernel {
    private:
        sycl::queue m_queue;

        // Physics state (Structure of Arrays)
        float* x_pos, * y_pos, * z_pos;
        float* x_vel, * y_vel, * z_vel;
        float* x_force, * y_force, * z_force;
        float* x_angVel, * y_angVel, * z_angVel;
        float* x_torque, * y_torque, * z_torque;
        float* orientW_gpu, * orientX_gpu, * orientY_gpu, * orientZ_gpu;
        float* invMass_gpu;
        float* invInertiaTensor_gpu;  // 9 floats per body (3x3 matrix)

        // Rendering (SYCL-OpenGL interop)
        unsigned int m_modelMatrixSSBO;

        // Metadata
        int m_numBodies;
        int m_capacity;

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