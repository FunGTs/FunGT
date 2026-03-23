#include <iostream>
#include <cmath>
#include "gpu_physics_kernel.hpp"


gpu::PhysicsKernel::PhysicsKernel()
    : m_numBodies(0), m_capacity(0),
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
    //Set the queue:
    m_queue = flib::sycl_handler::get_queue();
    //Kernel memory allocations
    initMemoryAllocations(maxBodies);
    //CPU   DATA
    m_staging.resize(maxBodies);

    m_spatialGrid = std::make_unique<SpatialGrid>(m_queue);
    if(m_spatialGrid==nullptr){
        std::cout << "Error allocating spatial grid pointer" << std::endl;
        return;
    }
    m_spatialGrid->init(maxBodies,2.0,100.f);
    // Create persistent CL interop from GL SSBO
    m_clQueue = sycl::get_native<sycl::backend::opencl>(m_queue);

    
    m_clInteropBuffer = clCreateFromGLBuffer(
        flib::sycl_handler::get_clContext(),
        CL_MEM_WRITE_ONLY,
        m_modelMatrixSSBO,
        NULL);

    if (m_clInteropBuffer == NULL) {
        std::cerr << "Failed to create CL buffer from GL SSBO!" << std::endl;
        return;
    }

    m_interopInitialized = true;
   
}

