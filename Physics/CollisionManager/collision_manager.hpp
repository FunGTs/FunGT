#if !defined(_COLLISION_MANAGER_H_)
#define _COLLISION_MANAGER_H_
#include <vector>
#include<algorithm>
#include "../ContactHelpers/contact_helpers.hpp"
#include "../Collisions/simple_collision.hpp"
#include "../RigidBody/rigid_body.hpp"
class CollisionManager {
private:
    std::vector<std::shared_ptr<RigidBody>> m_collidableBodies;

    
public:
    void add(std::shared_ptr<RigidBody> body) {
        m_collidableBodies.push_back(body);  // Converts to weak_ptr automatically
    }
    int getNumOfCollidableObjects(){
        
        int num = static_cast<int>(m_collidableBodies.size());

        return num;
    }
     // Remove body
    void remove(std::shared_ptr<RigidBody> body) {
        m_collidableBodies.erase(
            std::remove_if(
                m_collidableBodies.begin(),
                m_collidableBodies.end(),
                [&body](const std::shared_ptr<RigidBody>& shared) {
                    return shared == body;
                }),
            m_collidableBodies.end()
        );
    }
    // Access a body by index (returns shared_ptr)
    std::shared_ptr<RigidBody> getCollideBody(size_t index) const {
        if (index >= m_collidableBodies.size()) return nullptr; // bounds check
        return m_collidableBodies[index];  // return shared_ptr
    }
        // Getter (const)
    const std::vector<std::shared_ptr<RigidBody>>& getCollidable() const {
        return m_collidableBodies;
    }

    // Getter (non-const, allows modification)
    std::vector<std::shared_ptr<RigidBody>>& getCollidable() {
        return m_collidableBodies;
    }
    void detectCollisions() {
        //std::cout<<"Detecting Collisions among "<<m_collidableBodies.size()<<" objects.\n";
        std::vector<Contact> contacts;
        // Check collisions between valid bodies
        for (size_t i = 0; i < m_collidableBodies.size(); ++i) {
            for (size_t j = i + 1; j < m_collidableBodies.size(); ++j) {

                auto bodyA = m_collidableBodies[i];
                auto bodyB = m_collidableBodies[j];
                
                if (!bodyA || !bodyB || (bodyA->isStatic() && bodyB->isStatic())){
                    
                    continue;  // Skip if either is nullptr
  
                }               
                
                auto collision = SimpleCollision::Detect(bodyA, bodyB); // returns std::optional
                if (collision && collision->isValid()) {
                    //Add the collision to the list of contacts
                    //print some info
                    //std::cout<<"Collision detected between bodies at point ("<<collision->colissionPoint.x<<", "
                    //<<collision->colissionPoint.y<<", "<<collision->colissionPoint.z<<") with normal ("
                    //<<collision->colissionNormal.x<<", "<<collision->colissionNormal.y<<", "<<collision->colissionNormal.z
                    //<<") and penetration depth "<<collision->penetrationDepth<<"\n";
                    contacts.push_back(collision.value()); //.-value() returns a reference to the contained value
                }
            }
        }
        for(auto & _contact : contacts){
            ContactHelpers::resolveContactEx2(_contact);
        }
    }
    
};
#endif // _COLLISION_MANAGER_H_
