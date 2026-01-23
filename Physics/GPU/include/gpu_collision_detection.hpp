// gpu_collision_detection.hpp

#if !defined(_GPU_COLLISION_DETECTION_HPP_)
#define _GPU_COLLISION_DETECTION_HPP_

#include "gpu_contact.hpp"
#include "gpu_device_data.hpp"

namespace gpu {

    // Helper: check if sphere center is inside box
    inline bool sphereCenterInsideBox(
        float relX, float relY, float relZ,
        float halfX, float halfY, float halfZ)
    {
        return (sycl::fabs(relX) <= halfX) &&
            (sycl::fabs(relY) <= halfY) &&
            (sycl::fabs(relZ) <= halfZ);
    }

    // Returns true if collision detected, fills out contact data
    inline bool SphereBoxCollision(
        // Sphere data
        float sphereX, float sphereY, float sphereZ,
        float sphereRadius,
        // Box data
        float boxX, float boxY, float boxZ,
        float boxHalfX, float boxHalfY, float boxHalfZ,
        bool boxIsStatic,
        // Output
        float& normalX, float& normalY, float& normalZ,
        float& penetration,
        float& worldPointAx, float& worldPointAy, float& worldPointAz,
        float& worldPointBx, float& worldPointBy, float& worldPointBz,
        float& localPointAx, float& localPointAy, float& localPointAz,
        float& localPointBx, float& localPointBy, float& localPointBz)
    {
        // Find closest point on box to sphere center
        float relX = sphereX - boxX;
        float relY = sphereY - boxY;
        float relZ = sphereZ - boxZ;

        // Clamp to box bounds
        float closestX = sycl::fmax(-boxHalfX, sycl::fmin(boxHalfX, relX));
        float closestY = sycl::fmax(-boxHalfY, sycl::fmin(boxHalfY, relY));
        float closestZ = sycl::fmax(-boxHalfZ, sycl::fmin(boxHalfZ, relZ));

        // World space closest point
        float worldClosestX = boxX + closestX;
        float worldClosestY = boxY + closestY;
        float worldClosestZ = boxZ + closestZ;

        // Direction from closest point to sphere
        float dirX = sphereX - worldClosestX;
        float dirY = sphereY - worldClosestY;
        float dirZ = sphereZ - worldClosestZ;
        float distSq = dirX * dirX + dirY * dirY + dirZ * dirZ;
        float distance = sycl::sqrt(distSq);

        // Check if center is inside box
        bool insideBox = sphereCenterInsideBox(relX, relY, relZ,
            boxHalfX, boxHalfY, boxHalfZ);

        // No collision?
        if (!insideBox && distance >= sphereRadius) {
            return false;
        }

        // Collision detected - compute contact
        if (insideBox) {
            // Sphere center INSIDE box - find nearest face
            float dx = boxHalfX - sycl::fabs(relX);
            float dy = boxHalfY - sycl::fabs(relY);
            float dz = boxHalfZ - sycl::fabs(relZ);

            if (dx < dy && dx < dz) {
                // X axis nearest
                normalX = (relX > 0) ? 1.0f : -1.0f;
                normalY = 0.0f;
                normalZ = 0.0f;
                penetration = sphereRadius + dx;
            }
            else if (dy < dz) {
                // Y axis nearest
                if (boxIsStatic) {
                    // Ground - always push UP
                    normalX = 0.0f;
                    normalY = 1.0f;
                    normalZ = 0.0f;
                }
                else {
                    float distToTop = boxHalfY - relY;
                    float distToBottom = boxHalfY + relY;
                    normalX = 0.0f;
                    normalY = (distToTop < distToBottom) ? 1.0f : -1.0f;
                    normalZ = 0.0f;
                }
                penetration = sphereRadius + dy;
            }
            else {
                // Z axis nearest
                normalX = 0.0f;
                normalY = 0.0f;
                normalZ = (relZ > 0) ? 1.0f : -1.0f;
                penetration = sphereRadius + dz;
            }
        }
        else {
            // Sphere surface touching from outside
            if (distance < 0.0001f) {
                normalX = 0.0f;
                normalY = 1.0f;
                normalZ = 0.0f;
                penetration = sphereRadius;
            }
            else {
                normalX = dirX / distance;
                normalY = dirY / distance;
                normalZ = dirZ / distance;
                penetration = sphereRadius - distance;

                // HACK: If ground and normal points down, flip it
                if (boxIsStatic && normalY < 0) {
                    normalY = -normalY;
                }
            }
        }

        // World contact points
        worldPointAx = sphereX - normalX * (sphereRadius - penetration);
        worldPointAy = sphereY - normalY * (sphereRadius - penetration);
        worldPointAz = sphereZ - normalZ * (sphereRadius - penetration);
        worldPointBx = worldClosestX;
        worldPointBy = worldClosestY;
        worldPointBz = worldClosestZ;

        // Local contact points (relative to body centers)
        localPointAx = -normalX * (sphereRadius - penetration);
        localPointAy = -normalY * (sphereRadius - penetration);
        localPointAz = -normalZ * (sphereRadius - penetration);
        localPointBx = worldClosestX - boxX;
        localPointBy = worldClosestY - boxY;
        localPointBz = worldClosestZ - boxZ;

        return true;
    }

} // namespace gpu

#endif