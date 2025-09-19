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
        fungl::Vec3 acceleration = body->m_force * body->m_invMass;
        // Debug prints
        //inverse mass and force
        //printf("Inv Mass: %.4f\n", body->m_invMass);
       // printf("Force: (%.2f, %.2f, %.2f)\n", body->m_force.x, body->m_force.y, body->m_force.z);
       // printf("Lin Acc : %.4f %.4f %.4f\n", acceleration.x, acceleration.y, acceleration.z);
        //if(body->m_shape->GetType() == ShapeType::BOX)
        //std::cout<<"Lin Acc : " <<acceleration.x <<std::endl;
        body->m_vel += acceleration * dt;
        body->m_pos += body->m_vel * dt;
        //print delta time
       // printf("Delta Time: %.4f\n", dt);
       // printf("Position: (%.2f, %.2f, %.2f)\n", body->m_pos.x, body->m_pos.y, body->m_pos.z);
       // printf("Velocity: (%.2f, %.2f, %.2f)\n", body->m_vel.x, body->m_vel.y, body->m_vel.z);
        // Angular motion
        fungl::Vec3 angularAcceleration = body->m_invInertiaTensor * body->m_torque;
        //std::cout<<"Ang Acc : " <<angularAcceleration.x <<std::endl;
        body->m_angularVel += angularAcceleration * dt;
        
        // Update orientation (simplified - should use quaternions for stability)
        // For small rotations, this approximation works
        if (body->m_angularVel.length() > 0) {
            float angularSpeed = body->m_angularVel.length();
            fungl::Vec3 axis = body->m_angularVel*(1.0f / angularSpeed);
            float angle = angularSpeed * dt;
            Quaternion deltaRotation = Quaternion::fromAxisAngle(axis, angle);

            body->m_orientation = deltaRotation * body->m_orientation;
            body->m_orientation = body->m_orientation.normalize(); // Keep normalized to prevent drift
            
            // Simple rotation matrix update (for small angles)
            // In a full implementation, you'd use quaternions
            body->updateInertiaTensors();
        }
        
        // Clear forces and torques for next frame
        body->m_force = fungl::Vec3(0, 0, 0);
        body->m_torque = fungl::Vec3(0, 0, 0);
    }
    
    std::string getName() const override { return "Euler"; }



};

#endif // _EULER_INTEGRATOR_H_
