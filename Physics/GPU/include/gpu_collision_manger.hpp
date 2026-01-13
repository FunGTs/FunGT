#if !defined(_GPU_COLLISION_MANAGER_HPP_)
#define _GPU_COLLISION_MANAGER_HPP_

#include "gpu_physics_kernel.hpp"

namespace gpu {

    class CollisionManager {
    private:
        PhysicsKernel* m_kernel;  // Reference to kernel

    public:
        CollisionManager(PhysicsKernel* kernel) : m_kernel(kernel) {}

        // Add bodies (returns body ID)
        int addSphere(float x, float y, float z, float radius, float mass) {
            return m_kernel->addSphere(x, y, z, radius, mass);
        }

        int addBox(float x, float y, float z, float width, float height, float depth, float mass) {
            return m_kernel->addBox(x, y, z, width, height, depth, mass);
        }

        // Query
        int getNumBodies() const {
            return m_kernel->getNumBodies();
        }
    };

} // namespace gpu

#endif