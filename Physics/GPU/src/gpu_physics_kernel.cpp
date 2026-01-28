#include "../include/gpu_physics_kernel.hpp"
#include <iostream>
#include <cmath>


gpu::PhysicsKernel::PhysicsKernel()
    : m_numBodies(0), m_capacity(0),
    m_data{nullptr, nullptr, nullptr,
           nullptr, nullptr, nullptr,
           nullptr, nullptr, nullptr,
           nullptr, nullptr, nullptr,
           nullptr, nullptr, nullptr,
           nullptr, nullptr, nullptr,
           nullptr, nullptr, nullptr, nullptr},
    m_modelMatrixSSBO(0) {
}

gpu::PhysicsKernel::~PhysicsKernel() {
    cleanup();
}
void gpu::PhysicsKernel::debugVelocity(int bodyId) {
    float velY;
    std::cout<<" debug vel "<<std::endl;
    m_queue.memcpy(&velY, &m_data.y_vel[bodyId], sizeof(float)).wait();
    std::cout << "Body " << bodyId << " velY: " << velY << std::endl;
}
void gpu::PhysicsKernel::init(int maxBodies) {
    m_capacity = maxBodies;
    m_numBodies = 0;

    m_queue = flib::sycl_handler::get_queue();
    if (!checkGPUMemory(m_queue, maxBodies, m_worldSize, m_cellSize)) {
        throw std::runtime_error("Not enough GPU memory for requested body count");
    }
    auto device = m_queue.get_device();
    std::cout << "GPU Physics Kernel initializing on: "
        << device.get_info<sycl::info::device::name>()
        << std::endl;

    std::cout << "Allocating GPU memory for " << maxBodies << " bodies..." << std::endl;
    m_maxManifolds = maxBodies * 4;  // worst case: each body touches 4 others
    m_hashTableSize = m_maxManifolds * 2;  // keep hash table sparse
    
    m_manifolds = sycl::malloc_device<GPUManifold>(m_maxManifolds, m_queue);
    m_numManifolds = sycl::malloc_device<int>(1, m_queue);
    m_pairToManifold = sycl::malloc_device<int>(m_hashTableSize, m_queue);


    //Allocate shape data:
    m_data.shapeType   = sycl::malloc_device<int>(maxBodies, m_queue);      // 0 = sphere, 1 = box
    m_data.bodyMode    = sycl::malloc_device<int>(maxBodies, m_queue);      // 0 = STATIC, 1 = DYNAMIC
    m_data.radius      = sycl::malloc_device<float>(maxBodies, m_queue);       // for spheres
    m_data.halfExtentX = sycl::malloc_device<float>(maxBodies, m_queue);  // for boxes
    m_data.halfExtentY = sycl::malloc_device<float>(maxBodies, m_queue);
    m_data.halfExtentZ = sycl::malloc_device<float>(maxBodies, m_queue);
    //Allocate positions
    m_data.x_pos = sycl::malloc_device<float>(maxBodies, m_queue);
    m_data.y_pos = sycl::malloc_device<float>(maxBodies, m_queue);
    m_data.z_pos = sycl::malloc_device<float>(maxBodies, m_queue);

    // Allocate linear motion
    m_data.x_vel = sycl::malloc_device<float>(maxBodies, m_queue);
    m_data.y_vel = sycl::malloc_device<float>(maxBodies, m_queue);
    m_data.z_vel = sycl::malloc_device<float>(maxBodies, m_queue);
    m_data.x_force = sycl::malloc_device<float>(maxBodies, m_queue);
    m_data.y_force = sycl::malloc_device<float>(maxBodies, m_queue);
    m_data.z_force = sycl::malloc_device<float>(maxBodies, m_queue);

    // Allocate angular motion
    m_data.x_angVel = sycl::malloc_device<float>(maxBodies, m_queue);
    m_data.y_angVel = sycl::malloc_device<float>(maxBodies, m_queue);
    m_data.z_angVel = sycl::malloc_device<float>(maxBodies, m_queue);
    m_data.x_torque = sycl::malloc_device<float>(maxBodies, m_queue);
    m_data.y_torque = sycl::malloc_device<float>(maxBodies, m_queue);
    m_data.z_torque = sycl::malloc_device<float>(maxBodies, m_queue);

    // Allocate orientations
    m_data.orientW = sycl::malloc_device<float>(maxBodies, m_queue);
    m_data.orientX = sycl::malloc_device<float>(maxBodies, m_queue);
    m_data.orientY = sycl::malloc_device<float>(maxBodies, m_queue);
    m_data.orientZ = sycl::malloc_device<float>(maxBodies, m_queue);

    // Allocate mass properties
    m_data.invMass = sycl::malloc_device<float>(maxBodies, m_queue);
    m_data.invInertiaTensor = sycl::malloc_device<float>(maxBodies * 9, m_queue);

    m_data.restitution = sycl::malloc_device<float>(maxBodies, m_queue);
    m_data.friction = sycl::malloc_device<float>(maxBodies, m_queue);
    // Initialize all to zero
    int pairToManifold = -1;
    m_queue.memset(m_numManifolds, 0, sizeof(int)).wait();
    m_queue.fill(m_pairToManifold, pairToManifold, m_hashTableSize).wait(); 

    m_queue.memset(m_data.x_pos, 0, maxBodies * sizeof(float)).wait();
    m_queue.memset(m_data.y_pos, 0, maxBodies * sizeof(float)).wait();
    m_queue.memset(m_data.z_pos, 0, maxBodies * sizeof(float)).wait();
    m_queue.memset(m_data.x_vel, 0, maxBodies * sizeof(float)).wait();
    m_queue.memset(m_data.y_vel, 0, maxBodies * sizeof(float)).wait();
    m_queue.memset(m_data.z_vel, 0, maxBodies * sizeof(float)).wait();
    m_queue.memset(m_data.x_force, 0, maxBodies * sizeof(float)).wait();
    m_queue.memset(m_data.y_force, 0, maxBodies * sizeof(float)).wait();
    m_queue.memset(m_data.z_force, 0, maxBodies * sizeof(float)).wait();
    m_queue.memset(m_data.x_angVel, 0, maxBodies * sizeof(float)).wait();
    m_queue.memset(m_data.y_angVel, 0, maxBodies * sizeof(float)).wait();
    m_queue.memset(m_data.z_angVel, 0, maxBodies * sizeof(float)).wait();
    m_queue.memset(m_data.x_torque, 0, maxBodies * sizeof(float)).wait();
    m_queue.memset(m_data.y_torque, 0, maxBodies * sizeof(float)).wait();
    m_queue.memset(m_data.z_torque, 0, maxBodies * sizeof(float)).wait();

    // Initialize orientations to identity quaternion
    m_queue.fill(m_data.orientW, 1.0f, maxBodies).wait();
    m_queue.memset(m_data.orientX, 0, maxBodies * sizeof(float)).wait();
    m_queue.memset(m_data.orientY, 0, maxBodies * sizeof(float)).wait();
    m_queue.memset(m_data.orientZ, 0, maxBodies * sizeof(float)).wait();

    m_queue.memset(m_data.invMass, 0, maxBodies * sizeof(float)).wait();
    m_queue.memset(m_data.invInertiaTensor, 0, maxBodies * 9 * sizeof(float)).wait();

    // Initialize shape data to zero
    m_queue.memset(m_data.shapeType, 0, maxBodies * sizeof(int)).wait();
    m_queue.memset(m_data.bodyMode, 0, maxBodies * sizeof(int)).wait();
    m_queue.memset(m_data.radius, 0, maxBodies * sizeof(float)).wait();
    m_queue.memset(m_data.halfExtentX, 0, maxBodies * sizeof(float)).wait();
    m_queue.memset(m_data.halfExtentY, 0, maxBodies * sizeof(float)).wait();
    m_queue.memset(m_data.halfExtentZ, 0, maxBodies * sizeof(float)).wait();

    m_queue.memset(m_data.restitution,0,maxBodies*sizeof(float)).wait();
    m_queue.memset(m_data.friction, 0, maxBodies * sizeof(float)).wait();


    // Create OpenGL SSBO for model matrices
    std::cout << "Creating OpenGL SSBO for model matrices..." << std::endl;
    glGenBuffers(1, &m_modelMatrixSSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_modelMatrixSSBO);
    glBufferData(GL_SHADER_STORAGE_BUFFER,
        maxBodies * 16 * sizeof(float),
        nullptr,
        GL_DYNAMIC_DRAW);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        std::cerr << "OpenGL error creating SSBO: " << err << std::endl;
        return;
    }

    std::cout << "GPU Physics Kernel initialization complete!" << std::endl;
}

