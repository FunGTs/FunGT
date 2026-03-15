#include "../include/gpu_physics_world.hpp"
#include "gpu_physics_world.hpp"

// Constructor
gpu::PhysicsWorld::PhysicsWorld() {
    // Create kernel
    m_kernel = std::make_shared<gpu::PhysicsKernel>();
    m_kernel->init(1000);
    // Create collision manager (shares kernel ownership)
    m_collisionManager = std::make_shared<gpu::CollisionManager>(m_kernel);
}

void gpu::PhysicsWorld::update(float dt)
{

    constexpr int SUBSTEPS = 4;
    float subDt = dt / SUBSTEPS;

  

    for (int s = 0; s < SUBSTEPS; s++) {
        m_kernel->warmStart();
        m_kernel->clearManiFolds();
        m_kernel->applyForces(subDt);
        m_kernel->computeCellHashes();
        m_kernel->sortBodiesByCell();
        m_kernel->findCellBoundaries();
        m_kernel->detectStaticVsDynamic();
        m_kernel->detectDynamicVsDynamic();
        m_kernel->solveImpulsesB(subDt);
        m_kernel->integrate(subDt);
    }

    m_kernel->buildMatrices();
}
