#if !defined(_PHYSICS_WORLD_H_)
#define _PHYSICS_WORLD_H_
#include <vector>
#include <memory>
#include <iostream>
#include "../Shapes/sphere.hpp"
#include "../Shapes/box.hpp"
#include "../RigidBody/rigid_body.hpp"
#include "../Integrators/euler_integrator.hpp"
#include "../CollisionManager/collision_manager.hpp"


// Simple physics world to manage rigid bodies
class PhysicsWorld {
private:
    std::shared_ptr<CollisionManager> m_collisionManager;
    std::unique_ptr<Integrator> m_integrator;
    fungt::Vec3 gravity;
    
public:
    PhysicsWorld(fungt::Vec3 g = fungt::Vec3(0, -9.81f, 0)) : gravity(g) {
        SimpleCollision::Init();
        //std::unique_ptr<Integrator> m_integrator = std::make_unique<EulerIntegrator>(); //Default Integrator
        m_integrator = std::make_unique<EulerIntegrator>(); //Default Integrator
        m_collisionManager = std::make_shared<CollisionManager>();
    }
    void setIntegrator(std::unique_ptr<Integrator> _integrator){
        m_integrator = std::move(_integrator);
    }
    void runColliders(float dt){
        
        if(0 == m_collisionManager->getNumOfCollidableObjects()){
            std::cout<<"No collision objects added, exiting .. \n";
            return;
        }

        // Fixed physics timestep (for stable simulation)
        constexpr float physicsTimeStep = 1.0f / 60.0f; // 60 Hz
        static float accumulator = 0.0f;

        accumulator += dt;

        while (accumulator >= physicsTimeStep) {
            // Apply forces (like gravity)
            for (auto &body : m_collisionManager->getCollidable()) {
                if (body && !body->isStatic()) {
                    body->applyForce(gravity * body->m_mass);
                }
            }

            // Integrate all bodies
            for (auto &body : m_collisionManager->getCollidable()) {
                if (!body || body->isStatic()) continue;

                m_integrator->integrate(body, physicsTimeStep);

                // Damping
                float linearDamping = 1.f;  // Was 1.0f (no damping)
                float angularDamping = 0.98f;  // Was 0.90f

                body->m_vel = body->m_vel*linearDamping;
                body->m_angularVel = body->m_angularVel*angularDamping;

                // Stop very small velocities to avoid jitter
                if (body->m_vel.length() < 0.05f) body->m_vel = fungt::Vec3(0, 0, 0);
                if (body->m_angularVel.length() < 0.05f) body->m_angularVel = fungt::Vec3(0, 0, 0);
            }

            // Detect collisions
            m_collisionManager->detectCollisions();

            accumulator -= physicsTimeStep;
        }
              
    }
    std::shared_ptr<CollisionManager> getCollisionManager(){
       return m_collisionManager; 
    }
    
};
typedef std::shared_ptr<CollisionManager> spCollisionManager;

#endif // _PHYSICS_WORLD_H_