void gpu::PhysicsKernel::cleanup() {
    // Free GPU memory
    if (m_data.shapeType) sycl::free(m_data.shapeType, m_queue);
    if (m_data.bodyMode) sycl::free(m_data.bodyMode, m_queue);
    if (m_data.radius) sycl::free(m_data.radius, m_queue);
    if (m_data.halfExtentX) sycl::free(m_data.halfExtentX, m_queue);
    if (m_data.halfExtentY) sycl::free(m_data.halfExtentY, m_queue);
    if (m_data.halfExtentZ) sycl::free(m_data.halfExtentZ, m_queue);
    if (m_data.x_pos) sycl::free(m_data.x_pos, m_queue);
    if (m_data.y_pos) sycl::free(m_data.y_pos, m_queue);
    if (m_data.z_pos) sycl::free(m_data.z_pos, m_queue);
    if (m_data.x_vel) sycl::free(m_data.x_vel, m_queue);
    if (m_data.y_vel) sycl::free(m_data.y_vel, m_queue);
    if (m_data.z_vel) sycl::free(m_data.z_vel, m_queue);
    if (m_data.x_force) sycl::free(m_data.x_force, m_queue);
    if (m_data.y_force) sycl::free(m_data.y_force, m_queue);
    if (m_data.z_force) sycl::free(m_data.z_force, m_queue);
    if (m_data.x_angVel) sycl::free(m_data.x_angVel, m_queue);
    if (m_data.y_angVel) sycl::free(m_data.y_angVel, m_queue);
    if (m_data.z_angVel) sycl::free(m_data.z_angVel, m_queue);
    if (m_data.x_torque) sycl::free(m_data.x_torque, m_queue);
    if (m_data.y_torque) sycl::free(m_data.y_torque, m_queue);
    if (m_data.z_torque) sycl::free(m_data.z_torque, m_queue);
    if (m_data.orientW) sycl::free(m_data.orientW, m_queue);
    if (m_data.orientX) sycl::free(m_data.orientX, m_queue);
    if (m_data.orientY) sycl::free(m_data.orientY, m_queue);
    if (m_data.orientZ) sycl::free(m_data.orientZ, m_queue);
    if (m_data.invMass) sycl::free(m_data.invMass, m_queue);
    if (m_data.invInertiaTensor) sycl::free(m_data.invInertiaTensor, m_queue);

    // Delete OpenGL buffer
    if (m_modelMatrixSSBO) {
        glDeleteBuffers(1, &m_modelMatrixSSBO);
        m_modelMatrixSSBO = 0;
    }

    std::cout << "GPU Physics Kernel cleaned up" << std::endl;
}

