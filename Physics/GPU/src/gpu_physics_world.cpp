#include "../include/gpu_physics_world.hpp"
#include <iostream>
#include <CL/opencl.h>
gpu::PhysicsWorld::PhysicsWorld() : m_numBodies(0), m_capacity(0) {

}
gpu::PhysicsWorld::~PhysicsWorld() {
    // Free GPU memory
    if (x_pos) sycl::free(x_pos, m_queue);
    if (y_pos) sycl::free(y_pos, m_queue);
    if (z_pos) sycl::free(z_pos, m_queue);
    if (x_vel) sycl::free(x_vel, m_queue);
    if (y_vel) sycl::free(y_vel, m_queue);
    if (z_vel) sycl::free(z_vel, m_queue);
    if (orientW_gpu) sycl::free(orientW_gpu, m_queue);
    if (orientX_gpu) sycl::free(orientX_gpu, m_queue);
    if (orientY_gpu) sycl::free(orientY_gpu, m_queue);
    if (orientZ_gpu) sycl::free(orientZ_gpu, m_queue);

    // ADD THIS!
    if (invMass_gpu) sycl::free(invMass_gpu, m_queue);

    // Delete OpenGL buffer
    if (m_modelMatrixSSBO) {
        glDeleteBuffers(1, &m_modelMatrixSSBO);
    }

    std::cout << "GPU Physics cleaned up" << std::endl;
}
void gpu::PhysicsWorld::init(int maxBodies) {
    m_numBodies = 0;

    // Print GPU device info
    auto device = m_queue.get_device();
    std::cout << "GPU Physics initializing on: "
        << device.get_info<sycl::info::device::name>()
        << std::endl;

    // ========================================
    // Step 1: Allocate physics data on GPU (SYCL USM)
    // ========================================
    std::cout << "Allocating GPU memory for " << maxBodies << " bodies..." << std::endl;

    // Positions
    x_pos = sycl::malloc_device<float>(maxBodies, m_queue);
    y_pos = sycl::malloc_device<float>(maxBodies, m_queue);
    z_pos = sycl::malloc_device<float>(maxBodies, m_queue);

    // Velocities
    x_vel = sycl::malloc_device<float>(maxBodies, m_queue);
    y_vel = sycl::malloc_device<float>(maxBodies, m_queue);
    z_vel = sycl::malloc_device<float>(maxBodies, m_queue);

    // Orientations (quaternions)
    orientW_gpu = sycl::malloc_device<float>(maxBodies, m_queue);
    orientX_gpu = sycl::malloc_device<float>(maxBodies, m_queue);
    orientY_gpu = sycl::malloc_device<float>(maxBodies, m_queue);
    orientZ_gpu = sycl::malloc_device<float>(maxBodies, m_queue);

    // Initialize to zero/identity
    m_queue.memset(x_pos, 0, maxBodies * sizeof(float)).wait();
    m_queue.memset(y_pos, 0, maxBodies * sizeof(float)).wait();
    m_queue.memset(z_pos, 0, maxBodies * sizeof(float)).wait();

    m_queue.memset(x_vel, 0, maxBodies * sizeof(float)).wait();
    m_queue.memset(y_vel, 0, maxBodies * sizeof(float)).wait();
    m_queue.memset(z_vel, 0, maxBodies * sizeof(float)).wait();

    // Initialize orientations to identity quaternion (1, 0, 0, 0)
    m_queue.fill(orientW_gpu, 1.0f, maxBodies).wait();
    m_queue.memset(orientX_gpu, 0, maxBodies * sizeof(float)).wait();
    m_queue.memset(orientY_gpu, 0, maxBodies * sizeof(float)).wait();
    m_queue.memset(orientZ_gpu, 0, maxBodies * sizeof(float)).wait();
    // Initialize invMass to zero (static by default)
    m_queue.memset(invMass_gpu, 0, maxBodies * sizeof(float)).wait();
    // ========================================
    // Step 2: Create OpenGL SSBO for model matrices
    // ========================================
    std::cout << "Creating OpenGL SSBO for model matrices..." << std::endl;

    glGenBuffers(1, &m_modelMatrixSSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_modelMatrixSSBO);
    glBufferData(GL_SHADER_STORAGE_BUFFER,
        maxBodies * 16 * sizeof(float),  // mat4 = 16 floats
        nullptr,
        GL_DYNAMIC_DRAW);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    // Check OpenGL errors
    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        std::cerr << "OpenGL error creating SSBO: " << err << std::endl;
        return;
    }

    std::cout << "GPU Physics initialization complete!" << std::endl;
}

