// gpu_contact.hpp

#if !defined(_GPU_CONTACT_HPP_)
#define _GPU_CONTACT_HPP_

namespace gpu {

    struct GPUContactPoint {
        // Local space (for persistence)
        float localPointAx, localPointAy, localPointAz;
        float localPointBx, localPointBy, localPointBz;

        // World space (recalculated each frame)
        float worldPointAx, worldPointAy, worldPointAz;
        float worldPointBx, worldPointBy, worldPointBz;

        // Contact info
        float normalX, normalY, normalZ;
        float penetration;

        // Impulse cache (warm starting)
        float normalImpulse;
        float tangentImpulse1;
        float tangentImpulse2;

        // Lifetime
        int lifeTime;
    };

    struct GPUManifold {
        int bodyA;
        int bodyB;
        int numPoints;
        int padding;  // alignment
        GPUContactPoint points[4];
    };

    // Helper to create pair key
    inline int makePairKey(int a, int b) {
        int lo = (a < b) ? a : b;
        int hi = (a < b) ? b : a;
        return (lo << 16) | hi;
    }

    // Helper to unpack pair key
    inline void unpackPairKey(int key, int& a, int& b) {
        a = key >> 16;
        b = key & 0xFFFF;
    }

}

#endif