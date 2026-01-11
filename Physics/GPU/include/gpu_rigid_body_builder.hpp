#if !defined(_GPU_RIGID_BODY_BUILDER_H_)
#define _GPU_RIGID_BODY_BUILDER_H_

#include "gpu_rigid_body.hpp"
#include "Vector/vector3.hpp"

class GPURigidBodyBuilder {
public:
    // Create empty GPU rigid body state
    static GPURigidBody createState() {
        GPURigidBody state;
        return state;
    }

    // Add a sphere to GPU state
    static int addSphere(GPURigidBody& state,
        fungt::Vec3 position,
        float radius,
        float mass,
        float restitution = 0.6f,
        float friction = 0.3f) {
        int index = state.numBodies++;

        // Linear motion
        state.positions.push_back(position);
        state.velocities.push_back(fungt::Vec3(0, 0, 0));
        state.forces.push_back(fungt::Vec3(0, 0, 0));

        // Angular motion
        state.angularVelocities.push_back(fungt::Vec3(0, 0, 0));
        state.torques.push_back(fungt::Vec3(0, 0, 0));
        state.orientations.push_back(Quaternion(1, 0, 0, 0)); // Identity quaternion

        // Mass
        float invMass = (mass > 0.0f) ? (1.0f / mass) : 0.0f;
        state.invMasses.push_back(invMass);

        // Sphere inertia: I = (2/5) * m * r^2
        float I = (2.0f / 5.0f) * mass * radius * radius;
        float invI = (I > 0.0f) ? (1.0f / I) : 0.0f;

        fungt::Matrix3f invInertia;
        invInertia.m[0][0] = invI;
        invInertia.m[1][1] = invI;
        invInertia.m[2][2] = invI;
        state.invInertiaTensorsWorld.push_back(invInertia);

        // Material properties
        state.restitutions.push_back(restitution);
        state.frictions.push_back(friction);

        // Shape data
        state.shapeTypes.push_back(0); // 0 = sphere
        state.shapeRadii.push_back(radius);
        state.shapeSizes.push_back(fungt::Vec3(0, 0, 0)); // Not used for sphere

        return index;
    }

    // Add a box to GPU state
    static int addBox(GPURigidBody& state,
        fungt::Vec3 position,
        fungt::Vec3 size,
        float mass,
        float restitution = 0.6f,
        float friction = 0.3f) {
        int index = state.numBodies++;

        // Linear motion
        state.positions.push_back(position);
        state.velocities.push_back(fungt::Vec3(0, 0, 0));
        state.forces.push_back(fungt::Vec3(0, 0, 0));

        // Angular motion
        state.angularVelocities.push_back(fungt::Vec3(0, 0, 0));
        state.torques.push_back(fungt::Vec3(0, 0, 0));
        state.orientations.push_back(Quaternion(1, 0, 0, 0));

        // Mass
        float invMass = (mass > 0.0f) ? (1.0f / mass) : 0.0f;
        state.invMasses.push_back(invMass);

        // Box inertia tensor (assuming uniform density)
        // I_x = (1/12) * m * (h^2 + d^2)
        // I_y = (1/12) * m * (w^2 + d^2)
        // I_z = (1/12) * m * (w^2 + h^2)
        float w = size.x, h = size.y, d = size.z;
        fungt::Matrix3f invInertia;

        if (mass > 0.0f) {
            float Ix = (1.0f / 12.0f) * mass * (h * h + d * d);
            float Iy = (1.0f / 12.0f) * mass * (w * w + d * d);
            float Iz = (1.0f / 12.0f) * mass * (w * w + h * h);

            invInertia.m[0][0] = 1.0f / Ix;
            invInertia.m[1][1] = 1.0f / Iy;
            invInertia.m[2][2] = 1.0f / Iz;
        }
        state.invInertiaTensorsWorld.push_back(invInertia);

        // Material properties
        state.restitutions.push_back(restitution);
        state.frictions.push_back(friction);

        // Shape data
        state.shapeTypes.push_back(1); // 1 = box
        state.shapeRadii.push_back(0.0f); // Not used for box
        state.shapeSizes.push_back(size);

        return index;
    }

    // Add initial velocity to a body
    static void setVelocity(GPURigidBody& state, int index, const fungt::Vec3& vel) {
        state.velocities.set(index, vel);
    }

    // Add initial angular velocity
    static void setAngularVelocity(GPURigidBody& state, int index, const fungt::Vec3& angVel) {
        state.angularVelocities.set(index, angVel);
    }
};

#endif // _GPU_RIGID_BODY_BUILDER_H_