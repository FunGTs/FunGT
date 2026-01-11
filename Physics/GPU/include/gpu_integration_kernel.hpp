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
            // USM version - operates directly on device pointers
            static void integrate(
                sycl::queue& q,
                float* posX_gpu, float* posY_gpu, float* posZ_gpu,
                float* velX_gpu, float* velY_gpu, float* velZ_gpu,
                float* invMass_gpu,
                int numBodies,
                float dt)
            {
                if (numBodies == 0) return;

                // Calculate 2D grid dimensions
                std::size_t n = static_cast<std::size_t>(numBodies);
                std::size_t xdim = static_cast<std::size_t>(std::ceil(std::sqrt(n)));
                std::size_t ydim = xdim;

                // Submit kernel
                q.submit([&](sycl::handler& h) {
                    h.parallel_for(
                        sycl::range<2>(ydim, xdim),
                        [=](sycl::item<2> item) {

                            std::size_t i = item[0] * xdim + item[1];
                            if (i >= n) return;

                            // Skip static bodies
                            if (invMass_gpu[i] == 0.0f) return;

                            // Apply gravity
                            const float gravity = -9.81f;
                            velY_gpu[i] += gravity * dt;

                            // Integrate position: pos += vel * dt
                            posX_gpu[i] += velX_gpu[i] * dt;
                            posY_gpu[i] += velY_gpu[i] * dt;
                            posZ_gpu[i] += velZ_gpu[i] * dt;
                        });
                    });

                // Don't wait
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