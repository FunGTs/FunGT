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
                // Positions
                float* posX, float* posY, float* posZ,
                // Linear motion
                float* velX, float* velY, float* velZ,
                float* forceX, float* forceY, float* forceZ,
                // Angular motion
                float* angVelX, float* angVelY, float* angVelZ,
                float* torqueX, float* torqueY, float* torqueZ,
                // Orientations
                float* orientW, float* orientX, float* orientY, float* orientZ,
                // Mass properties
                float* invMass,
                float* invInertiaTensor,  // 9 floats per body
                int numBodies,
                float dt)
            {
                if (numBodies == 0) return;

                std::size_t n = static_cast<std::size_t>(numBodies);
                std::size_t xdim = static_cast<std::size_t>(std::ceil(std::sqrt(n)));
                std::size_t ydim = xdim;

                q.submit([&](sycl::handler& h) {
                    h.parallel_for(
                        sycl::range<2>(ydim, xdim),
                        [=](sycl::item<2> item) {

                            std::size_t i = item[0] * xdim + item[1];
                            if (i >= n) return;

                            // Skip static bodies
                            float im = invMass[i];
                            if (im == 0.0f) return;

                            // ========================================
                            // LINEAR MOTION
                            // ========================================

                            // Calculate acceleration: a = F / m = F * invMass
                            float accelX = forceX[i] * im;
                            float accelY = forceY[i] * im;
                            float accelZ = forceZ[i] * im;

                            // Update velocity: v += a * dt
                            velX[i] += accelX * dt;
                            velY[i] += accelY * dt;
                            velZ[i] += accelZ * dt;

                            // Update position: p += v * dt
                            posX[i] += velX[i] * dt;
                            posY[i] += velY[i] * dt;
                            posZ[i] += velZ[i] * dt;

                            // Clear forces for next frame
                            forceX[i] = 0.0f;
                            forceY[i] = 0.0f;
                            forceZ[i] = 0.0f;

                            // ========================================
                            // ANGULAR MOTION
                            // ========================================

                            // Get inverse inertia tensor (3x3 matrix)
                            int tensorIdx = i * 9;
                            float I00 = invInertiaTensor[tensorIdx + 0];
                            float I01 = invInertiaTensor[tensorIdx + 1];
                            float I02 = invInertiaTensor[tensorIdx + 2];
                            float I10 = invInertiaTensor[tensorIdx + 3];
                            float I11 = invInertiaTensor[tensorIdx + 4];
                            float I12 = invInertiaTensor[tensorIdx + 5];
                            float I20 = invInertiaTensor[tensorIdx + 6];
                            float I21 = invInertiaTensor[tensorIdx + 7];
                            float I22 = invInertiaTensor[tensorIdx + 8];

                            // Calculate angular acceleration: α = I^-1 * τ
                            float tx = torqueX[i];
                            float ty = torqueY[i];
                            float tz = torqueZ[i];

                            float angAccelX = I00 * tx + I01 * ty + I02 * tz;
                            float angAccelY = I10 * tx + I11 * ty + I12 * tz;
                            float angAccelZ = I20 * tx + I21 * ty + I22 * tz;

                            // Update angular velocity: ω += α * dt
                            float avx = angVelX[i] + angAccelX * dt;
                            float avy = angVelY[i] + angAccelY * dt;
                            float avz = angVelZ[i] + angAccelZ * dt;

                            angVelX[i] = avx;
                            angVelY[i] = avy;
                            angVelZ[i] = avz;

                            // Clear torques
                            torqueX[i] = 0.0f;
                            torqueY[i] = 0.0f;
                            torqueZ[i] = 0.0f;

                            // ========================================
                            // ORIENTATION UPDATE (Quaternion integration)
                            // ========================================

                            float angSpeed = sycl::sqrt(avx * avx + avy * avy + avz * avz);

                            if (angSpeed > 1e-6f) {
                                // Normalize angular velocity to get axis
                                float invSpeed = 1.0f / angSpeed;
                                float axisX = avx * invSpeed;
                                float axisY = avy * invSpeed;
                                float axisZ = avz * invSpeed;

                                // Angle of rotation
                                float angle = angSpeed * dt;
                                float halfAngle = angle * 0.5f;
                                float sinHalf = sycl::sin(halfAngle);
                                float cosHalf = sycl::cos(halfAngle);

                                // Delta quaternion from axis-angle
                                float dw = cosHalf;
                                float dx = axisX * sinHalf;
                                float dy = axisY * sinHalf;
                                float dz = axisZ * sinHalf;

                                // Current orientation
                                float qw = orientW[i];
                                float qx = orientX[i];
                                float qy = orientY[i];
                                float qz = orientZ[i];

                                // Quaternion multiplication: result = delta * current
                                float nw = dw * qw - dx * qx - dy * qy - dz * qz;
                                float nx = dw * qx + dx * qw + dy * qz - dz * qy;
                                float ny = dw * qy - dx * qz + dy * qw + dz * qx;
                                float nz = dw * qz + dx * qy - dy * qx + dz * qw;

                                // Normalize to prevent drift
                                float len = sycl::sqrt(nw * nw + nx * nx + ny * ny + nz * nz);
                                if (len > 1e-6f) {
                                    float invLen = 1.0f / len;
                                    orientW[i] = nw * invLen;
                                    orientX[i] = nx * invLen;
                                    orientY[i] = ny * invLen;
                                    orientZ[i] = nz * invLen;
                                }
                            }
                        });
                    });
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