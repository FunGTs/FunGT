#if !defined(_RIGID_BODY_H_)
#define _RIGID_BODY_H_
#include<memory>
#include "../Shapes/shape.hpp"
#include "../../Quaternion/quaternion.hpp"
#include<iostream>
class RigidBody{

public:
    fungt::Vec3 m_pos;
    fungt::Vec3 m_vel;
    fungt::Vec3 m_force;

    float m_mass; 
    float m_invMass;

    // Angular properties
    fungt::Vec3     m_angularVel;
    fungt::Vec3     m_torque;
    fungt::Matrix3f m_inertiaTensor;
    fungt::Matrix3f m_invInertiaTensor;
    fungt::Matrix3f m_inertiaTensorWorld;        // Inertia tensor in world space (updated each frame)
    fungt::Matrix3f m_invInertiaTensorWorld;     // Inverse inertia tensor in world space (updated each frame)
    Quaternion m_orientation;

     // Shape and material properties
    std::unique_ptr<Shape> m_shape;
    float m_restitution; // bounciness (0 = no bounce, 1 = perfect bounce)
    float m_friction;
    
    RigidBody(std::unique_ptr<Shape> _shape, float _mass, float _rest = 0.6f, float _frict = 0.3f) : m_pos(0, 0, 0), m_vel(0, 0, 0), m_force(0, 0, 0),
          m_mass(_mass), m_angularVel(0, 0, 0), m_torque(0, 0, 0),
          m_shape(std::move(_shape)), m_restitution(_rest), m_friction(_frict) {
        
        if (m_mass > 0) {
            m_invMass = 1.0f / m_mass;
            m_inertiaTensor = m_shape->getInertiaMatrix(m_mass);
            // For simplicity, assume inverse is diagonal (works for our shapes)
            m_invInertiaTensor.m[0][0] = 1.0f / m_inertiaTensor.m[0][0];
            m_invInertiaTensor.m[1][1] = 1.0f / m_inertiaTensor.m[1][1];
            m_invInertiaTensor.m[2][2] = 1.0f / m_inertiaTensor.m[2][2];

            // Initialize world space tensors
            updateInertiaTensors();
            //print data:
            std::cout<<"Inertia Tensor : \n";
            for(int i=0; i<3; i++){
                for(int j=0; j<3; j++){
                    std::cout<<m_inertiaTensor.m[i][j]<<" ";
                }
                std::cout<<"\n";
            }
            std::cout<<"Inverse Inertia Tensor : \n";
            for(int i=0; i<3; i++){
                for(int j=0; j<3; j++){
                    std::cout<<m_invInertiaTensor.m[i][j]<<" ";
                }       
                std::cout<<"\n";
            }    
        } else {
            // Static body
            m_invMass = 0;
        }
    }


     // Apply force at center of mass
    void applyForce(const fungt::Vec3& f) {
        m_force += f;
    }
    
    // Apply force at a specific point (creates torque)
    void applyForceAtPoint(const fungt::Vec3& f, const fungt::Vec3& point) {
        m_force += f;
        fungt::Vec3 r = point - m_pos;
        m_torque += r.cross(f);
    }
    
    // Apply impulse (instant velocity change)
    void applyImpulse(const fungt::Vec3& impulse) {
        m_vel += impulse * m_invMass;
    }
    
    // Apply angular impulse
    void applyAngularImpulse(const fungt::Vec3& angularImpulse) {
        m_angularVel += m_invInertiaTensor * angularImpulse;
    }
     void updateInertiaTensors() {
        fungt::Matrix3f rotMatrix = m_orientation.toMatrix();
        fungt::Matrix3f rotMatrixT = rotMatrix.transpose();
        
        // I_world = R * I_local * R^T
        m_inertiaTensorWorld = rotMatrix * m_inertiaTensor * rotMatrixT;
        m_invInertiaTensorWorld = rotMatrix * m_invInertiaTensor * rotMatrixT;
    }
    // Check if body is static
    bool isStatic() const {
        return m_invMass == 0;
    }
    
    // Get current kinetic energy
    float getKineticEnergy() const {
        float linear = 0.5f * m_mass * m_vel.dot(m_vel);
        float angular = 0.5f * m_angularVel.dot(m_inertiaTensor * m_angularVel);
        return linear + angular;
    }
     // Get current orientation as Euler angles (for debugging/display)
    fungt::Vec3 getEulerAngles() const {
        fungt::Matrix3f rot = m_orientation.toMatrix();
        
        // Extract Euler angles (YXZ order)
        float y = std::atan2(rot.m[0][2], rot.m[2][2]);
        float x = std::atan2(-rot.m[1][2], std::sqrt(rot.m[1][0]*rot.m[1][0] + rot.m[1][1]*rot.m[1][1]));
        float z = std::atan2(rot.m[1][0], rot.m[1][1]);
        
        return fungt::Vec3(x, y, z);
    }

};


#endif // _RIGID_BODY_H_
