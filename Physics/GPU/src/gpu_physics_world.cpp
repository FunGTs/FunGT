#include "../include/gpu_physics_world.hpp"

// Constructor - nothing needed, PhysicsKernel handles it
gpu::PhysicsWorld::PhysicsWorld() {
}

gpu::PhysicsWorld::~PhysicsWorld() {
}

void gpu::PhysicsWorld::init(int maxBodies) {
    m_kernel.init(maxBodies);
}

int gpu::PhysicsWorld::addSphere(float x, float y, float z, float radius, float mass) {
    return m_kernel.addSphere(x, y, z, radius, mass);
}

int gpu::PhysicsWorld::addBox(float x, float y, float z, float width, float height, float depth, float mass) {
    return m_kernel.addBox(x, y, z, width, height, depth, mass);
}

void gpu::PhysicsWorld::update(float dt) {
    m_kernel.applyForces(dt);
    m_kernel.integrate(dt);
    m_kernel.broadPhase();
    m_kernel.narrowPhase();
    m_kernel.solve();
    m_kernel.buildMatrices();
}

void gpu::PhysicsWorld::runColliders() {
    // Can call specific collision methods if needed
    m_kernel.broadPhase();
    m_kernel.narrowPhase();
}

int gpu::PhysicsWorld::getNumBodies() const {
    return m_kernel.getNumBodies();
}

unsigned int gpu::PhysicsWorld::getModelMatrixSSBO() const {
    return m_kernel.getModelMatrixSSBO();
}