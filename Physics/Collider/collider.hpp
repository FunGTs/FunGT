#if !defined(_COLLIDER_H_)
#define _COLLIDER_H_
#include "../Collisions/simple_collision.hpp"
class Collider{

    public:
       std::vector<std::shared_ptr<RigidBody>> m_bodies;
       
       Collider();
       
       
       void addCollisionBody(std::unique_ptr<RigidBody> body);
       void findCollisions(Contact &contact);
       void resolveCollisions(Contact &contact);
       void run();




};

#endif // _COLLIDER_H_
