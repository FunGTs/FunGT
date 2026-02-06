#include "collision_manager.hpp"
#include <algorithm>

// BodyPairKey implementation
BodyPairKey::BodyPairKey(size_t a, size_t b) {
    if (a < b) {
        bodyA_id = a;
        bodyB_id = b;
    } else {
        bodyA_id = b;
        bodyB_id = a;
    }
}

bool BodyPairKey::operator<(const BodyPairKey& other) const {
    if (bodyA_id != other.bodyA_id) return bodyA_id < other.bodyA_id;
    return bodyB_id < other.bodyB_id;
}

// CollisionManager implementation
CollisionManager::CollisionManager() {
    fungt::Vec3 worldMin(-100, -100, -100);
    fungt::Vec3 worldMax(100, 100, 100);
    float cellSize = 5.0f;
    m_broadPhase = std::make_unique<UniformGrid>(worldMin, worldMax, cellSize);
}

void CollisionManager::add(std::shared_ptr<RigidBody> body) {
    m_collidableBodies.push_back(body);
}

int CollisionManager::getNumOfCollidableObjects() {
    return static_cast<int>(m_collidableBodies.size());
}

void CollisionManager::remove(std::shared_ptr<RigidBody> body) {
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

std::shared_ptr<RigidBody> CollisionManager::getCollideBody(size_t index) const {
    if (index >= m_collidableBodies.size()) return nullptr;
    return m_collidableBodies[index];
}

const std::vector<std::shared_ptr<RigidBody>>& CollisionManager::getCollidable() const {
    return m_collidableBodies;
}

std::vector<std::shared_ptr<RigidBody>>& CollisionManager::getCollidable() {
    return m_collidableBodies;
}

void CollisionManager::detectCollisions() {
    std::vector<Contact> contacts;

    for (size_t i = 0; i < m_collidableBodies.size(); ++i) {
        for (size_t j = i + 1; j < m_collidableBodies.size(); ++j) {
            auto bodyA = m_collidableBodies[i];
            auto bodyB = m_collidableBodies[j];

            if (!bodyA || !bodyB || (bodyA->isStatic() && bodyB->isStatic())) {
                continue;
            }

            auto collision = SimpleCollision::Detect(bodyA, bodyB);
            if (collision && collision->isValid()) {
                contacts.push_back(collision.value());
            }
        }
    }

    // Solve contacts multiple times
    for (int iteration = 0; iteration < 10; ++iteration) {
        for (auto& _contact : contacts) {
            ContactHelpers::resolveContactEx(_contact);
        }
    }
}

void CollisionManager::detectCollisionsUg() {
    std::vector<Contact> contacts;

    auto potentialPairs = m_broadPhase->getPotentialPairs(m_collidableBodies);

    for (auto [i, j] : potentialPairs) {
        auto bodyA = m_collidableBodies[i];
        auto bodyB = m_collidableBodies[j];

        if (!bodyA || !bodyB || (bodyA->isStatic() && bodyB->isStatic())) {
            continue;
        }

        auto collision = SimpleCollision::Detect(bodyA, bodyB);
        if (collision && collision->isValid()) {
            contacts.push_back(collision.value());
        }
    }

    for (int iteration = 0; iteration < 5; ++iteration) {
        for (auto& _contact : contacts) {
            ContactHelpers::resolveContactEx(_contact);
        }
    }
}

void CollisionManager::detectCollisionsEx() {
    std::vector<Contact> contacts;

    for (size_t i = 0; i < m_collidableBodies.size(); ++i) {
        for (size_t j = i + 1; j < m_collidableBodies.size(); ++j) {
            auto bodyA = m_collidableBodies[i];
            auto bodyB = m_collidableBodies[j];

            if (!bodyA || !bodyB || (bodyA->isStatic() && bodyB->isStatic())) {
                continue;
            }

            auto collision = SimpleCollision::Detect(bodyA, bodyB);
            if (collision && collision->isValid()) {
                contacts.push_back(collision.value());
            }
        }
    }

    // Velocity resolution
    for (int iter = 0; iter < 10; ++iter) {
        for (auto& contact : contacts) {
            ContactHelpers::resolveContactEx(contact);
        }
    }

    // Position correction
    for (auto& body : m_collidableBodies) {
        if (body && !body->isStatic()) {
            body->m_pushVelocity = fungt::Vec3(0, 0, 0);
        }
    }

    for (auto& contact : contacts) {
        ContactHelpers::resolveSplitPenetration(contact);
    }

    for (auto& body : m_collidableBodies) {
        if (body && !body->isStatic()) {
            body->m_pos += body->m_pushVelocity;
        }
    }
}

void CollisionManager::detectCollisionsManifold() {
    constexpr float ERP = 0.2f;
    constexpr float dt = 1.0f / 120.0f;

    // Step 1: Use broad phase to get potential collision pairs
    auto potentialPairs = m_broadPhase->getPotentialPairs(m_collidableBodies);
   // std::cout << "Broad phase found " << potentialPairs.size() << " pairs" << std::endl;  // ADD THIS
    // Step 2: Detect collisions and update/create manifolds
    for (auto [i, j] : potentialPairs) {
        auto bodyA = m_collidableBodies[i];
        auto bodyB = m_collidableBodies[j];

        if (!bodyA || !bodyB || (bodyA->isStatic() && bodyB->isStatic())) {
            continue;
        }

        BodyPairKey key(i, j);
        std::optional<ContactManifold> newManifold = ManifoldCollision::Detect(bodyA, bodyB);
        //std::cout << "  Pair (" << i << "," << j << "): ";
        if (newManifold) {
            //std::cout << "COLLISION DETECTED!" << std::endl;
            auto it = m_manifoldCache.find(key);
            if (it != m_manifoldCache.end()) {
                // Warm-start: preserve impulses from previous frame
                warmStartManifold(newManifold.value(), it->second);
                it->second = newManifold.value();
            } else {
                // New collision
                
                m_manifoldCache[key] = newManifold.value();
            }
        }
        // else{
        //     std::cout << "NO COLLISION" << std::endl;
        // }
    }

    // Step 3: Sequential impulse solver
    for (int iteration = 0; iteration < 1; ++iteration) {
        for (auto& [key, manifold] : m_manifoldCache) {
            auto bodyA = manifold.getBodyA();
            auto bodyB = manifold.getBodyB();

            if (!bodyA || !bodyB) {
                continue;
            }

            for (int i = 0; i < manifold.getNumPoints(); ++i) {
                ContactPoint& cp = manifold.getPoint(i);
                solveContactImpulse(cp, bodyA, bodyB, dt, ERP);
            }
        }
    }
}

void CollisionManager::clearManifoldCache() {
    m_manifoldCache.clear();
}

// Private helper methods

void CollisionManager::warmStartManifold(ContactManifold& newManifold, const ContactManifold& existingManifold) {
    for (int k = 0; k < newManifold.getNumPoints(); ++k) {
        ContactPoint& newPoint = newManifold.getPoint(k);

        int closestIndex = -1;
        float minDist = 0.02f;

        for (int j = 0; j < existingManifold.getNumPoints(); ++j) {
            const ContactPoint& existingPoint = const_cast<ContactManifold&>(existingManifold).getPoint(j);
            float dist = (newPoint.worldPointA - existingPoint.worldPointA).length();
            if (dist < minDist) {
                minDist = dist;
                closestIndex = j;
            }
        }

        if (closestIndex >= 0) {
            const ContactPoint& existingPoint = const_cast<ContactManifold&>(existingManifold).getPoint(closestIndex);
            newPoint.normalImpulse = existingPoint.normalImpulse;
            newPoint.tangentImpulse1 = existingPoint.tangentImpulse1;
            newPoint.tangentImpulse2 = existingPoint.tangentImpulse2;
            newPoint.lifeTime = existingPoint.lifeTime + 1;
        }
    }
}

void CollisionManager::solveContactImpulse(ContactPoint& cp, std::shared_ptr<RigidBody> bodyA,
                                          std::shared_ptr<RigidBody> bodyB, float dt, float ERP) {
    if (cp.penetrationDepth > 5.0f) {
        return;  // Skip unstable contacts
    }

    fungt::Vec3 rA = cp.worldPointA - bodyA->m_pos;
    fungt::Vec3 rB = cp.worldPointB - bodyB->m_pos;

    // Compute relative velocity at contact point
    fungt::Vec3 velA = bodyA->m_vel + bodyA->m_angularVel.cross(rA);
    fungt::Vec3 velB = bodyB->m_vel + bodyB->m_angularVel.cross(rB);
    float rel_vel = (velA - velB).dot(cp.normal);

    // Compute effective mass along contact normal
    fungt::Vec3 rACrossN = rA.cross(cp.normal);
    fungt::Vec3 rBCrossN = rB.cross(cp.normal);
    float kNormal = bodyA->m_invMass + bodyB->m_invMass;

    if (!bodyA->isStatic()) {
        fungt::Vec3 temp = bodyA->m_invInertiaTensorWorld * rACrossN;
        kNormal += rACrossN.dot(temp);
    }
    if (!bodyB->isStatic()) {
        fungt::Vec3 temp = bodyB->m_invInertiaTensorWorld * rBCrossN;
        kNormal += rBCrossN.dot(temp);
    }

    if (kNormal <= 0.0001f) {
        return;  // Invalid effective mass
    }
    float effectiveMass = 1.0f / kNormal;

    // Restitution coefficient
    float restitution = std::min(bodyA->m_restitution, bodyB->m_restitution);

    // Baumgarte stabilization for position correction
    float biasTerm = 0.0f;
    if (cp.penetrationDepth > 0.01f) {
        biasTerm = (ERP / dt) * cp.penetrationDepth;
    }

    // Velocity error + position correction
    float velocityBias = rel_vel * (1.0f + restitution);
    float totalBias = velocityBias + biasTerm;

    // Compute impulse magnitude
    float lambda = totalBias * effectiveMass;
    float deltaImpulse = lambda - cp.normalImpulse;

    // Accumulate and clamp to prevent pulling
    float sum = cp.normalImpulse + deltaImpulse;
    if (sum < 0.0f) {
        deltaImpulse = -cp.normalImpulse;
        cp.normalImpulse = 0.0f;
    } else {
        cp.normalImpulse = sum;
    }
    float velB_before = bodyB->m_vel.y;
    // Apply impulse to bodies
    fungt::Vec3 impulse = cp.normal * deltaImpulse;

    if (!bodyA->isStatic()) {
        bodyA->m_vel -= impulse * bodyA->m_invMass;
    }
    if (!bodyB->isStatic()) {
        bodyB->m_vel += impulse * bodyB->m_invMass;
    }
    // ADD THIS DEBUG:
    // static int debugCount = 0;
    // if (debugCount++ % 60 == 0) {
    //     std::cout << "IMPULSE DEBUG:" << std::endl;
    //     std::cout << "  normal=(" << cp.normal.x << "," << cp.normal.y << "," << cp.normal.z << ")" << std::endl;
    //     std::cout << "  rel_vel=" << rel_vel << std::endl;
    //     std::cout << "  velocityBias=" << velocityBias << std::endl;
    //     std::cout << "  biasTerm=" << biasTerm << std::endl;
    //     std::cout << "  deltaImpulse=" << deltaImpulse << std::endl;
    //     std::cout << "  impulse.y=" << impulse.y << std::endl;
    //     std::cout << "  velB BEFORE=" << velB_before << " AFTER=" << bodyB->m_vel.y << std::endl;
    // }
    // Solve friction after normal impulse
    solveFriction(cp, bodyA, bodyB, rA, rB, effectiveMass);
}

void CollisionManager::solveFriction(ContactPoint& cp, std::shared_ptr<RigidBody> bodyA,
                                     std::shared_ptr<RigidBody> bodyB, const fungt::Vec3& rA,
                                     const fungt::Vec3& rB, float effectiveMass) {
    fungt::Vec3 velA = bodyA->m_vel + bodyA->m_angularVel.cross(rA);
    fungt::Vec3 velB = bodyB->m_vel + bodyB->m_angularVel.cross(rB);
    float rel_vel = (velA - velB).dot(cp.normal);

    fungt::Vec3 tangentVel = (velA - velB) - cp.normal * rel_vel;
    float tangentSpeed = tangentVel.length();

    if (tangentSpeed <= 0.001f) {
        return;  // No tangential motion
    }

    fungt::Vec3 tangentDir = tangentVel / tangentSpeed;

    float friction = std::sqrt(bodyA->m_friction * bodyB->m_friction);
    float maxFriction = friction * std::abs(cp.normalImpulse);

    float tangentImpulse = -tangentSpeed * effectiveMass;
    tangentImpulse = std::max(-maxFriction, std::min(maxFriction, tangentImpulse));

    fungt::Vec3 frictionImpulse = tangentDir * tangentImpulse;

    if (!bodyA->isStatic()) {
        bodyA->m_vel += frictionImpulse * bodyA->m_invMass;
        bodyA->m_angularVel += bodyA->m_invInertiaTensorWorld * rA.cross(frictionImpulse);
    }
    if (!bodyB->isStatic()) {
        bodyB->m_vel -= frictionImpulse * bodyB->m_invMass;
        bodyB->m_angularVel -= bodyB->m_invInertiaTensorWorld * rB.cross(frictionImpulse);
    }
}
