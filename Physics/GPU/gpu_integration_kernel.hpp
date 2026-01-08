#if !defined(_INTEGRATION_KERNEL_HPP_)
#define _INTEGRATION_KERNEL_HPP_

#include <sycl/sycl.hpp>
#include "gpu_rigid_body.hpp"
#include <iostream>

namespace fungt {
    namespace gpu {

        class IntegrationKernel {
        public:
            // Helper to calculate optimal 2D grid dimensions
            static void calculate2DGrid(int numBodies, int& gridX, int& gridY) {
                // Aim for square-ish grid (better cache locality)
                gridX = static_cast<int>(std::sqrt(numBodies));
                gridY = (numBodies + gridX - 1) / gridX;  // Round up

                // Prefer dimensions that are multiples of 16 (workgroup size)
                gridX = ((gridX + 15) / 16) * 16;
                gridY = ((gridY + 15) / 16) * 16;
            }

            // Run integration on GPU using SYCL
            static void integrate(sycl::queue& q,
                GPURigidBody& state,
                float dt) {

                int numBodies = state.numBodies;
                if (numBodies == 0) return;

                // Calculate 2D grid dimensions
                int gridX, gridY;
                calculate2DGrid(numBodies, gridX, gridY);

                std::cout << "GPU Grid: " << gridX << " x " << gridY
                    << " = " << (gridX * gridY) << " threads for "
                    << numBodies << " bodies" << std::endl;

                // Create SYCL buffers from host vectors
                sycl::buffer<float> posX_buf(state.positions.x.data(), sycl::range<1>(numBodies));
                sycl::buffer<float> posY_buf(state.positions.y.data(), sycl::range<1>(numBodies));
                sycl::buffer<float> posZ_buf(state.positions.z.data(), sycl::range<1>(numBodies));

                sycl::buffer<float> velX_buf(state.velocities.x.data(), sycl::range<1>(numBodies));
                sycl::buffer<float> velY_buf(state.velocities.y.data(), sycl::range<1>(numBodies));
                sycl::buffer<float> velZ_buf(state.velocities.z.data(), sycl::range<1>(numBodies));

                sycl::buffer<float> invMass_buf(state.invMasses.data(), sycl::range<1>(numBodies));

                // Submit kernel to GPU
                q.submit([&](sycl::handler& h) {
                    // Get accessors for reading and writing
                    auto posX = posX_buf.get_access<sycl::access::mode::read_write>(h);
                    auto posY = posY_buf.get_access<sycl::access::mode::read_write>(h);
                    auto posZ = posZ_buf.get_access<sycl::access::mode::read_write>(h);

                    auto velX = velX_buf.get_access<sycl::access::mode::read_write>(h);
                    auto velY = velY_buf.get_access<sycl::access::mode::read_write>(h);
                    auto velZ = velZ_buf.get_access<sycl::access::mode::read_write>(h);

                    auto invMass = invMass_buf.get_access<sycl::access::mode::read>(h);
                    // 2D parallel
                    sycl::range<2> globalSize(gridX, gridY);
                    sycl::range<2> localSize(16,16);
                    // Define the kernel
                    h.parallel_for(sycl::range<2>(globalSize,localSize), [=](sycl::nd_item<2> item) {
                        int y = item.get_global_id(0);
                        int x = item.get_global_id(1);
                        int i = y * gridX + x;
                        // Skip static bodies (invMass == 0)
                        if (invMass[i] == 0.0f) return;

                        // Apply gravity
                        float gravity = -9.81f;
                        velY[i] += gravity * dt;

                        // Integrate position: pos += vel * dt
                        posX[i] += velX[i] * dt;
                        posY[i] += velY[i] * dt;
                        posZ[i] += velZ[i] * dt;
                        });
                    });

                // Wait for kernel to complete
                q.wait();

                // Buffers automatically copy back to host when they go out of scope!
            }

            // CPU reference implementation for validation
            static void integrateCPU(GPURigidBody& state, float dt) {
                for (int i = 0; i < state.numBodies; i++) {
                    // Skip static bodies
                    if (state.invMasses[i] == 0.0f) continue;

                    // Apply gravity
                    float gravity = -9.81f;
                    state.velocities.y[i] += gravity * dt;

                    // Integrate position
                    state.positions.x[i] += state.velocities.x[i] * dt;
                    state.positions.y[i] += state.velocities.y[i] * dt;
                    state.positions.z[i] += state.velocities.z[i] * dt;
                }
            }
        };

    } // namespace gpu
} // namespace fungt

#endif // _INTEGRATION_KERNEL_HPP_