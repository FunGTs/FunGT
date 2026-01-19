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
        void update(float dt) {
        

            try {
                //std::cout << "Calling applyForces..." << std::endl;
                m_kernel->applyForces(dt);
               
                //std::cout << "applyForces OK" << std::endl;
            }
            catch (const sycl::exception& e) {
                std::cerr << "ERROR in applyForces: " << e.what() << std::endl;
                throw;
            }

            try {
                //std::cout << "Calling integrate..." << std::endl;
                m_kernel->integrate(dt);
                
                //std::cout << "integrate OK" << std::endl;
            }
            catch (const sycl::exception& e) {
                std::cerr << "ERROR in integrate: " << e.what() << std::endl;
                throw;
            }

            // Comment out these for now
            // m_kernel->broadPhase();
            // m_kernel->narrowPhase();
            // m_kernel->solve();

            try {
                //std::cout << "Calling buildMatrices..." << std::endl;
                m_kernel->buildMatrices();
                //std::cout << "buildMatrices OK" << std::endl;
            }
            catch (const sycl::exception& e) {
                std::cerr << "ERROR in buildMatrices: " << e.what() << std::endl;
                throw;
            }
        }

        int getNumBodies() const {
            return m_kernel->getNumBodies();
        }
    };

} // namespace gpu

#endif