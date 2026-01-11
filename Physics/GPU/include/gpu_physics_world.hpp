#if !defined(_GPU_PHYSICS_WORLD_HPP_)
#define _GPU_PHYSICS_WORLD_HPP_

#include "gpu_physics_kernel.hpp"

namespace gpu {

    class PhysicsWorld {
    private:
        PhysicsKernel m_kernel;  // Does all the actual work

    public:
        PhysicsWorld();
        ~PhysicsWorld();

        // Initialization
        void init(int maxBodies);

        // Body management
        int addSphere(float x, float y, float z, float radius, float mass);
        int addBox(float x, float y, float z, float width, float height, float depth, float mass);

        // Physics update
        void update(float dt);
        void runColliders();

        // Getters
        int getNumBodies() const;
        unsigned int getModelMatrixSSBO() const;
    };

} // namespace gpu

#endif // _GPU_PHYSICS_WORLD_HPP_