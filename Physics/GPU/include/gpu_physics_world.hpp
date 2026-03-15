#if !defined(_GPU_PHYSICS_WORLD_HPP_)
#define _GPU_PHYSICS_WORLD_HPP_

#include "gpu_physics_kernel.hpp"
#include "gpu_collision_manger.hpp"
#include <memory>

namespace gpu {

    class PhysicsWorld {
    private:
        std::shared_ptr<gpu::PhysicsKernel> m_kernel;
        std::shared_ptr<gpu::CollisionManager> m_collisionManager;
        

    public:
        PhysicsWorld();

        ~PhysicsWorld() = default;

        // Get collision manager (shared_ptr!)
        std::shared_ptr<gpu::CollisionManager> getCollisionManager() {
            return m_collisionManager;
        }

        // Update physics
        void update(float dt);

        int getNumBodies() const {
            return m_kernel->getNumBodies();
        }
    };

} // namespace gpu

#endif