int gpu::PhysicsKernel::addSphere(float x, float y, float z, float radius, float mass, MODE mode) {
    if (m_numBodies >= m_capacity) {
        std::cerr << "ERROR: Physics kernel is full!" << std::endl;
        return -1;
    }

    int id = m_numBodies++;
    
    // Upload position
    m_queue.memcpy(&m_data.x_pos[id], &x, sizeof(float)).wait();
    m_queue.memcpy(&m_data.y_pos[id], &y, sizeof(float)).wait();
    m_queue.memcpy(&m_data.z_pos[id], &z, sizeof(float)).wait();

    // Initialize velocities/forces to zero
    float zero = 0.0f;
    m_queue.memcpy(&m_data.x_vel[id], &zero, sizeof(float)).wait();
    m_queue.memcpy(&m_data.y_vel[id], &zero, sizeof(float)).wait();
    m_queue.memcpy(&m_data.z_vel[id], &zero, sizeof(float)).wait();
    m_queue.memcpy(&m_data.x_force[id], &zero, sizeof(float)).wait();
    m_queue.memcpy(&m_data.y_force[id], &zero, sizeof(float)).wait();
    m_queue.memcpy(&m_data.z_force[id], &zero, sizeof(float)).wait();
    m_queue.memcpy(&m_data.x_angVel[id], &zero, sizeof(float)).wait();
    m_queue.memcpy(&m_data.y_angVel[id], &zero, sizeof(float)).wait();
    m_queue.memcpy(&m_data.z_angVel[id], &zero, sizeof(float)).wait();
    m_queue.memcpy(&m_data.x_torque[id], &zero, sizeof(float)).wait();
    m_queue.memcpy(&m_data.y_torque[id], &zero, sizeof(float)).wait();
    m_queue.memcpy(&m_data.z_torque[id], &zero, sizeof(float)).wait();

    // Initialize orientation to identity
    float one = 1.0f;
    m_queue.memcpy(&m_data.orientW[id], &one, sizeof(float)).wait();
    m_queue.memcpy(&m_data.orientX[id], &zero, sizeof(float)).wait();
    m_queue.memcpy(&m_data.orientY[id], &zero, sizeof(float)).wait();
    m_queue.memcpy(&m_data.orientZ[id], &zero, sizeof(float)).wait();

    // Set inverse mass
    float invMassVal = (mass > 0.0f) ? (1.0f / mass) : 0.0f;
    m_queue.memcpy(&m_data.invMass[id], &invMassVal, sizeof(float)).wait();

    // Calculate inverse inertia tensor for sphere: I = (2/5) * m * r²
    float invInertia = (mass > 0.0f) ? (2.5f / (mass * radius * radius)) : 0.0f;

    // Sphere inertia is diagonal
    float inertiaTensor[9] = {
        invInertia, 0.0f, 0.0f,
        0.0f, invInertia, 0.0f,
        0.0f, 0.0f, invInertia
    };

    m_queue.memcpy(&m_data.invInertiaTensor[id * 9], inertiaTensor, 9 * sizeof(float)).wait();

    // Set shape type and geometry
    int shapeType = 0;  // sphere
    int bodyModeVal = (mode == MODE::DYNAMIC) ? 1 : 0;
    m_queue.memcpy(&m_data.shapeType[id], &shapeType, sizeof(int)).wait();
    m_queue.memcpy(&m_data.bodyMode[id], &bodyModeVal, sizeof(int)).wait();
    m_queue.memcpy(&m_data.radius[id], &radius, sizeof(float)).wait();

    float restitution = 0.8;
    float friction    = 0.3;
    m_queue.memcpy(&m_data.restitution[id], &restitution,sizeof(float)).wait();
    m_queue.memcpy(&m_data.friction[id], &friction, sizeof(float)).wait();
    // === DEBUG ===
    float readback_invMass;
    m_queue.memcpy(&readback_invMass, &m_data.invMass[id], sizeof(float)).wait();
    std::cout << "addSphere: id=" << id << ", mass=" << mass << ", invMass=" << readback_invMass << std::endl;
    // === END DEBUG ===

    return id;
}

