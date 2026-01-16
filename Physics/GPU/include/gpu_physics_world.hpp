#if !defined(_GPU_PHYSICS_WORLD_HPP_)
#define _GPU_PHYSICS_WORLD_HPP_

#include "Physics/GPU/include/gpu_physics_kernel.hpp"
#include "Physics/GPU/include/gpu_collision_manager.hpp"
#include <memory>

namespace gpu {

    class PhysicsWorld {
    private:
        std::shared_ptr<gpu::CollisionManager> m_collisionManager;
        std::unique_ptr<gpu::PhysicsKernel> m_kernel;
        

    public:
        PhysicsWorld() {
            // Create kernel
            m_kernel = std::make_unique<gpu::PhysicsKernel>();
            m_kernel->init(10000);

            // Create collision manager (shares kernel)
            m_collisionManager = std::make_shared<gpu::CollisionManager>(m_kernel.get());
        }

        ~PhysicsWorld() = default;

        // Get collision manager (shared_ptr!)
        std::shared_ptr<gpu::CollisionManager> getCollisionManager() {
            return m_collisionManager;
        }

        // Update physics
        void update(float dt) {
            m_kernel->applyForces(dt);
            m_kernel->integrate(dt);
            // m_kernel->broadPhase();
            // m_kernel->narrowPhase();
            // m_kernel->solve();
            m_kernel->buildMatrices();
        }

        int getNumBodies() const {
            return m_kernel->getNumBodies();
        }
    };

} // namespace gpu

#endif