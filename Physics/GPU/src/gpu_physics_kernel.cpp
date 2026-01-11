#include "../include/gpu_physics_kernel.hpp"
#include <iostream>
#include <cmath>

gpu::PhysicsKernel::PhysicsKernel()
    : m_numBodies(0), m_capacity(0),
    x_pos(nullptr), y_pos(nullptr), z_pos(nullptr),
    x_vel(nullptr), y_vel(nullptr), z_vel(nullptr),
    x_force(nullptr), y_force(nullptr), z_force(nullptr),
    x_angVel(nullptr), y_angVel(nullptr), z_angVel(nullptr),
    x_torque(nullptr), y_torque(nullptr), z_torque(nullptr),
    orientW_gpu(nullptr), orientX_gpu(nullptr), orientY_gpu(nullptr), orientZ_gpu(nullptr),
    invMass_gpu(nullptr), invInertiaTensor_gpu(nullptr),
    m_modelMatrixSSBO(0) {
}

gpu::PhysicsKernel::~PhysicsKernel() {
    cleanup();
}

void gpu::PhysicsKernel::init(int maxBodies) {
    m_capacity = maxBodies;
    m_numBodies = 0;

    m_queue = flib::sycl_handler::get_queue();

    auto device = m_queue.get_device();
    std::cout << "GPU Physics Kernel initializing on: "
        << device.get_info<sycl::info::device::name>()
        << std::endl;

    std::cout << "Allocating GPU memory for " << maxBodies << " bodies..." << std::endl;

    // Allocate positions
    x_pos = sycl::malloc_device<float>(maxBodies, m_queue);
    y_pos = sycl::malloc_device<float>(maxBodies, m_queue);
    z_pos = sycl::malloc_device<float>(maxBodies, m_queue);

    // Allocate linear motion
    x_vel = sycl::malloc_device<float>(maxBodies, m_queue);
    y_vel = sycl::malloc_device<float>(maxBodies, m_queue);
    z_vel = sycl::malloc_device<float>(maxBodies, m_queue);
    x_force = sycl::malloc_device<float>(maxBodies, m_queue);
    y_force = sycl::malloc_device<float>(maxBodies, m_queue);
    z_force = sycl::malloc_device<float>(maxBodies, m_queue);

    // Allocate angular motion
    x_angVel = sycl::malloc_device<float>(maxBodies, m_queue);
    y_angVel = sycl::malloc_device<float>(maxBodies, m_queue);
    z_angVel = sycl::malloc_device<float>(maxBodies, m_queue);
    x_torque = sycl::malloc_device<float>(maxBodies, m_queue);
    y_torque = sycl::malloc_device<float>(maxBodies, m_queue);
    z_torque = sycl::malloc_device<float>(maxBodies, m_queue);

    // Allocate orientations
    orientW_gpu = sycl::malloc_device<float>(maxBodies, m_queue);
    orientX_gpu = sycl::malloc_device<float>(maxBodies, m_queue);
    orientY_gpu = sycl::malloc_device<float>(maxBodies, m_queue);
    orientZ_gpu = sycl::malloc_device<float>(maxBodies, m_queue);

    // Allocate mass properties
    invMass_gpu = sycl::malloc_device<float>(maxBodies, m_queue);
    invInertiaTensor_gpu = sycl::malloc_device<float>(maxBodies * 9, m_queue);

    // Initialize all to zero
    m_queue.memset(x_pos, 0, maxBodies * sizeof(float)).wait();
    m_queue.memset(y_pos, 0, maxBodies * sizeof(float)).wait();
    m_queue.memset(z_pos, 0, maxBodies * sizeof(float)).wait();
    m_queue.memset(x_vel, 0, maxBodies * sizeof(float)).wait();
    m_queue.memset(y_vel, 0, maxBodies * sizeof(float)).wait();
    m_queue.memset(z_vel, 0, maxBodies * sizeof(float)).wait();
    m_queue.memset(x_force, 0, maxBodies * sizeof(float)).wait();
    m_queue.memset(y_force, 0, maxBodies * sizeof(float)).wait();
    m_queue.memset(z_force, 0, maxBodies * sizeof(float)).wait();
    m_queue.memset(x_angVel, 0, maxBodies * sizeof(float)).wait();
    m_queue.memset(y_angVel, 0, maxBodies * sizeof(float)).wait();
    m_queue.memset(z_angVel, 0, maxBodies * sizeof(float)).wait();
    m_queue.memset(x_torque, 0, maxBodies * sizeof(float)).wait();
    m_queue.memset(y_torque, 0, maxBodies * sizeof(float)).wait();
    m_queue.memset(z_torque, 0, maxBodies * sizeof(float)).wait();

    // Initialize orientations to identity quaternion
    m_queue.fill(orientW_gpu, 1.0f, maxBodies).wait();
    m_queue.memset(orientX_gpu, 0, maxBodies * sizeof(float)).wait();
    m_queue.memset(orientY_gpu, 0, maxBodies * sizeof(float)).wait();
    m_queue.memset(orientZ_gpu, 0, maxBodies * sizeof(float)).wait();

    m_queue.memset(invMass_gpu, 0, maxBodies * sizeof(float)).wait();
    m_queue.memset(invInertiaTensor_gpu, 0, maxBodies * 9 * sizeof(float)).wait();

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
    if (x_pos) sycl::free(x_pos, m_queue);
    if (y_pos) sycl::free(y_pos, m_queue);
    if (z_pos) sycl::free(z_pos, m_queue);
    if (x_vel) sycl::free(x_vel, m_queue);
    if (y_vel) sycl::free(y_vel, m_queue);
    if (z_vel) sycl::free(z_vel, m_queue);
    if (x_force) sycl::free(x_force, m_queue);
    if (y_force) sycl::free(y_force, m_queue);
    if (z_force) sycl::free(z_force, m_queue);
    if (x_angVel) sycl::free(x_angVel, m_queue);
    if (y_angVel) sycl::free(y_angVel, m_queue);
    if (z_angVel) sycl::free(z_angVel, m_queue);
    if (x_torque) sycl::free(x_torque, m_queue);
    if (y_torque) sycl::free(y_torque, m_queue);
    if (z_torque) sycl::free(z_torque, m_queue);
    if (orientW_gpu) sycl::free(orientW_gpu, m_queue);
    if (orientX_gpu) sycl::free(orientX_gpu, m_queue);
    if (orientY_gpu) sycl::free(orientY_gpu, m_queue);
    if (orientZ_gpu) sycl::free(orientZ_gpu, m_queue);
    if (invMass_gpu) sycl::free(invMass_gpu, m_queue);
    if (invInertiaTensor_gpu) sycl::free(invInertiaTensor_gpu, m_queue);

    // Delete OpenGL buffer
    if (m_modelMatrixSSBO) {
        glDeleteBuffers(1, &m_modelMatrixSSBO);
        m_modelMatrixSSBO = 0;
    }

    std::cout << "GPU Physics Kernel cleaned up" << std::endl;
}

