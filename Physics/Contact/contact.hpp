#if !defined(_CONTACT_H_)
#define _CONTACT_H_
#include <memory>
#include "../RigidBody/rigid_body.hpp"
struct Contact {
    std::weak_ptr<RigidBody> bodyA;
    std::weak_ptr<RigidBody> bodyB;
    fungl::Vec3 colissionPoint;
    fungl::Vec3 colissionNormal;  // Points from A to B
    float penetrationDepth;
    
    Contact(std::weak_ptr<RigidBody> a, std::weak_ptr<RigidBody> b, fungl::Vec3 point, fungl::Vec3 normal, float depth)
        : bodyA(a), bodyB(b), colissionPoint(point), colissionNormal(normal), penetrationDepth(depth) {}
    
    // Helper methods to safely access bodies
    std::shared_ptr<RigidBody> getBodyA() const {
        return bodyA.lock();
    }
    
    std::shared_ptr<RigidBody> getBodyB() const {
        return bodyB.lock();
    }
    
    // Check if both bodies are still valid
    bool isValid() const {
        return !bodyA.expired() && !bodyB.expired();
    }
};

#endif // _CONTACT_H_