void gpu::PhysicsKernel::initMemoryAllocations(int maxBodies)
{
    m_capacity = maxBodies;
    m_numBodies = 0;

    
    if (!checkGPUMemory(m_queue, maxBodies, m_worldSize, m_cellSize)) {
        throw std::runtime_error("Not enough GPU memory for requested body count");
    }
    auto device = m_queue.get_device();
    std::cout << "GPU Physics Kernel initializing on: "
        << device.get_info<sycl::info::device::name>()
        << std::endl;

    std::cout << "Allocating GPU memory for " << maxBodies << " bodies..." << std::endl;
    m_maxManifolds = maxBodies * 8;  // worst case: each body touches 4 others
    m_hashTableSize = m_maxManifolds * 4;  // keep hash table sparse

    m_manifolds = sycl::malloc_device<GPUManifold>(m_maxManifolds, m_queue);
    m_numManifolds = sycl::malloc_device<int>(1, m_queue);
    m_pairToManifold = sycl::malloc_device<int>(m_hashTableSize, m_queue);


    //Allocate shape data:
    m_data.shapeType = sycl::malloc_device<int>(maxBodies, m_queue);      // 0 = sphere, 1 = box
    m_data.bodyMode = sycl::malloc_device<int>(maxBodies, m_queue);      // 0 = STATIC, 1 = DYNAMIC
    m_data.radius = sycl::malloc_device<float>(maxBodies, m_queue);       // for spheres
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

    m_data.dx_vel = sycl::malloc_device<float>(maxBodies, m_queue);
    m_data.dy_vel = sycl::malloc_device<float>(maxBodies, m_queue);
    m_data.dz_vel = sycl::malloc_device<float>(maxBodies, m_queue);
    m_data.dx_angVel = sycl::malloc_device<float>(maxBodies, m_queue);
    m_data.dy_angVel = sycl::malloc_device<float>(maxBodies, m_queue);
    m_data.dz_angVel = sycl::malloc_device<float>(maxBodies, m_queue);
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

    m_queue.memset(m_data.restitution, 0, maxBodies * sizeof(float)).wait();
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
void gpu::PhysicsKernel::initUniformGrid(){

    // Grid parameters - tune these based on your world
    float worldSize = 100.0f;  // Adjust based on your simulation bounds
    float maxSphereRadius = 1.0f;  // Your largest sphere

    m_gridData.cellSize = 2.0f * maxSphereRadius;
    m_gridData.invCellSize = 1.0f / m_gridData.cellSize;

    m_gridData.gridDimX = static_cast<int>(worldSize / m_gridData.cellSize);
    m_gridData.gridDimY = static_cast<int>(worldSize / m_gridData.cellSize);
    m_gridData.gridDimZ = static_cast<int>(worldSize / m_gridData.cellSize);

    m_gridData.totalCells = m_gridData.gridDimX * m_gridData.gridDimY * m_gridData.gridDimZ;

    // Allocate device memory
    m_gridData.cellHash = sycl::malloc_device<int>(m_capacity, m_queue); //m_capacity = maxBodies
    m_gridData.bodyIndex = sycl::malloc_device<int>(m_capacity, m_queue);
    m_gridData.cellStart = sycl::malloc_device<int>(m_gridData.totalCells, m_queue);
    m_gridData.cellEnd = sycl::malloc_device<int>(m_gridData.totalCells, m_queue);

    m_gridInitialized = true;

    std::cout << "Uniform grid initialized: "
        << m_gridData.gridDimX << "x"
        << m_gridData.gridDimY << "x"
        << m_gridData.gridDimZ
        << " = " << m_gridData.totalCells << " cells\n";
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
    if (m_data.dx_vel) sycl::free(m_data.dx_vel, m_queue);
    if (m_data.dy_vel) sycl::free(m_data.dy_vel, m_queue);
    if (m_data.dz_vel) sycl::free(m_data.dz_vel, m_queue);
    if (m_data.dx_angVel) sycl::free(m_data.dx_angVel, m_queue);
    if (m_data.dy_angVel) sycl::free(m_data.dy_angVel, m_queue);
    if (m_data.dz_angVel) sycl::free(m_data.dz_angVel, m_queue);
    if (m_data.invInertiaTensor) sycl::free(m_data.invInertiaTensor, m_queue);
    // NEW: Free grid memory
    if (m_gridInitialized) {
        sycl::free(m_gridData.cellHash, m_queue);
        sycl::free(m_gridData.bodyIndex, m_queue);
        sycl::free(m_gridData.cellStart, m_queue);
        sycl::free(m_gridData.cellEnd, m_queue);
    }
    // Delete OpenGL buffer
    if (m_modelMatrixSSBO) {
        glDeleteBuffers(1, &m_modelMatrixSSBO);
        m_modelMatrixSSBO = 0;
    }
    if (m_clInteropBuffer) {
        clReleaseMemObject(m_clInteropBuffer);
        m_clInteropBuffer = nullptr;
    }
    std::cout << "GPU Physics Kernel cleaned up" << std::endl;
}

void gpu::PhysicsKernel::sendToDevice()
{
    if (m_numBodies == 0) return;
    if (m_flushed) {
        std::cerr << "WARNING: sendToDevice() already called. Ignoring." << std::endl;
        return;
    }

    int n = m_numBodies;
    std::cout << "Sending " << n << " bodies to GPU..." << std::endl;

    // Shape data
    m_queue.memcpy(m_data.shapeType, m_staging.shapeType.data(), n * sizeof(int));
    m_queue.memcpy(m_data.bodyMode, m_staging.bodyMode.data(), n * sizeof(int));
    m_queue.memcpy(m_data.radius, m_staging.radius.data(), n * sizeof(float));
    m_queue.memcpy(m_data.halfExtentX, m_staging.halfExtentX.data(), n * sizeof(float));
    m_queue.memcpy(m_data.halfExtentY, m_staging.halfExtentY.data(), n * sizeof(float));
    m_queue.memcpy(m_data.halfExtentZ, m_staging.halfExtentZ.data(), n * sizeof(float));

    // Positions
    m_queue.memcpy(m_data.x_pos, m_staging.x_pos.data(), n * sizeof(float));
    m_queue.memcpy(m_data.y_pos, m_staging.y_pos.data(), n * sizeof(float));
    m_queue.memcpy(m_data.z_pos, m_staging.z_pos.data(), n * sizeof(float));

    // Linear velocity
    m_queue.memcpy(m_data.x_vel, m_staging.x_vel.data(), n * sizeof(float));
    m_queue.memcpy(m_data.y_vel, m_staging.y_vel.data(), n * sizeof(float));
    m_queue.memcpy(m_data.z_vel, m_staging.z_vel.data(), n * sizeof(float));

    // Forces
    m_queue.memcpy(m_data.x_force, m_staging.x_force.data(), n * sizeof(float));
    m_queue.memcpy(m_data.y_force, m_staging.y_force.data(), n * sizeof(float));
    m_queue.memcpy(m_data.z_force, m_staging.z_force.data(), n * sizeof(float));

    // Angular velocity
    m_queue.memcpy(m_data.x_angVel, m_staging.x_angVel.data(), n * sizeof(float));
    m_queue.memcpy(m_data.y_angVel, m_staging.y_angVel.data(), n * sizeof(float));
    m_queue.memcpy(m_data.z_angVel, m_staging.z_angVel.data(), n * sizeof(float));

    // Torques
    m_queue.memcpy(m_data.x_torque, m_staging.x_torque.data(), n * sizeof(float));
    m_queue.memcpy(m_data.y_torque, m_staging.y_torque.data(), n * sizeof(float));
    m_queue.memcpy(m_data.z_torque, m_staging.z_torque.data(), n * sizeof(float));

    // Orientations
    m_queue.memcpy(m_data.orientW, m_staging.orientW.data(), n * sizeof(float));
    m_queue.memcpy(m_data.orientX, m_staging.orientX.data(), n * sizeof(float));
    m_queue.memcpy(m_data.orientY, m_staging.orientY.data(), n * sizeof(float));
    m_queue.memcpy(m_data.orientZ, m_staging.orientZ.data(), n * sizeof(float));

    // Mass properties
    m_queue.memcpy(m_data.invMass, m_staging.invMass.data(), n * sizeof(float));
    m_queue.memcpy(m_data.invInertiaTensor, m_staging.invInertiaTensor.data(), n * 9 * sizeof(float));

    // Material
    m_queue.memcpy(m_data.restitution, m_staging.restitution.data(), n * sizeof(float));
    m_queue.memcpy(m_data.friction, m_staging.friction.data(), n * sizeof(float));

    // Single synchronization point
    m_queue.wait();

    m_flushed = true;
    std::cout << "Flush complete: " << n << " bodies uploaded in one batch." << std::endl;
}

void gpu::PhysicsKernel::sortBodiesByCell()
{
    m_radixSort->sort(m_gridData.cellHash, m_gridData.bodyIndex, m_numBodies);
}

void gpu::PhysicsKernel::findCellBoundaries()
{
    UniformGridData grid = m_gridData;
    int n = m_numBodies;

    // Reset all cells to empty
    m_queue.fill(grid.cellStart, -1, grid.totalCells).wait();
    m_queue.fill(grid.cellEnd, -1, grid.totalCells).wait();

    std::size_t xdim = 32;
    std::size_t ydim = (n + xdim - 1) / xdim;

    m_queue.submit([&](sycl::handler& cgh) {
        cgh.parallel_for(sycl::range<2>(ydim, xdim), [=](sycl::item<2> item) {
            std::size_t i = item[0] * xdim + item[1];
            if (i >= n) return;

            int cellHash = grid.cellHash[i];
            if (cellHash < 0) return;  // Skip static bodies

            // Check if this is the start of a new cell
            if (i == 0 || grid.cellHash[i - 1] != cellHash) {
                grid.cellStart[cellHash] = i;
            }

            // Check if this is the end of a cell
            if (i == n - 1 || grid.cellHash[i + 1] != cellHash) {
                grid.cellEnd[cellHash] = i + 1;
            }
            });
    }).wait();
}

void gpu::PhysicsKernel::debugGrid()
{
    std::cout << "\n========== GRID DEBUG ==========\n";

    // Copy cell hashes and body indices to host
    std::vector<int> cellHash_host(m_numBodies);
    std::vector<int> bodyIndex_host(m_numBodies);
    std::vector<float> x_pos_host(m_numBodies);
    std::vector<float> y_pos_host(m_numBodies);
    std::vector<int> bodyMode_host(m_numBodies);

    m_queue.memcpy(cellHash_host.data(), m_gridData.cellHash, m_numBodies * sizeof(int)).wait();
    m_queue.memcpy(bodyIndex_host.data(), m_gridData.bodyIndex, m_numBodies * sizeof(int)).wait();
    m_queue.memcpy(x_pos_host.data(), m_data.x_pos, m_numBodies * sizeof(float)).wait();
    m_queue.memcpy(y_pos_host.data(), m_data.y_pos, m_numBodies * sizeof(float)).wait();
    m_queue.memcpy(bodyMode_host.data(), m_data.bodyMode, m_numBodies * sizeof(int)).wait();

    std::cout << "Total bodies: " << m_numBodies << "\n";
    std::cout << "Grid: " << m_gridData.gridDimX << "x" << m_gridData.gridDimY << "x" << m_gridData.gridDimZ << "\n";
    std::cout << "Cell size: " << m_gridData.cellSize << "\n\n";

    // Print first 11 bodies (ground + 10 balls)
    for (int i = 0; i < std::min(11, m_numBodies); i++) {
        int originalIdx = bodyIndex_host[i];
        std::cout << "Index[" << i << "]: "
            << "bodyID=" << originalIdx << " "
            << "mode=" << (bodyMode_host[originalIdx] == 0 ? "STATIC" : "DYNAMIC") << " "
            << "pos=(" << x_pos_host[originalIdx] << "," << y_pos_host[originalIdx] << ") "
            << "cellHash=" << cellHash_host[i] << "\n";
    }
    std::cout << "================================\n\n";
}
int gpu::PhysicsKernel::addSphere(float x, float y, float z, float radius, float mass, float vx, float vy, float vz, MODE mode) {
    if (m_numBodies >= m_capacity) {
        std::cerr << "ERROR: Physics kernel is full!" << std::endl;
        return -1;
    }

    int id = m_numBodies++;

    // Position
    m_staging.x_pos[id] = x;
    m_staging.y_pos[id] = y;
    m_staging.z_pos[id] = z;

    // Lateral velocity for natural spin on impact

    m_staging.x_vel[id] = vx;
    m_staging.y_vel[id] = vy;
    m_staging.z_vel[id] = vz;

    // Forces, angular velocity, torque all default to 0.0f from resize()

    // Orientation: identity quaternion (w=1 already set by resize())

    // Inverse mass
    float invMassVal = (mass > 0.0f) ? (1.0f / mass) : 0.0f;
    m_staging.invMass[id] = invMassVal;

    // Inverse inertia tensor for sphere: I = (2/5) * m * r^2
    float invInertia = (mass > 0.0f) ? (2.5f / (mass * radius * radius)) : 0.0f;
    int tensorIdx = id * 9;
    m_staging.invInertiaTensor[tensorIdx + 0] = invInertia;
    m_staging.invInertiaTensor[tensorIdx + 4] = invInertia;
    m_staging.invInertiaTensor[tensorIdx + 8] = invInertia;
    // Off-diagonal entries default to 0.0f from resize()

    // Shape data
    m_staging.shapeType[id] = 0;  // sphere
    m_staging.bodyMode[id] = (mode == MODE::DYNAMIC) ? 1 : 0;
    m_staging.radius[id] = radius;

    // Material
    m_staging.restitution[id] = 0.8f;
    m_staging.friction[id] = 0.3f;

    return id;
}
int gpu::PhysicsKernel::addBox(float x, float y, float z, float width, float height, float depth, float mass, MODE mode) {
    if (m_numBodies >= m_capacity) {
        std::cerr << "ERROR: Physics kernel is full!" << std::endl;
        return -1;
    }

    int id = m_numBodies++;

    // Position
    m_staging.x_pos[id] = x;
    m_staging.y_pos[id] = y;
    m_staging.z_pos[id] = z;

    // Velocity, forces, angular, torque all default to 0.0f from resize()

    // Orientation: identity quaternion (w=1 already set by resize())

    // Inverse mass
    float invMassVal = (mass > 0.0f) ? (1.0f / mass) : 0.0f;
    m_staging.invMass[id] = invMassVal;

    // Inverse inertia tensor for box: I = (1/12) * m * (h^2 + d^2, w^2 + d^2, w^2 + h^2)
    int tensorIdx = id * 9;
    if (mass > 0.0f) {
        float w2 = width * width;
        float h2 = height * height;
        float d2 = depth * depth;
        m_staging.invInertiaTensor[tensorIdx + 0] = 12.0f / (mass * (h2 + d2));
        m_staging.invInertiaTensor[tensorIdx + 4] = 12.0f / (mass * (w2 + d2));
        m_staging.invInertiaTensor[tensorIdx + 8] = 12.0f / (mass * (w2 + h2));
    }

    // Shape data
    m_staging.shapeType[id] = 1;  // box
    m_staging.bodyMode[id] = (mode == MODE::DYNAMIC) ? 1 : 0;
    m_staging.halfExtentX[id] = width * 0.5f;
    m_staging.halfExtentY[id] = height * 0.5f;
    m_staging.halfExtentZ[id] = depth * 0.5f;

    // Material
    m_staging.restitution[id] = 0.5f;
    m_staging.friction[id] = 0.3f;

    return id;
}
void gpu::PhysicsKernel::computeCellHashes() {
    DeviceData data = m_data;
    UniformGridData grid = m_gridData;
    int n = m_numBodies;

    std::size_t xdim = 32;
    std::size_t ydim = (n + xdim - 1) / xdim;

    m_queue.submit([&](sycl::handler& cgh) {
        cgh.parallel_for(sycl::range<2>(ydim, xdim), [=](sycl::item<2> item) {
            std::size_t i = item[0] * xdim + item[1];
            if (i >= n) return;

            // Skip static bodies (they don't go in the grid)
            if (data.bodyMode[i] == 0) {
                grid.cellHash[i] = -1;
                grid.bodyIndex[i] = i;
                return;
            }

            // Get body position
            float px = data.x_pos[i];
            float py = data.y_pos[i];
            float pz = data.z_pos[i];

            // Compute grid cell coordinates
            float offsetX = px + 50.0f;  // shift so minimum position maps to positive
            float offsetY = py + 50.0f;
            float offsetZ = pz + 50.0f;

            int cellX = static_cast<int>(sycl::floor(offsetX * grid.invCellSize));
            int cellY = static_cast<int>(sycl::floor(offsetY * grid.invCellSize));
            int cellZ = static_cast<int>(sycl::floor(offsetZ * grid.invCellSize));
            // Clamp to grid bounds
            cellX = sycl::clamp(cellX, 0, grid.gridDimX - 1);
            cellY = sycl::clamp(cellY, 0, grid.gridDimY - 1);
            cellZ = sycl::clamp(cellZ, 0, grid.gridDimZ - 1);

            // Compute 1D cell hash
            int cellHash = cellX + cellY * grid.gridDimX + cellZ * grid.gridDimX * grid.gridDimY;

                grid.cellHash[i] = cellHash;
                grid.bodyIndex[i] = i;
            });
        }).wait();
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
                    // data.x_vel[i] *= 0.98f;
                    // data.y_vel[i] *= 0.98f;
                    // data.z_vel[i] *= 0.98f;
                    // data.x_angVel[i] *= 0.95f;
                    // data.y_angVel[i] *= 0.95f;
                    // data.z_angVel[i] *= 0.95f;
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
void gpu::PhysicsKernel::solveImpulsesB(float dt) {
    DeviceData data = m_data;
    GPUManifold* manifolds = m_manifolds;
    int* numManifolds = m_numManifolds;

    constexpr float ERP = 0.2f;
    constexpr int ITERATIONS = 16;

    int manifoldCount;
    m_queue.memcpy(&manifoldCount, numManifolds, sizeof(int)).wait();

    if (manifoldCount == 0) return;

    std::size_t mxdim = 32;
    std::size_t mydim = (manifoldCount + mxdim - 1) / mxdim;
    std::size_t mn = static_cast<std::size_t>(manifoldCount);

    std::size_t bxdim = 32;
    std::size_t bydim = (m_numBodies + bxdim - 1) / bxdim;
    std::size_t bn = static_cast<std::size_t>(m_numBodies);
    std::cout << "manifolds: " << manifoldCount << " / " << m_maxManifolds << std::endl;
    for (int iter = 0; iter < ITERATIONS; iter++) {
        // 1. Zero accumulators
        m_queue.memset(data.dx_vel, 0, m_numBodies * sizeof(float));
        m_queue.memset(data.dy_vel, 0, m_numBodies * sizeof(float));
        m_queue.memset(data.dz_vel, 0, m_numBodies * sizeof(float));
        m_queue.memset(data.dx_angVel, 0, m_numBodies * sizeof(float));
        m_queue.memset(data.dy_angVel, 0, m_numBodies * sizeof(float));
        m_queue.memset(data.dz_angVel, 0, m_numBodies * sizeof(float));
        m_queue.wait();

        // 2. Solve: read from velocities, write to accumulators
        m_queue.submit([data, manifolds, mn, mxdim, mydim, dt, ERP](sycl::handler& h) {
            h.parallel_for(sycl::range<2>(mydim, mxdim),
                [data, manifolds, mn, mxdim, dt, ERP](sycl::item<2> item) {
                    std::size_t i = item[0] * mxdim + item[1];
                    if (i >= mn) return;

                    GPUManifold* m = &manifolds[i];
                    int bodyA = m->bodyA;
                    int bodyB = m->bodyB;

                    for (int p = 0; p < m->numPoints; p++) {
                        solveContactImpulseJacobi(&m->points[p], data, bodyA, bodyB, dt, ERP);
                    }
                });
            }).wait();

        // 3. Apply accumulators to actual velocities
        m_queue.submit([data, bn, bxdim, bydim](sycl::handler& h) {
            h.parallel_for(sycl::range<2>(bydim, bxdim),
                [data, bn, bxdim](sycl::item<2> item) {
                    std::size_t i = item[0] * bxdim + item[1];
                    if (i >= bn) return;
                    if (data.bodyMode[i] == 0) return;

                    data.x_vel[i] += data.dx_vel[i];
                    data.y_vel[i] += data.dy_vel[i];
                    data.z_vel[i] += data.dz_vel[i];
                    data.x_angVel[i] += data.dx_angVel[i];
                    data.y_angVel[i] += data.dy_angVel[i];
                    data.z_angVel[i] += data.dz_angVel[i];
                });
            }).wait();
    }
}
void gpu::PhysicsKernel::solveImpulsesA(float dt) {
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

    // Finish OpenGL operations
    glFinish();

    // Acquire GL object for SYCL use

    cl_event acquire_event;
    clEnqueueAcquireGLObjects(m_clQueue, 1, &m_clInteropBuffer, 0, NULL, &acquire_event);
    clWaitForEvents(1, &acquire_event);

    // SYCL kernel scope
    {
        sycl::context syclCtx = flib::sycl_handler::get_sycl_context();
        sycl::buffer<float> matrixBuf =
            sycl::make_buffer<sycl::backend::opencl, float>(m_clInteropBuffer, syclCtx);

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

    clFinish(m_clQueue);

    // Release GL object back to OpenGL
    cl_event release_event;
    clEnqueueReleaseGLObjects(m_clQueue, 1, &m_clInteropBuffer, 0, NULL, &release_event);
    clWaitForEvents(1, &release_event);

    // Memory barrier
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

    // Release OpenCL memory object
    //clReleaseMemObject(clbuffer);
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

void gpu::PhysicsKernel::detectDynamicVsDynamic()
{

    DeviceData data = m_data;
    UniformGridData grid = m_gridData;
    GPUManifold* manifolds = m_manifolds;
    int* pairToManifold = m_pairToManifold;
    int* numManifolds = m_numManifolds;
    int hashTableSize = m_hashTableSize;
    int maxManifolds = m_maxManifolds;
    int n = m_numBodies;

    std::size_t xdim = 32;
    std::size_t ydim = (n + xdim - 1) / xdim;

    m_queue.submit([&](sycl::handler& cgh) {
        cgh.parallel_for(sycl::range<2>(ydim, xdim), [=](sycl::item<2> item) {
            std::size_t i = item[0] * xdim + item[1];
            if (i >= n) return;

            // Get original body index
            int bodyA = grid.bodyIndex[i];

            // Skip if static
            if (data.bodyMode[bodyA] == 0) return;

            // Get body A's cell
            int cellHash = grid.cellHash[i];
            if (cellHash < 0) return;

            // Compute 3D cell coordinates
            int gridDimX = grid.gridDimX;
            int gridDimY = grid.gridDimY;
            int gridDimZ = grid.gridDimZ;
            
            int cellX = cellHash % gridDimX;
            int cellY = (cellHash / gridDimX) % gridDimY;
            int cellZ = cellHash / (gridDimX * gridDimY);

            // Check 27 neighboring cells (3x3x3)
            for (int dz = -1; dz <= 1; dz++) {
                for (int dy = -1; dy <= 1; dy++) {
                    for (int dx = -1; dx <= 1; dx++) {
                        int nx = cellX + dx;
                        int ny = cellY + dy;
                        int nz = cellZ + dz;

                        // Bounds check
                        if (nx < 0 || nx >= gridDimX) continue;
                        if (ny < 0 || ny >= gridDimY) continue;
                        if (nz < 0 || nz >= gridDimZ) continue;

                        // Compute neighbor cell hash
                        int neighborHash = nx + ny * gridDimX + nz * gridDimX * gridDimY;

                        // Get bodies in this cell
                        int start = grid.cellStart[neighborHash];
                        int end = grid.cellEnd[neighborHash];

                        if (start == -1) continue;  // Empty cell

                        // Check all bodies in neighbor cell
                        for (int j = start; j < end; j++) {
                            int bodyB = grid.bodyIndex[j];

                            // Skip self
                            if (bodyA == bodyB) continue;

                            // Skip if static
                            if (data.bodyMode[bodyB] == 0) continue;

                            // Avoid duplicate pairs (only check if bodyA < bodyB)
                            if (bodyA >= bodyB) continue;

                            // Both are spheres - do sphere-sphere collision
                            if (data.shapeType[bodyA] == 0 && data.shapeType[bodyB] == 0) {
                                // Sphere positions
                                float ax = data.x_pos[bodyA];
                                float ay = data.y_pos[bodyA];
                                float az = data.z_pos[bodyA];
                                float ar = data.radius[bodyA];

                                float bx = data.x_pos[bodyB];
                                float by = data.y_pos[bodyB];
                                float bz = data.z_pos[bodyB];
                                float br = data.radius[bodyB];

                                // Distance check
                                float dx_sphere = bx - ax;
                                float dy_sphere = by - ay;
                                float dz_sphere = bz - az;
                                float dxSquare = dx_sphere*dx_sphere;
                                float dySquare = dy_sphere*dy_sphere;
                                float dzSquare = dz_sphere*dz_sphere;
                                float distSq = dxSquare + dySquare + dzSquare;
                                float combinedRadius = ar + br;

                                if (distSq < combinedRadius * combinedRadius && distSq > 0.0001f) {
                                    float dist = sycl::sqrt(distSq);
                                    float penetration = combinedRadius - dist;

                                    // Normal from A to B
                                    float nx = dx_sphere / dist;
                                    float ny = dy_sphere / dist;
                                    float nz = dz_sphere / dist;

                                    // Contact points
                                    float worldAx = ax + nx * ar;
                                    float worldAy = ay + ny * ar;
                                    float worldAz = az + nz * ar;

                                    float worldBx = bx - nx * br;
                                    float worldBy = by - ny * br;
                                    float worldBz = bz - nz * br;

                                    float localAx = nx * ar;
                                    float localAy = ny * ar;
                                    float localAz = nz * ar;

                                    float localBx = -nx * br;
                                    float localBy = -ny * br;
                                    float localBz = -nz * br;

                                    // Create/update manifold
                                    int manifoldIdx = findManifold(pairToManifold, manifolds,
                                        hashTableSize, bodyA, bodyB);
                                    if (manifoldIdx == -1) {
                                        manifoldIdx = createManifold(pairToManifold, manifolds,
                                            numManifolds, hashTableSize,
                                            maxManifolds, bodyA, bodyB);
                                    }

                                    if (manifoldIdx != -1) {
                                        addContactToManifold(manifolds, manifoldIdx,
                                            localAx, localAy, localAz,
                                            localBx, localBy, localBz,
                                            worldAx, worldAy, worldAz,
                                            worldBx, worldBy, worldBz,
                                            nx, ny, nz, penetration);
                                    }
                                }
                            }
                        }
                    }
                }
            }
            });
        }).wait();
}
void gpu::PhysicsKernel::projectPositions() {
    int manifoldCount;
    m_queue.memcpy(&manifoldCount, m_numManifolds, sizeof(int)).wait();
    if (manifoldCount == 0) return;

    DeviceData data = m_data;
    GPUManifold* manifolds = m_manifolds;
    std::size_t mn = static_cast<std::size_t>(manifoldCount);
    std::size_t mxdim = 32;
    std::size_t mydim = (mn + mxdim - 1) / mxdim;

     float SLOP = 0.01f;
     float CORRECTION_PERCENT = 0.4f;

    m_queue.submit([data, manifolds, mn, mxdim, mydim](sycl::handler& h) {
        h.parallel_for(sycl::range<2>(mydim, mxdim),
            [data, manifolds, mn, mxdim](sycl::item<2> item) {
                std::size_t i = item[0] * mxdim + item[1];
                if (i >= mn) return;

                GPUManifold* m = &manifolds[i];
                int bodyA = m->bodyA;
                int bodyB = m->bodyB;

                float invMassA = data.invMass[bodyA];
                float invMassB = data.invMass[bodyB];
                float totalInvMass = invMassA + invMassB;

                if (totalInvMass <= 0.0001f) return;

                for (int p = 0; p < m->numPoints; p++) {
                    GPUContactPoint* cp = &m->points[p];
                    float pen = cp->penetration - 0.01f;
                    if (pen <= 0.0f) continue;

                    float correctionMag = pen * 0.8f / totalInvMass;

                    float cx = correctionMag * cp->normalX;
                    float cy = correctionMag * cp->normalY;
                    float cz = correctionMag * cp->normalZ;

                    if (data.bodyMode[bodyA] == 1) {
                        sycl::atomic_ref<float, sycl::memory_order::relaxed,
                            sycl::memory_scope::device,
                            sycl::access::address_space::global_space> rx(data.x_pos[bodyA]);
                        sycl::atomic_ref<float, sycl::memory_order::relaxed,
                            sycl::memory_scope::device,
                            sycl::access::address_space::global_space> ry(data.y_pos[bodyA]);
                        sycl::atomic_ref<float, sycl::memory_order::relaxed,
                            sycl::memory_scope::device,
                            sycl::access::address_space::global_space> rz(data.z_pos[bodyA]);
                        rx.fetch_add(-cx * invMassA);
                        ry.fetch_add(-cy * invMassA);
                        rz.fetch_add(-cz * invMassA);
                    }

                    if (data.bodyMode[bodyB] == 1) {
                        sycl::atomic_ref<float, sycl::memory_order::relaxed,
                            sycl::memory_scope::device,
                            sycl::access::address_space::global_space> rx(data.x_pos[bodyB]);
                        sycl::atomic_ref<float, sycl::memory_order::relaxed,
                            sycl::memory_scope::device,
                            sycl::access::address_space::global_space> ry(data.y_pos[bodyB]);
                        sycl::atomic_ref<float, sycl::memory_order::relaxed,
                            sycl::memory_scope::device,
                            sycl::access::address_space::global_space> rz(data.z_pos[bodyB]);
                        rx.fetch_add(cx * invMassB);
                        ry.fetch_add(cy * invMassB);
                        rz.fetch_add(cz * invMassB);
                    }
                }
            });
        }).wait();
}
void gpu::PhysicsKernel::refreshManifolds() {
    int manifoldCount;
    m_queue.memcpy(&manifoldCount, m_numManifolds, sizeof(int)).wait();
    if (manifoldCount == 0) return;

    DeviceData data = m_data;
    GPUManifold* manifolds = m_manifolds;
    std::size_t mn = static_cast<std::size_t>(manifoldCount);
    std::size_t mxdim = 32;
    std::size_t mydim = (mn + mxdim - 1) / mxdim;

    m_queue.submit([data, manifolds, mn, mxdim, mydim](sycl::handler& h) {
        h.parallel_for(sycl::range<2>(mydim, mxdim),
            [data, manifolds, mn, mxdim](sycl::item<2> item) {
                std::size_t i = item[0] * mxdim + item[1];
                if (i >= mn) return;

                GPUManifold* m = &manifolds[i];
                if (m->numPoints == 0) return;

                int bodyA = m->bodyA;
                int bodyB = m->bodyB;

                if (data.shapeType[bodyA] == 0 && data.shapeType[bodyB] == 0) {
                    float ax = data.x_pos[bodyA];
                    float ay = data.y_pos[bodyA];
                    float az = data.z_pos[bodyA];
                    float ar = data.radius[bodyA];
                    float bx = data.x_pos[bodyB];
                    float by = data.y_pos[bodyB];
                    float bz = data.z_pos[bodyB];
                    float br = data.radius[bodyB];

                    float dx = bx - ax;
                    float dy = by - ay;
                    float dz = bz - az;
                    float distSq = dx * dx + dy * dy + dz * dz;
                    float combinedRadius = ar + br;

                    if (distSq >= combinedRadius * combinedRadius || distSq < 0.0001f) {
                        m->numPoints = 0;
                        return;
                    }

                    float dist = sycl::sqrt(distSq);
                    float pen = combinedRadius - dist;
                    float invDist = 1.0f / dist;
                    float nx = dx * invDist;
                    float ny = dy * invDist;
                    float nz = dz * invDist;

                    for (int p = 0; p < m->numPoints; p++) {
                        m->points[p].normalX = nx;
                        m->points[p].normalY = ny;
                        m->points[p].normalZ = nz;
                        m->points[p].penetration = pen;
                        m->points[p].worldPointAx = ax + nx * ar;
                        m->points[p].worldPointAy = ay + ny * ar;
                        m->points[p].worldPointAz = az + nz * ar;
                        m->points[p].worldPointBx = bx - nx * br;
                        m->points[p].worldPointBy = by - ny * br;
                        m->points[p].worldPointBz = bz - nz * br;
                        m->points[p].localPointAx = nx * ar;
                        m->points[p].localPointAy = ny * ar;
                        m->points[p].localPointAz = nz * ar;
                        m->points[p].localPointBx = -nx * br;
                        m->points[p].localPointBy = -ny * br;
                        m->points[p].localPointBz = -nz * br;
                    }
                }

                if (data.shapeType[bodyA] == 1 && data.shapeType[bodyB] == 0) {
                    float sx = data.x_pos[bodyB];
                    float sy = data.y_pos[bodyB];
                    float sz = data.z_pos[bodyB];
                    float sr = data.radius[bodyB];
                    float bx = data.x_pos[bodyA];
                    float by = data.y_pos[bodyA];
                    float bz = data.z_pos[bodyA];
                    float hx = data.halfExtentX[bodyA];
                    float hy = data.halfExtentY[bodyA];
                    float hz = data.halfExtentZ[bodyA];

                    float relX = sx - bx;
                    float relY = sy - by;
                    float relZ = sz - bz;

                    float closestX = sycl::fmax(-hx, sycl::fmin(hx, relX));
                    float closestY = sycl::fmax(-hy, sycl::fmin(hy, relY));
                    float closestZ = sycl::fmax(-hz, sycl::fmin(hz, relZ));

                    float wcx = bx + closestX;
                    float wcy = by + closestY;
                    float wcz = bz + closestZ;

                    float dx = sx - wcx;
                    float dy = sy - wcy;
                    float dz = sz - wcz;
                    float distSq = dx * dx + dy * dy + dz * dz;

                    if (distSq >= sr * sr || distSq < 0.0001f) {
                        m->numPoints = 0;
                        return;
                    }

                    float dist = sycl::sqrt(distSq);
                    float pen = sr - dist;
                    float invDist = 1.0f / dist;
                    float nx = dx * invDist;
                    float ny = dy * invDist;
                    float nz = dz * invDist;

                    for (int p = 0; p < m->numPoints; p++) {
                        m->points[p].normalX = nx;
                        m->points[p].normalY = ny;
                        m->points[p].normalZ = nz;
                        m->points[p].penetration = pen;
                        m->points[p].worldPointAx = sx - nx * (sr - pen);
                        m->points[p].worldPointAy = sy - ny * (sr - pen);
                        m->points[p].worldPointAz = sz - nz * (sr - pen);
                        m->points[p].worldPointBx = wcx;
                        m->points[p].worldPointBy = wcy;
                        m->points[p].worldPointBz = wcz;
                        m->points[p].localPointAx = -nx * (sr - pen);
                        m->points[p].localPointAy = -ny * (sr - pen);
                        m->points[p].localPointAz = -nz * (sr - pen);
                        m->points[p].localPointBx = wcx - bx;
                        m->points[p].localPointBy = wcy - by;
                        m->points[p].localPointBz = wcz - bz;
                    }
                }
            });
        }).wait();
}
void gpu::PhysicsKernel::warmStart() {
    int manifoldCount;
    m_queue.memcpy(&manifoldCount, m_numManifolds, sizeof(int)).wait();
    if (manifoldCount == 0) return;

    DeviceData data = m_data;
    GPUManifold* manifolds = m_manifolds;
    std::size_t mn = static_cast<std::size_t>(manifoldCount);
    std::size_t mxdim = 32;
    std::size_t mydim = (mn + mxdim - 1) / mxdim;

    m_queue.submit([data, manifolds, mn, mxdim, mydim](sycl::handler& h) {
        h.parallel_for(sycl::range<2>(mydim, mxdim),
            [data, manifolds, mn, mxdim](sycl::item<2> item) {
                std::size_t i = item[0] * mxdim + item[1];
                if (i >= mn) return;

                GPUManifold* m = &manifolds[i];
                if (m->numPoints == 0) return;

                int bodyA = m->bodyA;
                int bodyB = m->bodyB;
                float invMassA = data.invMass[bodyA];
                float invMassB = data.invMass[bodyB];

                for (int p = 0; p < m->numPoints; p++) {
                    float cachedImpulse = m->points[p].normalImpulse * 0.5f;
                    m->points[p].normalImpulse = 0.0f;
                    if (cachedImpulse <= 0.0f) continue;

                    float nx = m->points[p].normalX;
                    float ny = m->points[p].normalY;
                    float nz = m->points[p].normalZ;

                    float ix = cachedImpulse * nx;
                    float iy = cachedImpulse * ny;
                    float iz = cachedImpulse * nz;

                    if (data.bodyMode[bodyA] == 1) {
                        atomicAdd(&data.x_vel[bodyA], -ix * invMassA);
                        atomicAdd(&data.y_vel[bodyA], -iy * invMassA);
                        atomicAdd(&data.z_vel[bodyA], -iz * invMassA);
                    }

                    if (data.bodyMode[bodyB] == 1) {
                        atomicAdd(&data.x_vel[bodyB], ix * invMassB);
                        atomicAdd(&data.y_vel[bodyB], iy * invMassB);
                        atomicAdd(&data.z_vel[bodyB], iz * invMassB);
                    }
                }
            });
        }).wait();
}