int gpu::PhysicsKernel::addSphere(float x, float y, float z, float radius, float mass) {
    if (m_numBodies >= m_capacity) {
        std::cerr << "ERROR: Physics kernel is full!" << std::endl;
        return -1;
    }

    int id = m_numBodies++;

    // Upload position
    m_queue.memcpy(&x_pos[id], &x, sizeof(float)).wait();
    m_queue.memcpy(&y_pos[id], &y, sizeof(float)).wait();
    m_queue.memcpy(&z_pos[id], &z, sizeof(float)).wait();

    // Initialize velocities/forces to zero
    float zero = 0.0f;
    m_queue.memcpy(&x_vel[id], &zero, sizeof(float)).wait();
    m_queue.memcpy(&y_vel[id], &zero, sizeof(float)).wait();
    m_queue.memcpy(&z_vel[id], &zero, sizeof(float)).wait();
    m_queue.memcpy(&x_force[id], &zero, sizeof(float)).wait();
    m_queue.memcpy(&y_force[id], &zero, sizeof(float)).wait();
    m_queue.memcpy(&z_force[id], &zero, sizeof(float)).wait();
    m_queue.memcpy(&x_angVel[id], &zero, sizeof(float)).wait();
    m_queue.memcpy(&y_angVel[id], &zero, sizeof(float)).wait();
    m_queue.memcpy(&z_angVel[id], &zero, sizeof(float)).wait();
    m_queue.memcpy(&x_torque[id], &zero, sizeof(float)).wait();
    m_queue.memcpy(&y_torque[id], &zero, sizeof(float)).wait();
    m_queue.memcpy(&z_torque[id], &zero, sizeof(float)).wait();

    // Initialize orientation to identity
    float one = 1.0f;
    m_queue.memcpy(&orientW_gpu[id], &one, sizeof(float)).wait();
    m_queue.memcpy(&orientX_gpu[id], &zero, sizeof(float)).wait();
    m_queue.memcpy(&orientY_gpu[id], &zero, sizeof(float)).wait();
    m_queue.memcpy(&orientZ_gpu[id], &zero, sizeof(float)).wait();

    // Set inverse mass
    float invMass = (mass > 0.0f) ? (1.0f / mass) : 0.0f;
    m_queue.memcpy(&invMass_gpu[id], &invMass, sizeof(float)).wait();

    // Calculate inverse inertia tensor for sphere: I = (2/5) * m * r²
    float invInertia = (mass > 0.0f) ? (2.5f / (mass * radius * radius)) : 0.0f;

    // Sphere inertia is diagonal
    float inertiaTensor[9] = {
        invInertia, 0.0f, 0.0f,
        0.0f, invInertia, 0.0f,
        0.0f, 0.0f, invInertia
    };

    m_queue.memcpy(&invInertiaTensor_gpu[id * 9], inertiaTensor, 9 * sizeof(float)).wait();

    return id;
}

