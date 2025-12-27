#if !defined(_COLLISION_MANAGER_H_)
#define _COLLISION_MANAGER_H_
#include <vector>
#include <map>
#include <memory>
#include "Physics/ContactHelpers/contact_helpers.hpp"
#include "Physics/Collisions/simple_collision.hpp"
#include "Physics/Collisions/manifold_collision.hpp"
#include "Physics/Contact/contact_manifold.hpp"
#include "Physics/RigidBody/rigid_body.hpp"
#include "Physics/BroadPhase/uniform_grid.hpp"

// Helper to create unique key for body pairs
struct BodyPairKey {
    size_t bodyA_id;
    size_t bodyB_id;

    BodyPairKey(size_t a, size_t b);
    bool operator<(const BodyPairKey& other) const;
};

class CollisionManager {
private:
    std::vector<std::shared_ptr<RigidBody>> m_collidableBodies;
    std::unique_ptr<UniformGrid> m_broadPhase;
    std::map<BodyPairKey, ContactManifold> m_manifoldCache;

    // Helper methods for collision manifold detection
    void warmStartManifold(ContactManifold& newManifold, const ContactManifold& existingManifold);
    void solveContactImpulse(ContactPoint& cp, std::shared_ptr<RigidBody> bodyA,
                            std::shared_ptr<RigidBody> bodyB, float dt, float ERP);
    void solveFriction(ContactPoint& cp, std::shared_ptr<RigidBody> bodyA,
                      std::shared_ptr<RigidBody> bodyB, const fungt::Vec3& rA,
                      const fungt::Vec3& rB, float effectiveMass);

public:
    CollisionManager();

    void add(std::shared_ptr<RigidBody> body);
    int getNumOfCollidableObjects();
    void remove(std::shared_ptr<RigidBody> body);
    std::shared_ptr<RigidBody> getCollideBody(size_t index) const;
    const std::vector<std::shared_ptr<RigidBody>>& getCollidable() const;
    std::vector<std::shared_ptr<RigidBody>>& getCollidable();

    void detectCollisions();
    void detectCollisionsUg();
    void detectCollisionsEx();
    void detectCollisionsManifold();
    void clearManifoldCache();
};
#endif // _COLLISION_MANAGER_H_
