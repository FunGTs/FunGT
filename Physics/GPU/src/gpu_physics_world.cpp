#include "../include/gpu_physics_world.hpp"

// Constructor
gpu::PhysicsWorld::PhysicsWorld() {
    // Create kernel
    m_kernel = std::make_shared<gpu::PhysicsKernel>();
    m_kernel->init(100);

    // Create collision manager (shares kernel ownership)
    m_collisionManager = std::make_shared<gpu::CollisionManager>(m_kernel);
}