int gpu::PhysicsKernel::addBox(float x, float y, float z, float width, float height, float depth, float mass) {
    if (m_numBodies >= m_capacity) {
        std::cerr << "ERROR: Physics kernel is full!" << std::endl;
        return -1;
    }

    int id = m_numBodies++;

    // Same as sphere but different inertia
    m_queue.memcpy(&x_pos[id], &x, sizeof(float)).wait();
    m_queue.memcpy(&y_pos[id], &y, sizeof(float)).wait();
    m_queue.memcpy(&z_pos[id], &z, sizeof(float)).wait();

    float zero = 0.0f;
    m_queue.memcpy(&x_vel[id], &zero, sizeof(float)).wait();
    m_queue.memcpy(&y_vel[id], &zero, sizeof(float)).wait();
    m_queue.memcpy(&z_vel[id], &zero, sizeof(float)).wait();
    m_queue.memcpy(&x_force[id], &zero, sizeof(float)).wait();
    m_queue.memcpy(&y_force[id], &zero, sizeof(float)).wait();
    m_queue.memcpy(&z_force[id], &zero, sizeof(float)).wait();
    m_queue.memcpy(&x_angVel[id], &zero, sizeof(float)).wait();
    m_queue.memcpy(&y_angVel[id], &zero, sizeof(float)).wait();
    m_queue.memcpy(&z_angVel[id], &zero, sizeof(float)).wait();
    m_queue.memcpy(&x_torque[id], &zero, sizeof(float)).wait();
    m_queue.memcpy(&y_torque[id], &zero, sizeof(float)).wait();
    m_queue.memcpy(&z_torque[id], &zero, sizeof(float)).wait();

    float one = 1.0f;
    m_queue.memcpy(&orientW_gpu[id], &one, sizeof(float)).wait();
    m_queue.memcpy(&orientX_gpu[id], &zero, sizeof(float)).wait();
    m_queue.memcpy(&orientY_gpu[id], &zero, sizeof(float)).wait();
    m_queue.memcpy(&orientZ_gpu[id], &zero, sizeof(float)).wait();

    float invMass = (mass > 0.0f) ? (1.0f / mass) : 0.0f;
    m_queue.memcpy(&invMass_gpu[id], &invMass, sizeof(float)).wait();

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

    m_queue.memcpy(&invInertiaTensor_gpu[id * 9], invInertia, 9 * sizeof(float)).wait();

    return id;
}

void gpu::PhysicsKernel::applyForces(float dt) {
    if (m_numBodies == 0) return;

    std::size_t n = static_cast<std::size_t>(m_numBodies);
    std::size_t xdim = static_cast<std::size_t>(std::ceil(std::sqrt(n)));
    std::size_t ydim = xdim;

    m_queue.submit([this, n, xdim, ydim](sycl::handler& h) {
        h.parallel_for(sycl::range<2>(ydim, xdim),
            [this, n, xdim](sycl::item<2> item) {

                std::size_t i = item[0] * xdim + item[1];
                if (i >= n) return;
                if (invMass_gpu[i] == 0.0f) return;

                // Apply gravity
                float mass = 1.0f / invMass_gpu[i];
                y_force[i] = mass * -9.81f;
            });
        });
}

