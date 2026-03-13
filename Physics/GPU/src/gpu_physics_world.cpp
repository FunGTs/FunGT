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

    try {
        //std::cout << "Calling applyForces..." << std::endl;
        m_kernel->applyForces(dt);
        m_kernel->clearManiFolds();
        //std::cout << "applyForces OK" << std::endl;
    }
    catch (const sycl::exception& e) {
        std::cerr << "ERROR in applyForces: " << e.what() << std::endl;
        throw;
    }
    // ========== NEW: UPDATE UNIFORM GRID ==========
    try {
        // Step 1: Compute which cell each body is in
        m_kernel->computeCellHashes();

        // Step 2: Sort bodies by cell (groups nearby bodies together)
        m_kernel->sortBodiesByCell();

        // Step 3: Find where each cell starts/ends in sorted array
        m_kernel->findCellBoundaries();

        // DEBUG: Print grid state (first frame only)
        static bool first_frame = true;
        // if (first_frame) {
        //     m_kernel->debugGrid();
        //     first_frame = false;
        // }
    }
    catch (const sycl::exception& e) {
        std::cerr << "ERROR in grid update: " << e.what() << std::endl;
        throw;
    }
    try {
        //std::cout << "Calling applyForces..." << std::endl;
        m_kernel->detectStaticVsDynamic();
        m_kernel->detectDynamicVsDynamic();
        //m_kernel->debugManifolds();
        //std::cout << "applyForces OK" << std::endl;
    }
    catch (const sycl::exception& e) {
        std::cerr << "ERROR in applyForces: " << e.what() << std::endl;
        throw;
    }
    try {
        //std::cout << "Calling applyForces..." << std::endl;
        m_kernel->solveImpulses(dt);
        //m_kernel->debugVelocity(1);
        //std::cout << "applyForces OK" << std::endl;
    }
    catch (const sycl::exception& e) {
        std::cerr << "ERROR in applyForces: " << e.what() << std::endl;
        throw;
    }
    try {
        //std::cout << "Calling integrate..." << std::endl;
        m_kernel->integrate(dt);
        //m_kernel->debugVelocity(1);
        //std::cout << "integrate OK" << std::endl;
    }
    catch (const sycl::exception& e) {
        std::cerr << "ERROR in integrate: " << e.what() << std::endl;
        throw;
    }

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