int gpu::PhysicsKernel::addBox(float x, float y, float z, float width, float height, float depth, float mass, MODE mode) {
    if (m_numBodies >= m_capacity) {
        std::cerr << "ERROR: Physics kernel is full!" << std::endl;
        return -1;
    }

    int id = m_numBodies++;

    // Same as sphere but different inertia
    m_queue.memcpy(&m_data.x_pos[id], &x, sizeof(float)).wait();
    m_queue.memcpy(&m_data.y_pos[id], &y, sizeof(float)).wait();
    m_queue.memcpy(&m_data.z_pos[id], &z, sizeof(float)).wait();

    float zero = 0.0f;
    m_queue.memcpy(&m_data.x_vel[id], &zero, sizeof(float)).wait();
    m_queue.memcpy(&m_data.y_vel[id], &zero, sizeof(float)).wait();
    m_queue.memcpy(&m_data.z_vel[id], &zero, sizeof(float)).wait();
    m_queue.memcpy(&m_data.x_force[id], &zero, sizeof(float)).wait();
    m_queue.memcpy(&m_data.y_force[id], &zero, sizeof(float)).wait();
    m_queue.memcpy(&m_data.z_force[id], &zero, sizeof(float)).wait();
    m_queue.memcpy(&m_data.x_angVel[id], &zero, sizeof(float)).wait();
    m_queue.memcpy(&m_data.y_angVel[id], &zero, sizeof(float)).wait();
    m_queue.memcpy(&m_data.z_angVel[id], &zero, sizeof(float)).wait();
    m_queue.memcpy(&m_data.x_torque[id], &zero, sizeof(float)).wait();
    m_queue.memcpy(&m_data.y_torque[id], &zero, sizeof(float)).wait();
    m_queue.memcpy(&m_data.z_torque[id], &zero, sizeof(float)).wait();

    float one = 1.0f;
    m_queue.memcpy(&m_data.orientW[id], &one, sizeof(float)).wait();
    m_queue.memcpy(&m_data.orientX[id], &zero, sizeof(float)).wait();
    m_queue.memcpy(&m_data.orientY[id], &zero, sizeof(float)).wait();
    m_queue.memcpy(&m_data.orientZ[id], &zero, sizeof(float)).wait();

    float invMassVal = (mass > 0.0f) ? (1.0f / mass) : 0.0f;
    m_queue.memcpy(&m_data.invMass[id], &invMassVal, sizeof(float)).wait();

    // Box inertia tensor: I = (1/12) * m * (h² + d², w² + d², w² + h²)
    float invInertia[9] = { 0 };
    if (mass > 0.0f) {
        float w2 = width * width;
        float h2 = height * height;
        float d2 = depth * depth;
        invInertia[0] = 12.0f / (mass * (h2 + d2));  // Ixx
        invInertia[4] = 12.0f / (mass * (w2 + d2));  // Iyy
        invInertia[8] = 12.0f / (mass * (w2 + h2));  // Izz
    }

    m_queue.memcpy(&m_data.invInertiaTensor[id * 9], invInertia, 9 * sizeof(float)).wait();

    // Set shape type and geometry
    int shapeType = 1;  // box
    int bodyModeVal = (mode == MODE::DYNAMIC) ? 1 : 0;  // ADD THIS LINE
    m_queue.memcpy(&m_data.bodyMode[id], &bodyModeVal, sizeof(int)).wait();  // ADD THIS LINE
    m_queue.memcpy(&m_data.shapeType[id], &shapeType, sizeof(int)).wait();
    float halfX = width * 0.5f;
    float halfY = height * 0.5f;
    float halfZ = depth * 0.5f;
    m_queue.memcpy(&m_data.halfExtentX[id], &halfX, sizeof(float)).wait();
    m_queue.memcpy(&m_data.halfExtentY[id], &halfY, sizeof(float)).wait();
    m_queue.memcpy(&m_data.halfExtentZ[id], &halfZ, sizeof(float)).wait();

    float readback_invMass;
    m_queue.memcpy(&readback_invMass, &m_data.invMass[id], sizeof(float)).wait();

    float restitution = 0.5;
    float friction = 0.3;
    m_queue.memcpy(&m_data.restitution[id], &restitution, sizeof(float)).wait();
    m_queue.memcpy(&m_data.friction[id], &friction, sizeof(float)).wait();
    std::cout << "addBox: id=" << id << ", mass=" << mass << ", invMass=" << readback_invMass << std::endl;
    return id;
}

