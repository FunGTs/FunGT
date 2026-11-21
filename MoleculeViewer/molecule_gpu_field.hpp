#ifndef FUNGT_MOLECULE_GPU_FIELD_HPP
#define FUNGT_MOLECULE_GPU_FIELD_HPP

#include <sycl/sycl.hpp>
#include <cstddef>
#include <limits>

namespace fungt {

struct GPUAtom {
    float x, y, z, radius;
    float r, g, b;
};

struct GPUBond {
    float ax, ay, az;
    float bx, by, bz;
    float radius;
};

// Arrays must be device-accessible and remain valid until grid sampling finishes.
inline auto makeMoleculeGPUField(const GPUAtom* atoms, std::size_t atomCount,
                                 const GPUBond* bonds, std::size_t bondCount,
                                 float blendK) {
    const float initialDistance = std::numeric_limits<float>::max();
    return [=](float x, float y, float z) -> float {
        float d = initialDistance;

        for (std::size_t i = 0; i < atomCount; ++i) {
            float dx = x - atoms[i].x;
            float dy = y - atoms[i].y;
            float dz = z - atoms[i].z;
            float sd = sycl::sqrt(dx * dx + dy * dy + dz * dz) - atoms[i].radius;
            if (blendK <= 0.0f) {
                d = sycl::fmin(d, sd);
            } else {
                float h = sycl::fmax(blendK - sycl::fabs(d - sd), 0.0f) / blendK;
                d = sycl::fmin(d, sd) - h * h * blendK * 0.25f;
            }
        }

        for (std::size_t i = 0; i < bondCount; ++i) {
            float px = x - bonds[i].ax;
            float py = y - bonds[i].ay;
            float pz = z - bonds[i].az;
            float bx = bonds[i].bx - bonds[i].ax;
            float by = bonds[i].by - bonds[i].ay;
            float bz = bonds[i].bz - bonds[i].az;
            float lengthSquared = bx * bx + by * by + bz * bz;
            float t = lengthSquared > 0.0f
                ? sycl::clamp((px * bx + py * by + pz * bz) / lengthSquared, 0.0f, 1.0f)
                : 0.0f;
            float dx = px - bx * t;
            float dy = py - by * t;
            float dz = pz - bz * t;
            float cd = sycl::sqrt(dx * dx + dy * dy + dz * dz) - bonds[i].radius;
            if (blendK <= 0.0f) {
                d = sycl::fmin(d, cd);
            } else {
                float h = sycl::fmax(blendK - sycl::fabs(d - cd), 0.0f) / blendK;
                d = sycl::fmin(d, cd) - h * h * blendK * 0.25f;
            }
        }

        return d;
    };
}

} // namespace fungt

#endif
