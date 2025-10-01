#include "collider.hpp"

Collider::Collider()
{

}

void Collider::addCollisionBody(std::unique_ptr<RigidBody> body)
{
    m_bodies.push_back(std::move(body));
}

void Collider::findCollisions(Contact &contact)
{
    std::vector<Contact> collisions;
    for(int i = 0; i<m_bodies.size(); i++){

        for(int j = i+1; j<m_bodies.size(); j++){

            auto bodyA = m_bodies[i];
            auto bodyB = m_bodies[j];
            
            // Skip if either body is null or both are static
            if (!bodyA || !bodyB || (bodyA->isStatic() && bodyB->isStatic())) continue;
            
            // Use  dispatcher!
            auto collision = SimpleCollision::Detect(bodyA, bodyB);
            if (collision && collision->isValid()) {
                collisions.push_back(collision.value());
            }

        }

    }
    for(auto& collision : collisions) {
        resolveCollisions(collision);
    }

}

void Collider::resolveCollisions(Contact &contact)
{
    auto bodyA = contact.getBodyA();
    auto bodyB = contact.getBodyB();
    
    // Check if bodies are still valid
    if (!bodyA || !bodyB) return;
    
    fungt::Vec3 normal = contact.colissionNormal;
}