void gpu::PhysicsWorld::update(float dt) {
    if (m_numBodies == 0) return;

    std::size_t n = static_cast<std::size_t>(m_numBodies);
    std::size_t xdim = static_cast<std::size_t>(std::ceil(std::sqrt(n)));
    std::size_t ydim = xdim;
    m_queue = flib::sycl_handler::get_queue();
    // ========================================
    // PART 1: Update physics (SYCL USM - no OpenGL)
    // ========================================

    // TODO: Integration kernel on x_pos, y_pos, z_pos, etc.
    fungt::gpu::IntegrationKernel::integrate(m_queue, x_pos, y_pos, z_pos, x_vel, y_vel, z_vel, invMass_gpu,dt);

    // TODO: Collision kernel
    // TODO: Solver kernel

    // ========================================
    // PART 2: Build model matrices (SYCL-OpenGL interop)
    // ========================================
    
    cl_context clcontext = flib::sycl_handler::get_clContext();
    cl_command_queue clqueue = sycl::get_native<sycl::backend::opencl>(m_queue);

    cl_mem clbuffer = clCreateFromGLBuffer(clcontext, CL_MEM_WRITE_ONLY,
        m_modelMatrixSSBO, NULL);

    glFinish();

    cl_event acquire_event;
    clEnqueueAcquireGLObjects(clqueue, 1, &clbuffer, 0, NULL, &acquire_event);
    clWaitForEvents(1, &acquire_event);

    {
        sycl::context syclCtx = flib::sycl_handler::get_sycl_context();
        sycl::buffer<float> matrixBuf =
            sycl::make_buffer<sycl::backend::opencl, float>(clbuffer, syclCtx);

        m_queue.submit([&](sycl::handler& cgh) {
            auto matrices = matrixBuf.get_access<sycl::access::mode::write>(cgh);

            // Capture physics pointers
            float* px = x_pos;
            float* py = y_pos;
            float* pz = z_pos;
            float* qw = orientW_gpu;
            float* qx = orientX_gpu;
            float* qy = orientY_gpu;
            float* qz = orientZ_gpu;

            cgh.parallel_for(sycl::range<2>{xdim, ydim},
                [=](sycl::item<2> item) {

                    std::size_t i = item[0] * ydim + item[1];
                    if (i >= n) return;

                    // Build TRS matrix from physics state
                    // TODO: Proper matrix computation
                    int baseIdx = i * 16;

                    // For now: Translation only
                    matrices[baseIdx + 0] = 1.0f;
                    matrices[baseIdx + 5] = 1.0f;
                    matrices[baseIdx + 10] = 1.0f;
                    matrices[baseIdx + 12] = px[i];  // Translation X
                    matrices[baseIdx + 13] = py[i];  // Translation Y
                    matrices[baseIdx + 14] = pz[i];  // Translation Z
                    matrices[baseIdx + 15] = 1.0f;
                });
            });

        m_queue.wait();
    }

    clFinish(clqueue);

    cl_event release_event;
    clEnqueueReleaseGLObjects(clqueue, 1, &clbuffer, 0, NULL, &release_event);
    clWaitForEvents(1, &release_event);

    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

    clReleaseMemObject(clbuffer);
}

void gpu::PhysicsWorld::runColliders() {
    // This will contain broad-phase and narrow-phase collision detection

    // TODO: Spatial hashing kernel (broad-phase)
    // SpatialHashKernel::buildGrid(...);

    // TODO: Collision detection kernel (narrow-phase)
    // CollisionKernel::detectCollisions(...);

    std::cout << "runColliders() - Not implemented yet" << std::endl;
}