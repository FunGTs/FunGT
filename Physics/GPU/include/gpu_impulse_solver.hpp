// gpu_impulse_solver.hpp

#if !defined(_GPU_IMPULSE_SOLVER_HPP_)
#define _GPU_IMPULSE_SOLVER_HPP_

#include "gpu_device_data.hpp"
#include "gpu_manifold_contacts.hpp"
#include <sycl/sycl.hpp>

namespace gpu {

    // Helper: cross product
    inline void cross(float ax, float ay, float az,
        float bx, float by, float bz,
        float& rx, float& ry, float& rz)
    {
        rx = ay * bz - az * by;
        ry = az * bx - ax * bz;
        rz = ax * by - ay * bx;
    }

    // Helper: dot product
    inline float dot(float ax, float ay, float az,
        float bx, float by, float bz)
    {
        return ax * bx + ay * by + az * bz;
    }

    // Helper: matrix * vector (3x3 * 3)
    inline void matVec(const float* mat, float vx, float vy, float vz,
        float& rx, float& ry, float& rz)
    {
        rx = mat[0] * vx + mat[1] * vy + mat[2] * vz;
        ry = mat[3] * vx + mat[4] * vy + mat[5] * vz;
        rz = mat[6] * vx + mat[7] * vy + mat[8] * vz;
    }

    // inline void solveContactImpulse(
    //     GPUContactPoint* cp,
    //     DeviceData data,
    //     int bodyA,
    //     int bodyB,
    //     float dt,
    //     float ERP)
    // {
    //     // Skip unstable contacts
    //     if (cp->penetration > 5.0f) {
    //         return;
    //     }

    //     // Body positions
    //     float posAx = data.x_pos[bodyA];
    //     float posAy = data.y_pos[bodyA];
    //     float posAz = data.z_pos[bodyA];
    //     float posBx = data.x_pos[bodyB];
    //     float posBy = data.y_pos[bodyB];
    //     float posBz = data.z_pos[bodyB];

    //     // r vectors (contact point relative to body center)
    //     float rAx = cp->worldPointAx - posAx;
    //     float rAy = cp->worldPointAy - posAy;
    //     float rAz = cp->worldPointAz - posAz;
    //     float rBx = cp->worldPointBx - posBx;
    //     float rBy = cp->worldPointBy - posBy;
    //     float rBz = cp->worldPointBz - posBz;

    //     // Body velocities
    //     float velAx = data.x_vel[bodyA];
    //     float velAy = data.y_vel[bodyA];
    //     float velAz = data.z_vel[bodyA];
    //     float velBx = data.x_vel[bodyB];
    //     float velBy = data.y_vel[bodyB];
    //     float velBz = data.z_vel[bodyB];

    //     // Angular velocities
    //     float angVelAx = data.x_angVel[bodyA];
    //     float angVelAy = data.y_angVel[bodyA];
    //     float angVelAz = data.z_angVel[bodyA];
    //     float angVelBx = data.x_angVel[bodyB];
    //     float angVelBy = data.y_angVel[bodyB];
    //     float angVelBz = data.z_angVel[bodyB];

    //     // Velocity at contact point: v + angVel x r
    //     float wAxrAx, wAxrAy, wAxrAz;
    //     cross(angVelAx, angVelAy, angVelAz, rAx, rAy, rAz, wAxrAx, wAxrAy, wAxrAz);

    //     float wBxrBx, wBxrBy, wBxrBz;
    //     cross(angVelBx, angVelBy, angVelBz, rBx, rBy, rBz, wBxrBx, wBxrBy, wBxrBz);

    //     float velAtContactAx = velAx + wAxrAx;
    //     float velAtContactAy = velAy + wAxrAy;
    //     float velAtContactAz = velAz + wAxrAz;

    //     float velAtContactBx = velBx + wBxrBx;
    //     float velAtContactBy = velBy + wBxrBy;
    //     float velAtContactBz = velBz + wBxrBz;