void gpu::PhysicsKernel::integrate(float dt) {
    if (m_numBodies == 0) return;

    std::size_t n = static_cast<std::size_t>(m_numBodies);
    std::size_t xdim = static_cast<std::size_t>(std::ceil(std::sqrt(n)));
    std::size_t ydim = xdim;

    m_queue.submit([this, dt, n, xdim, ydim](sycl::handler& h) {
        h.parallel_for(
            sycl::range<2>(ydim, xdim),
            [this, dt, n, xdim](sycl::item<2> item) {

                std::size_t i = item[0] * xdim + item[1];
                if (i >= n) return;

                // Use member variables DIRECTLY!
                float im = invMass_gpu[i];
                if (im == 0.0f) return;

                // LINEAR MOTION
                float accelX = x_force[i] * im;
                float accelY = y_force[i] * im;
                float accelZ = z_force[i] * im;

                x_vel[i] += accelX * dt;
                y_vel[i] += accelY * dt;
                z_vel[i] += accelZ * dt;

                x_pos[i] += x_vel[i] * dt;
                y_pos[i] += y_vel[i] * dt;
                z_pos[i] += z_vel[i] * dt;

                x_force[i] = 0.0f;
                y_force[i] = 0.0f;
                z_force[i] = 0.0f;

                // ANGULAR MOTION
                int tensorIdx = i * 9;
                float I00 = invInertiaTensor_gpu[tensorIdx + 0];
                float I01 = invInertiaTensor_gpu[tensorIdx + 1];
                float I02 = invInertiaTensor_gpu[tensorIdx + 2];
                float I10 = invInertiaTensor_gpu[tensorIdx + 3];
                float I11 = invInertiaTensor_gpu[tensorIdx + 4];
                float I12 = invInertiaTensor_gpu[tensorIdx + 5];
                float I20 = invInertiaTensor_gpu[tensorIdx + 6];
                float I21 = invInertiaTensor_gpu[tensorIdx + 7];
                float I22 = invInertiaTensor_gpu[tensorIdx + 8];

                float tx = x_torque[i];
                float ty = y_torque[i];
                float tz = z_torque[i];

                float angAccelX = I00 * tx + I01 * ty + I02 * tz;
                float angAccelY = I10 * tx + I11 * ty + I12 * tz;
                float angAccelZ = I20 * tx + I21 * ty + I22 * tz;

                float avx = x_angVel[i] + angAccelX * dt;
                float avy = y_angVel[i] + angAccelY * dt;
                float avz = z_angVel[i] + angAccelZ * dt;

                x_angVel[i] = avx;
                y_angVel[i] = avy;
                z_angVel[i] = avz;

                x_torque[i] = 0.0f;
                y_torque[i] = 0.0f;
                z_torque[i] = 0.0f;

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

                    float qw = orientW_gpu[i];
                    float qx = orientX_gpu[i];
                    float qy = orientY_gpu[i];
                    float qz = orientZ_gpu[i];

                    float nw = dw * qw - dx * qx - dy * qy - dz * qz;
                    float nx = dw * qx + dx * qw + dy * qz - dz * qy;
                    float ny = dw * qy - dx * qz + dy * qw + dz * qx;
                    float nz = dw * qz + dx * qy - dy * qx + dz * qw;

                    float len = sycl::sqrt(nw * nw + nx * nx + ny * ny + nz * nz);
                    if (len > 1e-6f) {
                        float invLen = 1.0f / len;
                        orientW_gpu[i] = nw * invLen;
                        orientX_gpu[i] = nx * invLen;
                        orientY_gpu[i] = ny * invLen;
                        orientZ_gpu[i] = nz * invLen;
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

void gpu::PhysicsKernel::solve() {
    std::cout << "solve() - TODO" << std::endl;
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

        m_queue.submit([this, n, xdim, ydim, &matrixBuf](sycl::handler& h) {
            auto matrices = matrixBuf.get_access<sycl::access::mode::write>(h);

            h.parallel_for(sycl::range<2>{ydim, xdim},
                [this, n, xdim, matrices](sycl::item<2> item) {

                    std::size_t i = item[0] * xdim + item[1];
                    if (i >= n) return;

                    int baseIdx = i * 16;

                    // Get position
                    float px = x_pos[i];
                    float py = y_pos[i];
                    float pz = z_pos[i];

                    // Get orientation (quaternion)
                    float qw = orientW_gpu[i];
                    float qx = orientX_gpu[i];
                    float qy = orientY_gpu[i];
                    float qz = orientZ_gpu[i];

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