void gpu::PhysicsKernel::applyForces(float dt) {
    if (m_numBodies == 0) return;
   
    std::size_t n = static_cast<std::size_t>(m_numBodies);
    std::size_t xdim = static_cast<std::size_t>(std::ceil(std::sqrt(n)));
    std::size_t ydim = xdim;

    DeviceData data = m_data;
    m_queue.submit([data, n, xdim, ydim](sycl::handler& h) {
        h.parallel_for(sycl::range<2>(ydim, xdim),
            [data, n, xdim](sycl::item<2> item) {

                std::size_t i = item[0] * xdim + item[1];
                if (i >= n) return;
                if (data.invMass[i] == 0.0f) return;

                // Apply gravity
                float mass = 1.0f / data.invMass[i];
                data.y_force[i] = mass * -9.81f;
            });
        });
    m_queue.wait();
}

void gpu::PhysicsKernel::integrate(float dt) {
    if (m_numBodies == 0) return;

    std::size_t n = static_cast<std::size_t>(m_numBodies);
    std::size_t xdim = static_cast<std::size_t>(std::ceil(std::sqrt(n)));
    std::size_t ydim = xdim;

    DeviceData data = m_data;
    m_queue.submit([data, dt, n, xdim, ydim](sycl::handler& h) {
        h.parallel_for(
            sycl::range<2>(ydim, xdim),
            [data, dt, n, xdim](sycl::item<2> item) {

                std::size_t i = item[0] * xdim + item[1];
                if (i >= n) return;

                float im = data.invMass[i];
                if (im == 0.0f) return;

                // LINEAR MOTION
                float accelX = data.x_force[i] * im;
                float accelY = data.y_force[i] * im;
                float accelZ = data.z_force[i] * im;

                data.x_vel[i] += accelX * dt;
                data.y_vel[i] += accelY * dt;
                data.z_vel[i] += accelZ * dt;

                data.x_pos[i] += data.x_vel[i] * dt;
                data.y_pos[i] += data.y_vel[i] * dt;
                data.z_pos[i] += data.z_vel[i] * dt;

                data.x_force[i] = 0.0f;
                data.y_force[i] = 0.0f;
                data.z_force[i] = 0.0f;

                // ANGULAR MOTION
                int tensorIdx = i * 9;
                float I00 = data.invInertiaTensor[tensorIdx + 0];
                float I01 = data.invInertiaTensor[tensorIdx + 1];
                float I02 = data.invInertiaTensor[tensorIdx + 2];
                float I10 = data.invInertiaTensor[tensorIdx + 3];
                float I11 = data.invInertiaTensor[tensorIdx + 4];
                float I12 = data.invInertiaTensor[tensorIdx + 5];
                float I20 = data.invInertiaTensor[tensorIdx + 6];
                float I21 = data.invInertiaTensor[tensorIdx + 7];
                float I22 = data.invInertiaTensor[tensorIdx + 8];

                float tx = data.x_torque[i];
                float ty = data.y_torque[i];
                float tz = data.z_torque[i];

                float angAccelX = I00 * tx + I01 * ty + I02 * tz;
                float angAccelY = I10 * tx + I11 * ty + I12 * tz;
                float angAccelZ = I20 * tx + I21 * ty + I22 * tz;

                float avx = data.x_angVel[i] + angAccelX * dt;
                float avy = data.y_angVel[i] + angAccelY * dt;
                float avz = data.z_angVel[i] + angAccelZ * dt;

                data.x_angVel[i] = avx;
                data.y_angVel[i] = avy;
                data.z_angVel[i] = avz;

                data.x_torque[i] = 0.0f;
                data.y_torque[i] = 0.0f;
                data.z_torque[i] = 0.0f;

                // ORIENTATION UPDATE
                float angSpeed = sycl::sqrt(avx * avx + avy * avy + avz * avz);

                if (angSpeed > 1e-6f) {
                    float invSpeed = 1.0f / angSpeed;
                    float axisX = avx * invSpeed;
                    float axisY = avy * invSpeed;
                    float axisZ = avz * invSpeed;

                    float angle = angSpeed * dt;
                    float halfAngle = angle * 0.5f;
                    float sinHalf = sycl::sin(halfAngle);
                    float cosHalf = sycl::cos(halfAngle);

                    float dw = cosHalf;
                    float dx = axisX * sinHalf;
                    float dy = axisY * sinHalf;
                    float dz = axisZ * sinHalf;

                    float qw = data.orientW[i];
                    float qx = data.orientX[i];
                    float qy = data.orientY[i];
                    float qz = data.orientZ[i];

                    float nw = dw * qw - dx * qx - dy * qy - dz * qz;
                    float nx = dw * qx + dx * qw + dy * qz - dz * qy;
                    float ny = dw * qy - dx * qz + dy * qw + dz * qx;
                    float nz = dw * qz + dx * qy - dy * qx + dz * qw;

                    float len = sycl::sqrt(nw * nw + nx * nx + ny * ny + nz * nz);
                    if (len > 1e-6f) {
                        float invLen = 1.0f / len;
                        data.orientW[i] = nw * invLen;
                        data.orientX[i] = nx * invLen;
                        data.orientY[i] = ny * invLen;
                        data.orientZ[i] = nz * invLen;
                    }
                }
            });
        });

}