    //     // Relative velocity at contact
    //     float relVelx = velAtContactAx - velAtContactBx;
    //     float relVely = velAtContactAy - velAtContactBy;
    //     float relVelz = velAtContactAz - velAtContactBz;

    //     // Normal component of relative velocity
    //     float nx = cp->normalX;
    //     float ny = cp->normalY;
    //     float nz = cp->normalZ;
    //     float relVelNormal = dot(relVelx, relVely, relVelz, nx, ny, nz);

    //     // Compute effective mass
    //     float invMassA = data.invMass[bodyA];
    //     float invMassB = data.invMass[bodyB];

    //     // rA x n
    //     float rAxNx, rAxNy, rAxNz;
    //     cross(rAx, rAy, rAz, nx, ny, nz, rAxNx, rAxNy, rAxNz);

    //     // rB x n
    //     float rBxNx, rBxNy, rBxNz;
    //     cross(rBx, rBy, rBz, nx, ny, nz, rBxNx, rBxNy, rBxNz);

    //     float kNormal = invMassA + invMassB;

    //     // Add angular contribution for body A (if dynamic)
    //     if (data.bodyMode[bodyA] == 1) {
    //         float tempx, tempy, tempz;
    //         matVec(&data.invInertiaTensor[bodyA * 9], rAxNx, rAxNy, rAxNz, tempx, tempy, tempz);
    //         kNormal += dot(rAxNx, rAxNy, rAxNz, tempx, tempy, tempz);
    //     }

    //     // Add angular contribution for body B (if dynamic)
    //     if (data.bodyMode[bodyB] == 1) {
    //         float tempx, tempy, tempz;
    //         matVec(&data.invInertiaTensor[bodyB * 9], rBxNx, rBxNy, rBxNz, tempx, tempy, tempz);
    //         kNormal += dot(rBxNx, rBxNy, rBxNz, tempx, tempy, tempz);
    //     }

    //     if (kNormal <= 0.0001f) {
    //         return;
    //     }

    //     float effectiveMass = 1.0f / kNormal;

    //     // Restitution
    //     float restA = data.restitution[bodyA];
    //     float restB = data.restitution[bodyB];
    //     float restitution = (restA < restB) ? restA : restB;

    //     // Baumgarte stabilization
    //     float biasTerm = 0.0f;
    //     if (cp->penetration > 0.01f) {
    //         biasTerm = (ERP / dt) * cp->penetration;
    //     }

    //     // FIX #1: Add negative sign to velocity bias
    //     float velocityBias = -relVelNormal * (1.0f + restitution);
    //     float totalBias = velocityBias + biasTerm;

    //     // Compute impulse
    //     float lambda = totalBias * effectiveMass;
    //     float deltaImpulse = lambda - cp->normalImpulse;

    //     // FIX #2: Clamp to 0.0f, not 4.0f
    //     float sum = cp->normalImpulse + deltaImpulse;
    //     if (sum < 0.0f) {
    //         deltaImpulse = -cp->normalImpulse;
    //         cp->normalImpulse = 0.0f;
    //     }
    //     else {
    //         cp->normalImpulse = sum;
    //     }

    //     // Apply impulse
    //     float impulseX = nx * deltaImpulse;
    //     float impulseY = ny * deltaImpulse;
    //     float impulseZ = nz * deltaImpulse;

    //     // Update body A velocity (if dynamic)
    //     if (data.bodyMode[bodyA] == 1) {
    //         data.x_vel[bodyA] -= impulseX * invMassA;
    //         data.y_vel[bodyA] -= impulseY * invMassA;
    //         data.z_vel[bodyA] -= impulseZ * invMassA;

    //         // FIX #3: Update angular velocity for body A
    //         float torqueAx, torqueAy, torqueAz;
    //         cross(rAx, rAy, rAz, impulseX, impulseY, impulseZ, torqueAx, torqueAy, torqueAz);

