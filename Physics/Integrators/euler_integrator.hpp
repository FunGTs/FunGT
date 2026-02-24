#if !defined(_EULER_INTEGRATOR_H_)
#define _EULER_INTEGRATOR_H_
#include "integrator.hpp"
class EulerIntegrator : public Integrator{

public:
    EulerIntegrator(){

    }
    virtual ~EulerIntegrator(){

    }
    void integrate(std::shared_ptr<RigidBody> body, float dt) override {
       if (body->m_invMass == 0) return; // Static body doesn't move

        // Linear motion
        fungt::Vec3 acceleration = body->m_force * body->m_invMass;

        // SAFETY: Check for NaN in acceleration
        if (!std::isfinite(acceleration.x) || !std::isfinite(acceleration.y) || !std::isfinite(acceleration.z)) {
            std::cerr << "ERROR: NaN in linear acceleration! Force: (" << body->m_force.x << ", " << body->m_force.y << ", " << body->m_force.z << "), invMass: " << body->m_invMass << std::endl;
            body->m_force = fungt::Vec3(0, 0, 0);
            return;
        }

        body->m_vel += acceleration * dt;
        body->m_pos += body->m_vel * dt;

        // SAFETY: Validate position and velocity after integration
        if (!std::isfinite(body->m_pos.x) || !std::isfinite(body->m_pos.y) || !std::isfinite(body->m_pos.z)) {
            std::cerr << "ERROR: NaN in position after linear integration! Vel: (" << body->m_vel.x << ", " << body->m_vel.y << ", " << body->m_vel.z << ")" << std::endl;
            // Revert position by subtracting the bad velocity delta
            body->m_pos -= body->m_vel * dt;
            body->m_vel = fungt::Vec3(0, 0, 0);
            // If still NaN, something is very wrong - just stop the body
            if (!std::isfinite(body->m_pos.x) || !std::isfinite(body->m_pos.y) || !std::isfinite(body->m_pos.z)) {
                std::cerr << "CRITICAL: Cannot recover position! Stopping body." << std::endl;
                body->m_vel = fungt::Vec3(0, 0, 0);
                body->m_angularVel = fungt::Vec3(0, 0, 0);
                return;
            }
        }
        if (!std::isfinite(body->m_vel.x) || !std::isfinite(body->m_vel.y) || !std::isfinite(body->m_vel.z)) {
            std::cerr << "ERROR: NaN in velocity after linear integration!" << std::endl;
            body->m_vel = fungt::Vec3(0, 0, 0);
        }

        // Angular motion
        fungt::Vec3 angularAcceleration = body->m_invInertiaTensor * body->m_torque;

        // SAFETY: Check for NaN in angular acceleration
        if (!std::isfinite(angularAcceleration.x) || !std::isfinite(angularAcceleration.y) || !std::isfinite(angularAcceleration.z)) {
            std::cerr << "ERROR: NaN in angular acceleration!" << std::endl;
            body->m_torque = fungt::Vec3(0, 0, 0);
            return;
        }

        body->m_angularVel += angularAcceleration * dt;

        // Update orientation (simplified - should use quaternions for stability)
        // For small rotations, this approximation works
        if (body->m_angularVel.length() > 1e-6f) {  // Added minimum threshold
            float angularSpeed = body->m_angularVel.length();

            // SAFETY: Prevent division by very small numbers
            if (angularSpeed < 1e-6f) {
                body->m_torque = fungt::Vec3(0, 0, 0);
                return;
            }

            fungt::Vec3 axis = body->m_angularVel*(1.0f / angularSpeed);

            // SAFETY: Check if axis is valid
            if (!std::isfinite(axis.x) || !std::isfinite(axis.y) || !std::isfinite(axis.z)) {
                std::cerr << "ERROR: NaN in rotation axis! angularVel: (" << body->m_angularVel.x << ", " << body->m_angularVel.y << ", " << body->m_angularVel.z << "), speed: " << angularSpeed << std::endl;
                body->m_angularVel = fungt::Vec3(0, 0, 0);
                body->m_torque = fungt::Vec3(0, 0, 0);
                return;
            }

            float angle = angularSpeed * dt;
            Quaternion deltaRotation = Quaternion::fromAxisAngle(axis, angle);

            body->m_orientation = deltaRotation * body->m_orientation;
            body->m_orientation = body->m_orientation.normalize(); // Keep normalized to prevent drift

            // Simple rotation matrix update (for small angles)
            // In a full implementation, you'd use quaternions
            body->updateInertiaTensors();
        }

        // Clear forces and torques for next frame
        body->m_force = fungt::Vec3(0, 0, 0);
        body->m_torque = fungt::Vec3(0, 0, 0);
    }
    
    std::string getName() const override { return "Euler"; }



};

#endif // _EULER_INTEGRATOR_H_