void gpu::PhysicsKernel::broadPhase() {
    std::cout << "broadPhase() - TODO" << std::endl;
}

void gpu::PhysicsKernel::narrowPhase() {
    std::cout << "narrowPhase() - TODO" << std::endl;
}

void gpu::PhysicsKernel::solveImpulses(float dt) {
    DeviceData data = m_data;
    GPUManifold* manifolds = m_manifolds;
    int* numManifolds = m_numManifolds;

    constexpr float ERP = 0.2f;
    constexpr int ITERATIONS = 1;

    int manifoldCount;
    m_queue.memcpy(&manifoldCount, numManifolds, sizeof(int)).wait();
    //std::cout << "=== SOLVE IMPULSES ===" << std::endl;
    //std::cout << "manifoldCount: " << manifoldCount << std::endl;

    if (manifoldCount == 0) {
       
       // std::cout << "No manifolds to solve!" << std::endl;
        return;
    }
    // DEBUG: Read body velocities before solve

    
    std::size_t xdim = 32;
    std::size_t ydim = (manifoldCount + xdim - 1) / xdim;
    std::size_t n = static_cast<std::size_t>(manifoldCount);

    for (int iter = 0; iter < ITERATIONS; iter++) {
        m_queue.submit([data, manifolds, n, xdim,ydim, dt, ERP](sycl::handler& h) {
            h.parallel_for(sycl::range<2>(ydim, xdim), [data, manifolds, n, xdim, dt, ERP](sycl::item<2> item) {
                std::size_t i = item[0] * xdim + item[1];
                if (i >= n) return;

                GPUManifold* m = &manifolds[i];
                int bodyA = m->bodyA;
                int bodyB = m->bodyB;

                for (int p = 0; p < m->numPoints; p++) {
                    //data.y_vel[bodyB] = -999.0f;
                    solveContactImpulse(&m->points[p], data, bodyA, bodyB, dt, ERP);
                    //data.y_vel[bodyB] = 4.0f;
                }
            });
        }).wait();

    }

}

