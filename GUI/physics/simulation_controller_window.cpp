#include "simulation_controller_window.hpp"


namespace fungt {

    // RigidBodySnapshot implementation
    RigidBodySnapshot::RigidBodySnapshot(std::shared_ptr<RigidBody> rb)
        : body(rb)
    {
        // Capture current state
        pos = rb->m_pos;
        vel = rb->m_vel;
        angularVel = rb->m_angularVel;
        force = rb->m_force;
        torque = rb->m_torque;
        orientation = rb->m_orientation;
    }

    void RigidBodySnapshot::restore() {
        if (!body) return;

        // Restore saved state
        body->m_pos = pos;
        body->m_vel = vel;
        body->m_angularVel = angularVel;
        body->m_force = force;
        body->m_torque = torque;
        body->m_orientation = orientation;

        // Clear any accumulated correction velocities
        body->m_pushVelocity = Vec3(0, 0, 0);
        body->m_turnVelocity = Vec3(0, 0, 0);

        // Update inertia tensors after orientation change
        body->updateInertiaTensors();
    }

} // namespace fungt