    //         float deltaAngVelAx, deltaAngVelAy, deltaAngVelAz;
    //         matVec(&data.invInertiaTensor[bodyA * 9], torqueAx, torqueAy, torqueAz,
    //             deltaAngVelAx, deltaAngVelAy, deltaAngVelAz);

    //         data.x_angVel[bodyA] -= deltaAngVelAx;
    //         data.y_angVel[bodyA] -= deltaAngVelAy;
    //         data.z_angVel[bodyA] -= deltaAngVelAz;
    //     }

    //     // Update body B velocity (if dynamic)
    //     if (data.bodyMode[bodyB] == 1) {
    //         data.x_vel[bodyB] += impulseX * invMassB;
    //         data.y_vel[bodyB] += impulseY * invMassB;
    //         data.z_vel[bodyB] += impulseZ * invMassB;

    //         // FIX #3: Update angular velocity for body B
    //         float torqueBx, torqueBy, torqueBz;
    //         cross(rBx, rBy, rBz, impulseX, impulseY, impulseZ, torqueBx, torqueBy, torqueBz);

    //         float deltaAngVelBx, deltaAngVelBy, deltaAngVelBz;
    //         matVec(&data.invInertiaTensor[bodyB * 9], torqueBx, torqueBy, torqueBz,
    //             deltaAngVelBx, deltaAngVelBy, deltaAngVelBz);

    //         data.x_angVel[bodyB] += deltaAngVelBx;
    //         data.y_angVel[bodyB] += deltaAngVelBy;
    //         data.z_angVel[bodyB] += deltaAngVelBz;
    //     }
    // }
    // 
    
    inline void solveContactImpulse(
        GPUContactPoint* cp,
        DeviceData data,
        int bodyA,
        int bodyB,
        float dt,
        float ERP)
    {
        if (cp->penetration <= 0.001f) {
            return;
        }

        // Get contact normal (works in 3D now)
        float nx = cp->normalX;
        float ny = cp->normalY;
        float nz = cp->normalZ;

        // Get velocities
        float velAx = data.x_vel[bodyA];
        float velAy = data.y_vel[bodyA];
        float velAz = data.z_vel[bodyA];
        float velBx = data.x_vel[bodyB];
        float velBy = data.y_vel[bodyB];
        float velBz = data.z_vel[bodyB];

        // Relative velocity along normal
        float relVelNormal = (velBx - velAx) * nx + (velBy - velAy) * ny + (velBz - velAz) * nz;
        // Skip if separating (positive = moving apart)
        if (relVelNormal > 0.0f) {
            return;
        }
        // Calculate impulse
        float invMassA = data.invMass[bodyA];
        float invMassB = data.invMass[bodyB];
        float effectiveMass = 1.0f / (invMassA + invMassB);

        //float impulse = relVelNormal * effectiveMass;
        //float restitution = 0.3f;  // 0 = no bounce, 1 = full bounce
        float restA = data.restitution[bodyA];
        float restB = data.restitution[bodyB];
        float restitution = (restA < restB) ? restA : restB;
        float impulse = -relVelNormal * (1.0f + restitution) * effectiveMass;
        // Add position correction
        float correction = (cp->penetration - 0.01f) / dt * 0.2f;
        impulse += correction * effectiveMass;

        // Apply impulse in 3D
        if (data.bodyMode[bodyA] == 1) {
            data.x_vel[bodyA] -= impulse * invMassA * nx;
            data.y_vel[bodyA] -= impulse * invMassA * ny;
            data.z_vel[bodyA] -= impulse * invMassA * nz;
        }

        if (data.bodyMode[bodyB] == 1) {
            data.x_vel[bodyB] += impulse * invMassB * nx;
            data.y_vel[bodyB] += impulse * invMassB * ny;
            data.z_vel[bodyB] += impulse * invMassB * nz;
        }
    }
} // namespace gpu

#endif