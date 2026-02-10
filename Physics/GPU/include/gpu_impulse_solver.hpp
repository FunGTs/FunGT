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
    inline void solveContactImpulseNoAng(
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
    inline void solveContactImpulseAng(
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

        // Get contact normal
        float nx = cp->normalX;
        float ny = cp->normalY;
        float nz = cp->normalZ;

        // Get positions
        float posAx = data.x_pos[bodyA];
        float posAy = data.y_pos[bodyA];
        float posAz = data.z_pos[bodyA];
        float posBx = data.x_pos[bodyB];
        float posBy = data.y_pos[bodyB];
        float posBz = data.z_pos[bodyB];

        // r vectors: from body center to contact point
        float rAx = cp->worldPointAx - posAx;
        float rAy = cp->worldPointAy - posAy;
        float rAz = cp->worldPointAz - posAz;
        float rBx = cp->worldPointBx - posBx;
        float rBy = cp->worldPointBy - posBy;
        float rBz = cp->worldPointBz - posBz;

        // Get linear velocities
        float velAx = data.x_vel[bodyA];
        float velAy = data.y_vel[bodyA];
        float velAz = data.z_vel[bodyA];
        float velBx = data.x_vel[bodyB];
        float velBy = data.y_vel[bodyB];
        float velBz = data.z_vel[bodyB];

        // Get angular velocities
        float angVelAx = data.x_angVel[bodyA];
        float angVelAy = data.y_angVel[bodyA];
        float angVelAz = data.z_angVel[bodyA];
        float angVelBx = data.x_angVel[bodyB];
        float angVelBy = data.y_angVel[bodyB];
        float angVelBz = data.z_angVel[bodyB];

        // Velocity at contact point: v + angVel × r
        // Cross product: angVelA × rA
        float wAxrAx = angVelAy * rAz - angVelAz * rAy;
        float wAxrAy = angVelAz * rAx - angVelAx * rAz;
        float wAxrAz = angVelAx * rAy - angVelAy * rAx;

        // Cross product: angVelB × rB
        float wBxrBx = angVelBy * rBz - angVelBz * rBy;
        float wBxrBy = angVelBz * rBx - angVelBx * rBz;
        float wBxrBz = angVelBx * rBy - angVelBy * rBx;

        // Velocity at contact points
        float velAtContactAx = velAx + wAxrAx;
        float velAtContactAy = velAy + wAxrAy;
        float velAtContactAz = velAz + wAxrAz;

        float velAtContactBx = velBx + wBxrBx;
        float velAtContactBy = velBy + wBxrBy;
        float velAtContactBz = velBz + wBxrBz;

        // Relative velocity at contact (B relative to A)
        float relVelx = velAtContactBx - velAtContactAx;
        float relVely = velAtContactBy - velAtContactAy;
        float relVelz = velAtContactBz - velAtContactAz;

        // Relative velocity along normal
        float relVelNormal = relVelx * nx + relVely * ny + relVelz * nz;

        // Skip if separating
        if (relVelNormal > 0.0f) {
            return;
        }

        // Inverse masses
        float invMassA = data.invMass[bodyA];
        float invMassB = data.invMass[bodyB];

        // Compute effective mass (includes angular contribution)
        // kNormal = 1/mA + 1/mB + (rA × n)ᵀ * I⁻¹_A * (rA × n) + (rB × n)ᵀ * I⁻¹_B * (rB × n)

        float kNormal = invMassA + invMassB;

        // rA × n
        float rAxNx = rAy * nz - rAz * ny;
        float rAxNy = rAz * nx - rAx * nz;
        float rAxNz = rAx * ny - rAy * nx;

        // rB × n
        float rBxNx = rBy * nz - rBz * ny;
        float rBxNy = rBz * nx - rBx * nz;
        float rBxNz = rBx * ny - rBy * nx;

        // Add angular contribution for body A (if dynamic)
        if (data.bodyMode[bodyA] == 1) {
            // I⁻¹_A * (rA × n)
            float* invIA = &data.invInertiaTensor[bodyA * 9];
            float tempAx = invIA[0] * rAxNx + invIA[1] * rAxNy + invIA[2] * rAxNz;
            float tempAy = invIA[3] * rAxNx + invIA[4] * rAxNy + invIA[5] * rAxNz;
            float tempAz = invIA[6] * rAxNx + invIA[7] * rAxNy + invIA[8] * rAxNz;

            // (rA × n) · (I⁻¹_A * (rA × n))
            kNormal += rAxNx * tempAx + rAxNy * tempAy + rAxNz * tempAz;
        }

        // Add angular contribution for body B (if dynamic)
        if (data.bodyMode[bodyB] == 1) {
            // I⁻¹_B * (rB × n)
            float* invIB = &data.invInertiaTensor[bodyB * 9];
            float tempBx = invIB[0] * rBxNx + invIB[1] * rBxNy + invIB[2] * rBxNz;
            float tempBy = invIB[3] * rBxNx + invIB[4] * rBxNy + invIB[5] * rBxNz;
            float tempBz = invIB[6] * rBxNx + invIB[7] * rBxNy + invIB[8] * rBxNz;

            // (rB × n) · (I⁻¹_B * (rB × n))
            kNormal += rBxNx * tempBx + rBxNy * tempBy + rBxNz * tempBz;
        }

        if (kNormal <= 0.0001f) {
            return;
        }

        float effectiveMass = 1.0f / kNormal;

        // Restitution
        float restA = data.restitution[bodyA];
        float restB = data.restitution[bodyB];
        float restitution = (restA < restB) ? restA : restB;

        // Calculate impulse
        float impulse = -relVelNormal * (1.0f + restitution) * effectiveMass;

        // Position correction (Baumgarte)
        float correction = (cp->penetration - 0.01f) / dt * 0.2f;
        impulse += correction * effectiveMass;

        // Clamp to prevent pulling
        if (impulse < 0.0f) impulse = 0.0f;

        // Impulse vector
        float impulseX = impulse * nx;
        float impulseY = impulse * ny;
        float impulseZ = impulse * nz;

        // Apply LINEAR impulse
        if (data.bodyMode[bodyA] == 1) {
            data.x_vel[bodyA] -= impulseX * invMassA;
            data.y_vel[bodyA] -= impulseY * invMassA;
            data.z_vel[bodyA] -= impulseZ * invMassA;
        }

        if (data.bodyMode[bodyB] == 1) {
            data.x_vel[bodyB] += impulseX * invMassB;
            data.y_vel[bodyB] += impulseY * invMassB;
            data.z_vel[bodyB] += impulseZ * invMassB;
        }

        // Apply ANGULAR impulse
        // torque = r × impulse
        // deltaAngVel = I⁻¹ * torque

        if (data.bodyMode[bodyA] == 1) {
            // torqueA = rA × (-impulse)  (negative because impulse points away from A)
            float torqueAx = rAy * (-impulseZ) - rAz * (-impulseY);
            float torqueAy = rAz * (-impulseX) - rAx * (-impulseZ);
            float torqueAz = rAx * (-impulseY) - rAy * (-impulseX);

            // deltaAngVelA = I⁻¹_A * torqueA
            float* invIA = &data.invInertiaTensor[bodyA * 9];
            float dAngVelAx = invIA[0] * torqueAx + invIA[1] * torqueAy + invIA[2] * torqueAz;
            float dAngVelAy = invIA[3] * torqueAx + invIA[4] * torqueAy + invIA[5] * torqueAz;
            float dAngVelAz = invIA[6] * torqueAx + invIA[7] * torqueAy + invIA[8] * torqueAz;

            data.x_angVel[bodyA] += dAngVelAx;
            data.y_angVel[bodyA] += dAngVelAy;
            data.z_angVel[bodyA] += dAngVelAz;
        }

        if (data.bodyMode[bodyB] == 1) {
            // torqueB = rB × impulse
            float torqueBx = rBy * impulseZ - rBz * impulseY;
            float torqueBy = rBz * impulseX - rBx * impulseZ;
            float torqueBz = rBx * impulseY - rBy * impulseX;

            // deltaAngVelB = I⁻¹_B * torqueB
            float* invIB = &data.invInertiaTensor[bodyB * 9];
            float dAngVelBx = invIB[0] * torqueBx + invIB[1] * torqueBy + invIB[2] * torqueBz;
            float dAngVelBy = invIB[3] * torqueBx + invIB[4] * torqueBy + invIB[5] * torqueBz;
            float dAngVelBz = invIB[6] * torqueBx + invIB[7] * torqueBy + invIB[8] * torqueBz;

            data.x_angVel[bodyB] += dAngVelBx;
            data.y_angVel[bodyB] += dAngVelBy;
            data.z_angVel[bodyB] += dAngVelBz;
        }
    }
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

        // Get contact normal
        float nx = cp->normalX;
        float ny = cp->normalY;
        float nz = cp->normalZ;

        // Get positions
        float posAx = data.x_pos[bodyA];
        float posAy = data.y_pos[bodyA];
        float posAz = data.z_pos[bodyA];
        float posBx = data.x_pos[bodyB];
        float posBy = data.y_pos[bodyB];
        float posBz = data.z_pos[bodyB];

        // r vectors: from body center to contact point
        float rAx = cp->worldPointAx - posAx;
        float rAy = cp->worldPointAy - posAy;
        float rAz = cp->worldPointAz - posAz;
        float rBx = cp->worldPointBx - posBx;
        float rBy = cp->worldPointBy - posBy;
        float rBz = cp->worldPointBz - posBz;

        // Get linear velocities
        float velAx = data.x_vel[bodyA];
        float velAy = data.y_vel[bodyA];
        float velAz = data.z_vel[bodyA];
        float velBx = data.x_vel[bodyB];
        float velBy = data.y_vel[bodyB];
        float velBz = data.z_vel[bodyB];

        // Get angular velocities
        float angVelAx = data.x_angVel[bodyA];
        float angVelAy = data.y_angVel[bodyA];
        float angVelAz = data.z_angVel[bodyA];
        float angVelBx = data.x_angVel[bodyB];
        float angVelBy = data.y_angVel[bodyB];
        float angVelBz = data.z_angVel[bodyB];

        // Velocity at contact point: v + angVel × r
        float wAxrAx = angVelAy * rAz - angVelAz * rAy;
        float wAxrAy = angVelAz * rAx - angVelAx * rAz;
        float wAxrAz = angVelAx * rAy - angVelAy * rAx;

        float wBxrBx = angVelBy * rBz - angVelBz * rBy;
        float wBxrBy = angVelBz * rBx - angVelBx * rBz;
        float wBxrBz = angVelBx * rBy - angVelBy * rBx;

        float velAtContactAx = velAx + wAxrAx;
        float velAtContactAy = velAy + wAxrAy;
        float velAtContactAz = velAz + wAxrAz;

        float velAtContactBx = velBx + wBxrBx;
        float velAtContactBy = velBy + wBxrBy;
        float velAtContactBz = velBz + wBxrBz;

        // Relative velocity at contact (B relative to A)
        float relVelx = velAtContactBx - velAtContactAx;
        float relVely = velAtContactBy - velAtContactAy;
        float relVelz = velAtContactBz - velAtContactAz;

        // Relative velocity along normal
        float relVelNormal = relVelx * nx + relVely * ny + relVelz * nz;

        // Skip if separating
        if (relVelNormal > 0.0f) {
            return;
        }

        // Inverse masses
        float invMassA = data.invMass[bodyA];
        float invMassB = data.invMass[bodyB];

        float kNormal = invMassA + invMassB;

        // rA × n
        float rAxNx = rAy * nz - rAz * ny;
        float rAxNy = rAz * nx - rAx * nz;
        float rAxNz = rAx * ny - rAy * nx;

        // rB × n
        float rBxNx = rBy * nz - rBz * ny;
        float rBxNy = rBz * nx - rBx * nz;
        float rBxNz = rBx * ny - rBy * nx;

        // Add angular contribution for body A (if dynamic)
        float* invIA = &data.invInertiaTensor[bodyA * 9];
        float* invIB = &data.invInertiaTensor[bodyB * 9];

        if (data.bodyMode[bodyA] == 1) {
            float tempAx = invIA[0] * rAxNx + invIA[1] * rAxNy + invIA[2] * rAxNz;
            float tempAy = invIA[3] * rAxNx + invIA[4] * rAxNy + invIA[5] * rAxNz;
            float tempAz = invIA[6] * rAxNx + invIA[7] * rAxNy + invIA[8] * rAxNz;
            kNormal += rAxNx * tempAx + rAxNy * tempAy + rAxNz * tempAz;
        }

        if (data.bodyMode[bodyB] == 1) {
            float tempBx = invIB[0] * rBxNx + invIB[1] * rBxNy + invIB[2] * rBxNz;
            float tempBy = invIB[3] * rBxNx + invIB[4] * rBxNy + invIB[5] * rBxNz;
            float tempBz = invIB[6] * rBxNx + invIB[7] * rBxNy + invIB[8] * rBxNz;
            kNormal += rBxNx * tempBx + rBxNy * tempBy + rBxNz * tempBz;
        }

        if (kNormal <= 0.0001f) {
            return;
        }

        float effectiveMass = 1.0f / kNormal;

        // Restitution
        float restA = data.restitution[bodyA];
        float restB = data.restitution[bodyB];
        float restitution = (restA < restB) ? restA : restB;

        // Calculate impulse
        float impulse = -relVelNormal * (1.0f + restitution) * effectiveMass;

        // Position correction (Baumgarte)
        float correction = (cp->penetration - 0.01f) / dt * 0.2f;
        impulse += correction * effectiveMass;

        // Clamp to prevent pulling
        if (impulse < 0.0f) impulse = 0.0f;

        // Store for friction
        cp->normalImpulse = impulse;

        // Impulse vector
        float impulseX = impulse * nx;
        float impulseY = impulse * ny;
        float impulseZ = impulse * nz;

        // Apply LINEAR impulse
        if (data.bodyMode[bodyA] == 1) {
            data.x_vel[bodyA] -= impulseX * invMassA;
            data.y_vel[bodyA] -= impulseY * invMassA;
            data.z_vel[bodyA] -= impulseZ * invMassA;
        }

        if (data.bodyMode[bodyB] == 1) {
            data.x_vel[bodyB] += impulseX * invMassB;
            data.y_vel[bodyB] += impulseY * invMassB;
            data.z_vel[bodyB] += impulseZ * invMassB;
        }

        // Apply ANGULAR impulse
        if (data.bodyMode[bodyA] == 1) {
            float torqueAx = rAy * (-impulseZ) - rAz * (-impulseY);
            float torqueAy = rAz * (-impulseX) - rAx * (-impulseZ);
            float torqueAz = rAx * (-impulseY) - rAy * (-impulseX);

            data.x_angVel[bodyA] += invIA[0] * torqueAx + invIA[1] * torqueAy + invIA[2] * torqueAz;
            data.y_angVel[bodyA] += invIA[3] * torqueAx + invIA[4] * torqueAy + invIA[5] * torqueAz;
            data.z_angVel[bodyA] += invIA[6] * torqueAx + invIA[7] * torqueAy + invIA[8] * torqueAz;
        }

        if (data.bodyMode[bodyB] == 1) {
            float torqueBx = rBy * impulseZ - rBz * impulseY;
            float torqueBy = rBz * impulseX - rBx * impulseZ;
            float torqueBz = rBx * impulseY - rBy * impulseX;

            data.x_angVel[bodyB] += invIB[0] * torqueBx + invIB[1] * torqueBy + invIB[2] * torqueBz;
            data.y_angVel[bodyB] += invIB[3] * torqueBx + invIB[4] * torqueBy + invIB[5] * torqueBz;
            data.z_angVel[bodyB] += invIB[6] * torqueBx + invIB[7] * torqueBy + invIB[8] * torqueBz;
        }

        // ========== FRICTION ==========
        // Tangent velocity = relative velocity - normal component
        float tangentVelx = relVelx - relVelNormal * nx;
        float tangentVely = relVely - relVelNormal * ny;
        float tangentVelz = relVelz - relVelNormal * nz;

        float tangentSpeed = sycl::sqrt(tangentVelx * tangentVelx + tangentVely * tangentVely + tangentVelz * tangentVelz);

        if (tangentSpeed < 0.0001f) {
            return;  // No sliding
        }

        // Tangent direction
        float tx = tangentVelx / tangentSpeed;
        float ty = tangentVely / tangentSpeed;
        float tz = tangentVelz / tangentSpeed;

        // rA × t
        float rAxTx = rAy * tz - rAz * ty;
        float rAxTy = rAz * tx - rAx * tz;
        float rAxTz = rAx * ty - rAy * tx;

        // rB × t
        float rBxTx = rBy * tz - rBz * ty;
        float rBxTy = rBz * tx - rBx * tz;
        float rBxTz = rBx * ty - rBy * tx;

        float kTangent = invMassA + invMassB;

        if (data.bodyMode[bodyA] == 1) {
            float tempAx = invIA[0] * rAxTx + invIA[1] * rAxTy + invIA[2] * rAxTz;
            float tempAy = invIA[3] * rAxTx + invIA[4] * rAxTy + invIA[5] * rAxTz;
            float tempAz = invIA[6] * rAxTx + invIA[7] * rAxTy + invIA[8] * rAxTz;
            kTangent += rAxTx * tempAx + rAxTy * tempAy + rAxTz * tempAz;
        }

        if (data.bodyMode[bodyB] == 1) {
            float tempBx = invIB[0] * rBxTx + invIB[1] * rBxTy + invIB[2] * rBxTz;
            float tempBy = invIB[3] * rBxTx + invIB[4] * rBxTy + invIB[5] * rBxTz;
            float tempBz = invIB[6] * rBxTx + invIB[7] * rBxTy + invIB[8] * rBxTz;
            kTangent += rBxTx * tempBx + rBxTy * tempBy + rBxTz * tempBz;
        }

        if (kTangent <= 0.0001f) {
            return;
        }

        float effectiveMassTangent = 1.0f / kTangent;

        // Friction coefficient
        float frictionA = data.friction[bodyA];
        float frictionB = data.friction[bodyB];
        float friction = (frictionA < frictionB) ? frictionA : frictionB;

        // Friction impulse
        float frictionImpulse = -tangentSpeed * effectiveMassTangent;

        // Coulomb clamp: |friction| <= μ * normalImpulse
        float maxFriction = friction * cp->normalImpulse;
        if (frictionImpulse < -maxFriction) frictionImpulse = -maxFriction;
        if (frictionImpulse > maxFriction) frictionImpulse = maxFriction;

        float fImpX = frictionImpulse * tx;
        float fImpY = frictionImpulse * ty;
        float fImpZ = frictionImpulse * tz;

        // Apply LINEAR friction
        if (data.bodyMode[bodyA] == 1) {
            data.x_vel[bodyA] -= fImpX * invMassA;
            data.y_vel[bodyA] -= fImpY * invMassA;
            data.z_vel[bodyA] -= fImpZ * invMassA;
        }

        if (data.bodyMode[bodyB] == 1) {
            data.x_vel[bodyB] += fImpX * invMassB;
            data.y_vel[bodyB] += fImpY * invMassB;
            data.z_vel[bodyB] += fImpZ * invMassB;
        }

        // Apply ANGULAR friction
        if (data.bodyMode[bodyA] == 1) {
            float torqueAx = rAy * (-fImpZ) - rAz * (-fImpY);
            float torqueAy = rAz * (-fImpX) - rAx * (-fImpZ);
            float torqueAz = rAx * (-fImpY) - rAy * (-fImpX);

            data.x_angVel[bodyA] += invIA[0] * torqueAx + invIA[1] * torqueAy + invIA[2] * torqueAz;
            data.y_angVel[bodyA] += invIA[3] * torqueAx + invIA[4] * torqueAy + invIA[5] * torqueAz;
            data.z_angVel[bodyA] += invIA[6] * torqueAx + invIA[7] * torqueAy + invIA[8] * torqueAz;
        }

        if (data.bodyMode[bodyB] == 1) {
            float torqueBx = rBy * fImpZ - rBz * fImpY;
            float torqueBy = rBz * fImpX - rBx * fImpZ;
            float torqueBz = rBx * fImpY - rBy * fImpX;

            data.x_angVel[bodyB] += invIB[0] * torqueBx + invIB[1] * torqueBy + invIB[2] * torqueBz;
            data.y_angVel[bodyB] += invIB[3] * torqueBx + invIB[4] * torqueBy + invIB[5] * torqueBz;
            data.z_angVel[bodyB] += invIB[6] * torqueBx + invIB[7] * torqueBy + invIB[8] * torqueBz;
        }
    }
} // namespace gpu

#endif