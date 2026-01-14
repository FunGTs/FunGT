#ifndef _PARTICLE_DEMOS_H_
#define _PARTICLE_DEMOS_H_

#include <vector>
#include <string>
#include <cmath>
#include <cstdlib>
#include <funlib/funlib.hpp>
#include "perlin_noise_gpu.hpp"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace fgt {

    // Demo names
    static std::vector<std::string> demoNames = {
        "Spiral Explosion",
        "Black Hole",
        "Vortex",
        "Firework",
        "Wave Field",
        "Smoke Plume"
    };

    // Update lambdas (GPU-compatible, no std::function)
    auto spiralExplosionUpdate = [](flib::Particle<float>& p, float dt) {
        constexpr float central_force_strength = 0.5f;
        constexpr float spiral_speed = 0.5f;
        float distance = std::sqrt(p.position[0] * p.position[0] + p.position[1] * p.position[1]);
        float radial_force = central_force_strength / (distance + 0.1f);
        p.velocity[0] += radial_force * p.position[0] * dt;
        p.velocity[1] += radial_force * p.position[1] * dt;
        p.velocity[0] -= spiral_speed * p.position[1] * dt;
        p.velocity[1] += spiral_speed * p.position[0] * dt;
        p.position[0] += p.velocity[0] * dt;
        p.position[1] += p.velocity[1] * dt;
        p.position[2] += p.velocity[2] * dt;
    };
    auto blackHoleUpdate = [](flib::Particle<float>& p, float dt) {
        constexpr float gravity = 5.0f;
        constexpr float event_horizon = 0.1f;
        float dx = -p.position[0];
        float dy = -p.position[1];
        float dz = -p.position[2];
        float dist_sq = dx * dx + dy * dy + dz * dz;
        float dist = std::sqrt(dist_sq);
        if (dist > event_horizon) {
            float force = gravity / dist_sq;
            p.velocity[0] += force * (dx / dist) * dt;
            p.velocity[1] += force * (dy / dist) * dt;
            p.velocity[2] += force * (dz / dist) * dt;
        }
        else {
            p.velocity[0] *= 0.9f;
            p.velocity[1] *= 0.9f;
            p.velocity[2] *= 0.9f;
        }
        p.position[0] += p.velocity[0] * dt;
        p.position[1] += p.velocity[1] * dt;
        p.position[2] += p.velocity[2] * dt;
        };

    auto vortexUpdate = [](flib::Particle<float>& p, float dt) {
        constexpr float vortex_strength = 2.0f;
        constexpr float lift_force = 0.3f;
        constexpr float damping = 0.98f;
        float r = std::sqrt(p.position[0] * p.position[0] + p.position[1] * p.position[1]);
        float tangential = vortex_strength / (r + 0.1f);
        p.velocity[0] += -tangential * p.position[1] * dt;
        p.velocity[1] += tangential * p.position[0] * dt;
        p.velocity[0] -= 0.5f * p.position[0] * dt;
        p.velocity[1] -= 0.5f * p.position[1] * dt;
        p.velocity[2] += lift_force * dt;
        p.velocity[0] *= damping;
        p.velocity[1] *= damping;
        p.position[0] += p.velocity[0] * dt;
        p.position[1] += p.velocity[1] * dt;
        p.position[2] += p.velocity[2] * dt;
    };

    auto fireworkUpdate = [](flib::Particle<float>& p, float dt) {
        constexpr float gravity = -2.0f;
        constexpr float drag = 0.99f;
        p.velocity[2] += gravity * dt;
        p.velocity[0] *= drag;
        p.velocity[1] *= drag;
        p.velocity[2] *= drag;
        p.position[0] += p.velocity[0] * dt;
        p.position[1] += p.velocity[1] * dt;
        p.position[2] += p.velocity[2] * dt;
        };

    auto waveUpdate = [](flib::Particle<float>& p, float dt) {
        // Remove static time - just use particle position as time offset
        constexpr float wave_speed = 2.0f;
        constexpr float amplitude = 0.5f;

        // Use particle's own position as time-varying component
        float time_offset = p.position[0] + p.position[1];
        float wave = std::sin(p.position[0] * wave_speed + time_offset) *
            std::cos(p.position[1] * wave_speed + time_offset);

        p.velocity[2] = wave * amplitude;
        p.position[2] += p.velocity[2] * dt;
    };

    auto smokeUpdate = [](flib::Particle<float>& p, float dt) {
        // Remove static time - use particle Z position as time proxy
        constexpr float buoyancy = 0.8f;
        constexpr float drag = 0.98f;
        constexpr float noise_scale = 1.5f;
        constexpr float turbulence_strength = 0.4f;

        // Use Z position as evolving time component (particles rise = time passes)
        float time = p.position[2] * 0.1f;

        float noise_x = PerlinGPU::turbulence3d(
            p.position[0] * noise_scale,
            p.position[1] * noise_scale,
            p.position[2] * noise_scale + time,
            4, 0.5f
        );

        float noise_y = PerlinGPU::turbulence3d(
            p.position[0] * noise_scale + 100.0f,
            p.position[1] * noise_scale,
            p.position[2] * noise_scale + time,
            4, 0.5f
        );

        float noise_z = PerlinGPU::turbulence3d(
            p.position[0] * noise_scale,
            p.position[1] * noise_scale + 100.0f,
            p.position[2] * noise_scale + time,
            4, 0.5f
        );

        noise_x = (noise_x - 0.5f) * 2.0f;
        noise_y = (noise_y - 0.5f) * 2.0f;
        noise_z = (noise_z - 0.5f) * 2.0f;

        p.velocity[0] += noise_x * turbulence_strength * dt;
        p.velocity[1] += noise_y * turbulence_strength * dt;
        p.velocity[2] += (buoyancy + noise_z * 0.2f) * dt;

        p.velocity[0] *= drag;
        p.velocity[1] *= drag;
        p.velocity[2] *= drag;

        p.position[0] += p.velocity[0] * dt;
        p.position[1] += p.velocity[1] * dt;
        p.position[2] += p.velocity[2] * dt;

        if (p.position[2] > 3.0f) {
            // Hash-based pseudo-random using particle's unique position
            unsigned int seed = *reinterpret_cast<unsigned int*>(&p.position[0]) ^
                *reinterpret_cast<unsigned int*>(&p.position[1]);

            // Simple hash function
            seed = seed * 747796405u + 2891336453u;
            float rand_x = ((seed >> 16) & 0xFFFF) / 65535.0f;

            seed = seed * 747796405u + 2891336453u;
            float rand_y = ((seed >> 16) & 0xFFFF) / 65535.0f;

            p.position[2] = -2.0f;
            p.position[0] = (rand_x - 0.5f) * 0.6f;
            p.position[1] = (rand_y - 0.5f) * 0.6f;
        }
    };

    // Init functions (CPU-side, can use std::function)
    using InitFunc = std::function<void(std::vector<flib::Particle<float>>&)>;

    static std::vector<InitFunc> demoInits = {
        // Spiral Explosion
        [](std::vector<flib::Particle<float>>& particles) {
            float radius = 0.5f;
            for (std::size_t i = 0; i < particles.size(); ++i) {
                float angle = static_cast<float>(rand()) / RAND_MAX * 2.0f * M_PI;
                float distance = static_cast<float>(rand()) / RAND_MAX * radius;
                particles[i].position[0] = distance * std::cos(angle);
                particles[i].position[1] = distance * std::sin(angle);
                particles[i].position[2] = 0.0f;
                particles[i].velocity[0] = 0.0f;
                particles[i].velocity[1] = 0.0f;
                particles[i].velocity[2] = 0.0f;
            }
        },
        // Black Hole
        [](std::vector<flib::Particle<float>>& particles) {
            float radius = 3.0f;
            for (std::size_t i = 0; i < particles.size(); ++i) {
                float theta = static_cast<float>(rand()) / RAND_MAX * 2.0f * M_PI;
                float phi = std::acos(2.0f * (static_cast<float>(rand()) / RAND_MAX) - 1.0f);
                float r = std::pow(static_cast<float>(rand()) / RAND_MAX, 1.0f / 3.0f) * radius;
                particles[i].position[0] = r * std::sin(phi) * std::cos(theta);
                particles[i].position[1] = r * std::sin(phi) * std::sin(theta);
                particles[i].position[2] = r * std::cos(phi);
                particles[i].velocity[0] = -0.5f * particles[i].position[1];
                particles[i].velocity[1] = 0.5f * particles[i].position[0];
                particles[i].velocity[2] = 0.0f;
            }
        },
        // Vortex
        [](std::vector<flib::Particle<float>>& particles) {
            float radius = 2.0f;
            float height = 3.0f;
            for (std::size_t i = 0; i < particles.size(); ++i) {
                float angle = static_cast<float>(rand()) / RAND_MAX * 2.0f * M_PI;
                float distance = static_cast<float>(rand()) / RAND_MAX * radius;
                float z = (static_cast<float>(rand()) / RAND_MAX - 0.5f) * height;
                particles[i].position[0] = distance * std::cos(angle);
                particles[i].position[1] = distance * std::sin(angle);
                particles[i].position[2] = z;
                particles[i].velocity[0] = 0.0f;
                particles[i].velocity[1] = 0.0f;
                particles[i].velocity[2] = 0.0f;
            }
        },
        // Firework
        [](std::vector<flib::Particle<float>>& particles) {
            for (std::size_t i = 0; i < particles.size(); ++i) {
                particles[i].position[0] = 0.0f;
                particles[i].position[1] = 0.0f;
                particles[i].position[2] = 0.0f;
                float theta = static_cast<float>(rand()) / RAND_MAX * 2.0f * M_PI;
                float phi = std::acos(2.0f * (static_cast<float>(rand()) / RAND_MAX) - 1.0f);
                float speed = 2.0f + (static_cast<float>(rand()) / RAND_MAX) * 1.0f;
                particles[i].velocity[0] = speed * std::sin(phi) * std::cos(theta);
                particles[i].velocity[1] = speed * std::sin(phi) * std::sin(theta);
                particles[i].velocity[2] = speed * std::cos(phi);
            }
        },
        // Wave Field
        [](std::vector<flib::Particle<float>>& particles) {
            float grid_size = 4.0f;
            int grid_dim = std::sqrt(particles.size());
            for (std::size_t i = 0; i < particles.size(); ++i) {
                int x = i % grid_dim;
                int y = i / grid_dim;
                particles[i].position[0] = (x / (float)grid_dim - 0.5f) * grid_size;
                particles[i].position[1] = (y / (float)grid_dim - 0.5f) * grid_size;
                particles[i].position[2] = 0.0f;
                particles[i].velocity[0] = 0.0f;
                particles[i].velocity[1] = 0.0f;
                particles[i].velocity[2] = 0.0f;
            }
        },
        // Smoke Plume
        [](std::vector<flib::Particle<float>>& particles) {
            float radius = 0.3f;
            for (std::size_t i = 0; i < particles.size(); ++i) {
                float angle = static_cast<float>(rand()) / RAND_MAX * 2.0f * M_PI;
                float distance = static_cast<float>(rand()) / RAND_MAX * radius;
                particles[i].position[0] = distance * std::cos(angle);
                particles[i].position[1] = distance * std::sin(angle);
                particles[i].position[2] = -2.0f;
                particles[i].velocity[0] = (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 0.2f;
                particles[i].velocity[1] = (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 0.2f;
                particles[i].velocity[2] = 1.0f + (static_cast<float>(rand()) / RAND_MAX) * 0.5f;
            }
        }
    };

} // namespace fgt

#endif // _PARTICLE_DEMOS_H_