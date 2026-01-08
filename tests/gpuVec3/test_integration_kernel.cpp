#include <gtest/gtest.h>
#include <sycl/sycl.hpp>
#include "Physics/GPU/gpu_rigid_body.hpp"
#include "Physics/GPU/gpu_rigid_body_builder.hpp"
#include "Physics/GPU/kernels/integration_kernel.hpp"
#include <cmath>

class IntegrationKernelTest : public ::testing::Test {
protected:
    sycl::queue q;
    
    void SetUp() override {
        // Print device info
        auto device = q.get_device();
        std::cout << "Running on: " 
                  << device.get_info<sycl::info::device::name>() 
                  << std::endl;
    }
};

TEST_F(IntegrationKernelTest, DeviceAvailable) {
    // Just verify SYCL queue is created
    auto device = q.get_device();
    EXPECT_TRUE(device.is_gpu() || device.is_cpu());
}

TEST_F(IntegrationKernelTest, SingleSphereFreeFall) {
    GPURigidBody state = GPURigidBodyBuilder::createState();
    
    // Add one sphere at height 10
    GPURigidBodyBuilder::addSphere(state, fungt::Vec3(0, 10, 0), 1.0f, 1.0f);
    
    float dt = 0.016f; // ~60 FPS
    
    // Run GPU integration
    fungt::gpu::IntegrationKernel::integrate(q, state, dt);
    
    // Check results
    float posY = state.positions.y[0];
    float velY = state.velocities.y[0];
    
    // Should have fallen due to gravity
    EXPECT_LT(posY, 10.0f);  // Position decreased
    EXPECT_LT(velY, 0.0f);   // Velocity is negative (falling)
    
    // Verify physics: v = gt
    float expectedVel = -9.81f * dt;
    EXPECT_NEAR(velY, expectedVel, 0.01f);
    
    // Verify physics: y = y0 + v*t
    float expectedPos = 10.0f + expectedVel * dt;
    EXPECT_NEAR(posY, expectedPos, 0.01f);
}

TEST_F(IntegrationKernelTest, StaticBodyDoesNotMove) {
    GPURigidBody state = GPURigidBodyBuilder::createState();
    
    // Add static box (mass = 0)
    GPURigidBodyBuilder::addBox(state, fungt::Vec3(0, 0, 0), 
                                fungt::Vec3(10, 1, 10), 0.0f);
    
    float dt = 0.016f;
    
    // Store initial position
    float initialY = state.positions.y[0];
    
    // Run GPU integration
    fungt::gpu::IntegrationKernel::integrate(q, state, dt);
    
    // Static body should NOT move
    EXPECT_FLOAT_EQ(state.positions.y[0], initialY);
    EXPECT_FLOAT_EQ(state.velocities.y[0], 0.0f);
}

TEST_F(IntegrationKernelTest, MultipleSpheresIndependent) {
    GPURigidBody state = GPURigidBodyBuilder::createState();
    
    // Add 3 spheres at different heights
    GPURigidBodyBuilder::addSphere(state, fungt::Vec3(0, 10, 0), 1.0f, 1.0f);
    GPURigidBodyBuilder::addSphere(state, fungt::Vec3(0, 20, 0), 1.0f, 1.0f);
    GPURigidBodyBuilder::addSphere(state, fungt::Vec3(0, 30, 0), 1.0f, 1.0f);
    
    float dt = 0.016f;
    
    // Run GPU integration
    fungt::gpu::IntegrationKernel::integrate(q, state, dt);
    
    // All should fall independently
    EXPECT_LT(state.positions.y[0], 10.0f);
    EXPECT_LT(state.positions.y[1], 20.0f);
    EXPECT_LT(state.positions.y[2], 30.0f);
    
    // All should have same velocity (same gravity)
    float vel0 = state.velocities.y[0];
    float vel1 = state.velocities.y[1];
    float vel2 = state.velocities.y[2];
    
    EXPECT_NEAR(vel0, vel1, 0.0001f);
    EXPECT_NEAR(vel1, vel2, 0.0001f);
}

TEST_F(IntegrationKernelTest, GPUMatchesCPU) {
    // Create two identical states
    GPURigidBody gpuState = GPURigidBodyBuilder::createState();
    GPURigidBody cpuState = GPURigidBodyBuilder::createState();
    
    // Add same bodies to both
    for (int i = 0; i < 10; i++) {
        float y = 5.0f + i * 2.0f;
        GPURigidBodyBuilder::addSphere(gpuState, fungt::Vec3(0, y, 0), 1.0f, 1.0f);
        GPURigidBodyBuilder::addSphere(cpuState, fungt::Vec3(0, y, 0), 1.0f, 1.0f);
    }
    
    float dt = 0.016f;
    
    // Run on GPU
    fungt::gpu::IntegrationKernel::integrate(q, gpuState, dt);
    
    // Run on CPU
    fungt::gpu::IntegrationKernel::integrateCPU(cpuState, dt);
    
    // Results should match EXACTLY
    for (int i = 0; i < 10; i++) {
        EXPECT_FLOAT_EQ(gpuState.positions.x[i], cpuState.positions.x[i]);
        EXPECT_FLOAT_EQ(gpuState.positions.y[i], cpuState.positions.y[i]);
        EXPECT_FLOAT_EQ(gpuState.positions.z[i], cpuState.positions.z[i]);
        
        EXPECT_FLOAT_EQ(gpuState.velocities.x[i], cpuState.velocities.x[i]);
        EXPECT_FLOAT_EQ(gpuState.velocities.y[i], cpuState.velocities.y[i]);
        EXPECT_FLOAT_EQ(gpuState.velocities.z[i], cpuState.velocities.z[i]);
    }
}

TEST_F(IntegrationKernelTest, MultipleTimeSteps) {
    GPURigidBody state = GPURigidBodyBuilder::createState();
    
    // Add sphere at height 100
    GPURigidBodyBuilder::addSphere(state, fungt::Vec3(0, 100, 0), 1.0f, 1.0f);
    
    float dt = 0.016f;
    float initialY = 100.0f;
    
    // Run 10 timesteps
    for (int step = 0; step < 10; step++) {
        fungt::gpu::IntegrationKernel::integrate(q, state, dt);
    }
    
    // Should have fallen significantly
    float finalY = state.positions.y[0];
    EXPECT_LT(finalY, initialY);
    EXPECT_GT(finalY, 0.0f);  // Should not have gone through ground yet
    
    // Velocity should be increasing (accelerating downward)
    EXPECT_LT(state.velocities.y[0], -9.81f * dt);
}
