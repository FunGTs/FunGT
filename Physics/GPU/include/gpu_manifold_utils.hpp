// gpu_manifold_utils.hpp

#if !defined(_GPU_MANIFOLD_UTILS_HPP_)
#define _GPU_MANIFOLD_UTILS_HPP_

#include "gpu_manifold_contacts.hpp"
#include <funlib/funlib.hpp>

namespace gpu {

    // Device function - find existing manifold for body pair
    inline int findManifold(
        int* pairToManifold,
        GPUManifold* manifolds,
        int hashTableSize,
        int bodyA,
        int bodyB)
    {
        // Create pair key
        int lo = (bodyA < bodyB) ? bodyA : bodyB;
        int hi = (bodyA < bodyB) ? bodyB : bodyA;
        int key = (lo << 16) | hi;

        // Hash to starting slot
        int hashIndex = key % hashTableSize;

        // Linear probe
        for (int i = 0; i < hashTableSize; i++) {
            int slot = (hashIndex + i) % hashTableSize;
            int manifoldIdx = pairToManifold[slot];

            // Empty slot - not found
            if (manifoldIdx == -1) {
                return -1;
            }

            // Check if match
            GPUManifold* m = &manifolds[manifoldIdx];
            if ((m->bodyA == bodyA && m->bodyB == bodyB) ||
                (m->bodyA == bodyB && m->bodyB == bodyA)) {
                return manifoldIdx;
            }
        }

        return -1;
    }

    // Device function - create new manifold for body pair
    inline int createManifold(
        int* pairToManifold,
        GPUManifold* manifolds,
        int* numManifolds,
        int hashTableSize,
        int maxManifolds,
        int bodyA,
        int bodyB)
    {
        // Atomic grab next slot
        sycl::atomic_ref<int,
            sycl::memory_order::relaxed,
            sycl::memory_scope::device,
            sycl::access::address_space::global_space> atomicCount(*numManifolds);

        int manifoldIdx = atomicCount.fetch_add(1);

        if (manifoldIdx >= maxManifolds) {
            atomicCount.fetch_sub(1);
            return -1;
        }

        // Initialize manifold
        GPUManifold* m = &manifolds[manifoldIdx];
        m->bodyA = bodyA;
        m->bodyB = bodyB;
        m->numPoints = 0;

        // Insert into hash table
        int lo = (bodyA < bodyB) ? bodyA : bodyB;
        int hi = (bodyA < bodyB) ? bodyB : bodyA;
        int key = (lo << 16) | hi;
        int hashIndex = key % hashTableSize;

        for (int i = 0; i < hashTableSize; i++) {
            int slot = (hashIndex + i) % hashTableSize;

            sycl::atomic_ref<int,
                sycl::memory_order::relaxed,
                sycl::memory_scope::device,
                sycl::access::address_space::global_space> atomicSlot(pairToManifold[slot]);

            int expected = -1;
            if (atomicSlot.compare_exchange_strong(expected, manifoldIdx)) {
                return manifoldIdx;
            }
        }

        // Hash table full
        atomicCount.fetch_sub(1);
        return -1;
    }
    inline void addContactToManifold(
        GPUManifold* manifolds,
        int manifoldIdx,
        float localAx, float localAy, float localAz,
        float localBx, float localBy, float localBz,
        float worldAx, float worldAy, float worldAz,
        float worldBx, float worldBy, float worldBz,
        float normalX, float normalY, float normalZ,
        float penetration)
    {
        GPUManifold* m = &manifolds[manifoldIdx];

        sycl::atomic_ref<int,
            sycl::memory_order::relaxed,
            sycl::memory_scope::device,
            sycl::access::address_space::global_space> atomicNumPoints(m->numPoints);

        // Try to grab a slot
        while (true) {
            int current = atomicNumPoints.load();

            if (current < 4) {
                // Room available - try to claim next slot
                if (atomicNumPoints.compare_exchange_strong(current, current + 1)) {
                    // Got slot 'current' - write contact
                    m->points[current].localPointAx = localAx;
                    m->points[current].localPointAy = localAy;
                    m->points[current].localPointAz = localAz;
                    m->points[current].localPointBx = localBx;
                    m->points[current].localPointBy = localBy;
                    m->points[current].localPointBz = localBz;
                    m->points[current].worldPointAx = worldAx;
                    m->points[current].worldPointAy = worldAy;
                    m->points[current].worldPointAz = worldAz;
                    m->points[current].worldPointBx = worldBx;
                    m->points[current].worldPointBy = worldBy;
                    m->points[current].worldPointBz = worldBz;
                    m->points[current].normalX = normalX;
                    m->points[current].normalY = normalY;
                    m->points[current].normalZ = normalZ;
                    m->points[current].penetration = penetration;
                    m->points[current].normalImpulse = 0.0f;
                    m->points[current].tangentImpulse1 = 0.0f;
                    m->points[current].tangentImpulse2 = 0.0f;
                    m->points[current].lifeTime = 0;
                    return;
                }
                // compare_exchange failed - another thread got it, loop and retry
            }
            else {
                // Full (numPoints >= 4) - try to replace shallowest
                // Use numPoints = 5 as "locked" state
                if (atomicNumPoints.compare_exchange_strong(current, 5)) {
                    // We have the lock - find shallowest
                    int replaceIdx = 0;
                    float minPen = m->points[0].penetration;

                    for (int i = 1; i < 4; i++) {
                        if (m->points[i].penetration < minPen) {
                            minPen = m->points[i].penetration;
                            replaceIdx = i;
                        }
                    }

                    // Only replace if new contact is deeper
                    if (penetration > minPen) {
                        m->points[replaceIdx].localPointAx = localAx;
                        m->points[replaceIdx].localPointAy = localAy;
                        m->points[replaceIdx].localPointAz = localAz;
                        m->points[replaceIdx].localPointBx = localBx;
                        m->points[replaceIdx].localPointBy = localBy;
                        m->points[replaceIdx].localPointBz = localBz;
                        m->points[replaceIdx].worldPointAx = worldAx;
                        m->points[replaceIdx].worldPointAy = worldAy;
                        m->points[replaceIdx].worldPointAz = worldAz;
                        m->points[replaceIdx].worldPointBx = worldBx;
                        m->points[replaceIdx].worldPointBy = worldBy;
                        m->points[replaceIdx].worldPointBz = worldBz;
                        m->points[replaceIdx].normalX = normalX;
                        m->points[replaceIdx].normalY = normalY;
                        m->points[replaceIdx].normalZ = normalZ;
                        m->points[replaceIdx].penetration = penetration;
                        m->points[replaceIdx].normalImpulse = 0.0f;
                        m->points[replaceIdx].tangentImpulse1 = 0.0f;
                        m->points[replaceIdx].tangentImpulse2 = 0.0f;
                        m->points[replaceIdx].lifeTime = 0;
                    }

                    // Release lock - back to 4
                    atomicNumPoints.store(4);
                    return;
                }
                // compare_exchange failed - someone else has lock or state changed
                // If locked (5), spin wait
                if (current == 5) {
                    continue;  // spin until unlocked
                }
                // State changed to < 4 somehow, loop and retry
            }
        }
    }

} // namespace gpu

#endif