void gpu::PhysicsKernel::buildMatrices() {
    if (m_numBodies == 0) return;

    std::size_t n = static_cast<std::size_t>(m_numBodies);
    std::size_t xdim = static_cast<std::size_t>(std::ceil(std::sqrt(n)));
    std::size_t ydim = xdim;

    // Get OpenCL context and queue for interop
    cl_context clcontext = flib::sycl_handler::get_clContext();
    cl_command_queue clqueue = sycl::get_native<sycl::backend::opencl>(m_queue);

    // Create OpenCL buffer from OpenGL SSBO
    cl_mem clbuffer = clCreateFromGLBuffer(clcontext, CL_MEM_WRITE_ONLY,
        m_modelMatrixSSBO, NULL);
    if (clbuffer == NULL) {
        std::cerr << "Failed to create CL buffer from GL SSBO!" << std::endl;
        return;
    }

    // Finish OpenGL operations
    glFinish();

    // Acquire GL object for SYCL use
    cl_event acquire_event;
    clEnqueueAcquireGLObjects(clqueue, 1, &clbuffer, 0, NULL, &acquire_event);
    clWaitForEvents(1, &acquire_event);

    // SYCL kernel scope
    {
        sycl::context syclCtx = flib::sycl_handler::get_sycl_context();
        sycl::buffer<float> matrixBuf =
            sycl::make_buffer<sycl::backend::opencl, float>(clbuffer, syclCtx);

        DeviceData data = m_data;
        m_queue.submit([data, n, xdim, ydim, &matrixBuf](sycl::handler& h) {
            auto matrices = matrixBuf.get_access<sycl::access::mode::write>(h);

            h.parallel_for(sycl::range<2>{ydim, xdim},
                [data, n, xdim, matrices](sycl::item<2> item) {

                    std::size_t i = item[0] * xdim + item[1];
                    if (i >= n) return;

                    int baseIdx = i * 16;

                    // Get position
                    float px = data.x_pos[i];
                    float py = data.y_pos[i];
                    float pz = data.z_pos[i];

                    // Get orientation (quaternion)
                    float qw = data.orientW[i];
                    float qx = data.orientX[i];
                    float qy = data.orientY[i];
                    float qz = data.orientZ[i];

                    // Convert quaternion to rotation matrix
                    float xx = qx * qx;
                    float xy = qx * qy;
                    float xz = qx * qz;
                    float xw = qx * qw;
                    float yy = qy * qy;
                    float yz = qy * qz;
                    float yw = qy * qw;
                    float zz = qz * qz;
                    float zw = qz * qw;

                    // Build 4x4 model matrix (column-major for OpenGL)
                    // Column 0 (rotation + scale)
                    matrices[baseIdx + 0] = 1.0f - 2.0f * (yy + zz);
                    matrices[baseIdx + 1] = 2.0f * (xy + zw);
                    matrices[baseIdx + 2] = 2.0f * (xz - yw);
                    matrices[baseIdx + 3] = 0.0f;

                    // Column 1
                    matrices[baseIdx + 4] = 2.0f * (xy - zw);
                    matrices[baseIdx + 5] = 1.0f - 2.0f * (xx + zz);
                    matrices[baseIdx + 6] = 2.0f * (yz + xw);
                    matrices[baseIdx + 7] = 0.0f;

                    // Column 2
                    matrices[baseIdx + 8] = 2.0f * (xz + yw);
                    matrices[baseIdx + 9] = 2.0f * (yz - xw);
                    matrices[baseIdx + 10] = 1.0f - 2.0f * (xx + yy);
                    matrices[baseIdx + 11] = 0.0f;

                    // Column 3 (translation)
                    matrices[baseIdx + 12] = px;
                    matrices[baseIdx + 13] = py;
                    matrices[baseIdx + 14] = pz;
                    matrices[baseIdx + 15] = 1.0f;
                });
            });

        m_queue.wait();
    }

    clFinish(clqueue);

    // Release GL object back to OpenGL
    cl_event release_event;
    clEnqueueReleaseGLObjects(clqueue, 1, &clbuffer, 0, NULL, &release_event);
    clWaitForEvents(1, &release_event);

    // Memory barrier
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

    // Release OpenCL memory object
    clReleaseMemObject(clbuffer);
}
void gpu::PhysicsKernel::clearManiFolds() {
    m_queue.memset(m_numManifolds, 0, sizeof(int)).wait();
    m_queue.fill(m_pairToManifold, -1, m_hashTableSize).wait();
}
void gpu::PhysicsKernel::debugManifolds() {
    int manifoldCount;
    m_queue.memcpy(&manifoldCount, m_numManifolds, sizeof(int)).wait();

    std::cout << "=== MANIFOLD DEBUG ===" << std::endl;
    std::cout << "Manifold count: " << manifoldCount << std::endl;

    if (manifoldCount == 0) {
        std::cout << "NO COLLISIONS DETECTED!" << std::endl;
        return;
    }

    // Copy manifolds to host
    GPUManifold* hostManifolds = new GPUManifold[manifoldCount];
    m_queue.memcpy(hostManifolds, m_manifolds, manifoldCount * sizeof(GPUManifold)).wait();

    for (int i = 0; i < manifoldCount; i++) {
        GPUManifold& m = hostManifolds[i];
        std::cout << "Manifold " << i << ": bodyA=" << m.bodyA
            << " bodyB=" << m.bodyB
            << " numPoints=" << m.numPoints << std::endl;

        for (int p = 0; p < m.numPoints; p++) {
            GPUContactPoint& cp = m.points[p];
            std::cout << "  Point " << p << ": normal=("
                << cp.normalX << "," << cp.normalY << "," << cp.normalZ
                << ") pen=" << cp.penetration
                << " impulse=" << cp.normalImpulse << std::endl;
        }
    }

    delete[] hostManifolds;
}
void gpu::PhysicsKernel::detectStaticVsDynamic() {
    DeviceData data = m_data;
    GPUManifold* manifolds = m_manifolds;
    int* pairToManifold = m_pairToManifold;
    int* numManifolds = m_numManifolds;
    int hashTableSize = m_hashTableSize;
    int maxManifolds = m_maxManifolds;
    std::size_t n = static_cast<std::size_t>(m_numBodies);

    std::size_t xdim = 32;
    std::size_t ydim = (n + xdim - 1) / xdim;
    // BEFORE kernel - print body info
   //std::cout << "=== detectStaticVsDynamic ===" << std::endl;
   // std::cout << "numBodies: " << m_numBodies << std::endl;

    // Check body modes on CPU
    int* hostBodyMode = new int[m_numBodies];
    int* hostShapeType = new int[m_numBodies];
    m_queue.memcpy(hostBodyMode, m_data.bodyMode, m_numBodies * sizeof(int)).wait();
    m_queue.memcpy(hostShapeType, m_data.shapeType, m_numBodies * sizeof(int)).wait();

    // for (int i = 0; i < m_numBodies; i++) {
    //     std::cout << "Body " << i << ": mode=" << hostBodyMode[i]
    //         << " shape=" << hostShapeType[i] << std::endl;
    // }
    delete[] hostBodyMode;
    delete[] hostShapeType;
    m_queue.submit([data, manifolds, pairToManifold, numManifolds, hashTableSize, maxManifolds, n, xdim,ydim](sycl::handler& h) {
        h.parallel_for(sycl::range<2>(ydim, xdim), [data, manifolds, pairToManifold, numManifolds, hashTableSize, maxManifolds, n, xdim](sycl::item<2> item) {
            std::size_t i = item[0] * xdim + item[1];
            if (i >= n) return;

            if (data.bodyMode[i] == 0) return;

            for (std::size_t j = 0; j < n; j++) {
                if (data.bodyMode[j] == 1) continue;

                if (data.shapeType[i] == 0 && data.shapeType[j] == 1) {
                    float nx, ny, nz, pen;
                    float wAx, wAy, wAz, wBx, wBy, wBz;
                    float lAx, lAy, lAz, lBx, lBy, lBz;

                    bool hit = SphereBoxCollision(
                        data.x_pos[i], data.y_pos[i], data.z_pos[i],
                        data.radius[i],
                        data.x_pos[j], data.y_pos[j], data.z_pos[j],
                        data.halfExtentX[j], data.halfExtentY[j], data.halfExtentZ[j],
                        true,
                        nx, ny, nz, pen,
                        wAx, wAy, wAz, wBx, wBy, wBz,
                        lAx, lAy, lAz, lBx, lBy, lBz
                    );

                    if (hit) {
                        int manifoldIdx = findManifold(pairToManifold, manifolds, hashTableSize, j, i);
                        if (manifoldIdx == -1) {
                            manifoldIdx = createManifold(pairToManifold, manifolds, numManifolds, hashTableSize, maxManifolds, j, i);
                        }
                        if (manifoldIdx == -1) {
                            data.y_vel[i] = -7777.0f;  // marker
                        }
                        else {
                            addContactToManifold(manifolds, manifoldIdx,
                                lAx, lAy, lAz,
                                lBx, lBy, lBz,
                                wAx, wAy, wAz,
                                wBx, wBy, wBz,
                                nx, ny, nz, pen);
                        }
                    }
                }
            }
        });
    }).wait();
}