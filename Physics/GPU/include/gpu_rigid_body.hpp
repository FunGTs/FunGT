#if !defined(_GPU_RIGID_BODY_H_)
#define _GPU_RIGID_BODY_H_
#include "Vector/gpu_vec3.hpp"
#include "Quaternion/quaternion.hpp"
struct GPURigidBody {
    // Linear motion (Vec3 arrays)
    fungt::gpuVec3 positions;
    fungt::gpuVec3 velocities;
    fungt::gpuVec3 forces;

    // Angular motion (Vec3 arrays)
    fungt::gpuVec3 angularVelocities;
    fungt::gpuVec3 torques;

    // Orientations (quaternions - keep as struct for now)
    std::vector<Quaternion> orientations;

    // Mass properties (scalars)
    std::vector<float> invMasses;

    // Inertia tensors (matrices - we'll optimize later if needed)
    std::vector<fungt::Matrix3f> invInertiaTensorsWorld;

    // Material properties (scalars)
    std::vector<float> restitutions;
    std::vector<float> frictions;

    // Shape data
    std::vector<int> shapeTypes;      // 0=sphere, 1=box
    std::vector<float> shapeRadii;    // For spheres
    fungt::gpuVec3 shapeSizes;              // For boxes (reuse fungt::gpuVec3!)

    int numBodies = 0;

    void clear() {
        positions.clear();
        velocities.clear();
        forces.clear();
        angularVelocities.clear();
        torques.clear();
        orientations.clear();
        invMasses.clear();
        invInertiaTensorsWorld.clear();
        restitutions.clear();
        frictions.clear();
        shapeTypes.clear();
        shapeRadii.clear();
        shapeSizes.clear();
        numBodies = 0;
    }

    void reserve(int n) {
        positions.reserve(n);
        velocities.reserve(n);
        forces.reserve(n);
        angularVelocities.reserve(n);
        torques.reserve(n);
        orientations.reserve(n);
        invMasses.reserve(n);
        invInertiaTensorsWorld.reserve(n);
        restitutions.reserve(n);
        frictions.reserve(n);
        shapeTypes.reserve(n);
        shapeRadii.reserve(n);
        shapeSizes.reserve(n);
    }
};


#endif // _GPU_RIGID_BODY_H_
