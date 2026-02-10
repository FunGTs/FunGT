#if !defined(_CONTACT_HELPERS_H_)
#define _CONTACT_HELPERS_H_
#include "../Contact/contact.hpp"
class ContactHelpers{

public:
static void resolveContactPBD(Contact& contact) {
    auto bodyA = contact.getBodyA();
    auto bodyB = contact.getBodyB();
    if (!bodyA || !bodyB) return;
    
    fungt::Vec3 normal = contact.colissionNormal;
    fungt::Vec3 contactPoint = contact.colissionPoint;
    
    // ----------------------------
    // Position Constraint: Eliminate ALL penetration
    // ----------------------------
    float penetration = contact.penetrationDepth;
    if (penetration > 0.0f) {
        float totalInvMass = bodyA->m_invMass + bodyB->m_invMass;
        if (totalInvMass > 0.0f) {
            // NO PERCENT - just fix it completely
            fungt::Vec3 correction = normal * penetration;
            
            if (!bodyA->isStatic()) {
                bodyA->m_pos += correction * (bodyA->m_invMass / totalInvMass);
            }
            if (!bodyB->isStatic()) {
                bodyB->m_pos -= correction * (bodyB->m_invMass / totalInvMass);
            }
        }
    }
    
    // ----------------------------
    // Velocity Constraint: Handle restitution/bounce
    // ----------------------------
    fungt::Vec3 rA = contactPoint - bodyA->m_pos;
    fungt::Vec3 rB = contactPoint - bodyB->m_pos;
    fungt::Vec3 velA = bodyA->m_vel + bodyA->m_angularVel.cross(rA);
    fungt::Vec3 velB = bodyB->m_vel + bodyB->m_angularVel.cross(rB);
    fungt::Vec3 relVel = velA - velB;
    float velAlongNormal = relVel.dot(normal);
    
    if (velAlongNormal > 0.0f) return; // Separating
    
    float restitution = std::min(bodyA->m_restitution, bodyB->m_restitution);
    if (std::fabs(velAlongNormal) < 1.0f) restitution = 0.0f;
    
    float invMassSum = bodyA->m_invMass + bodyB->m_invMass +
                       calculateAngularFactor(bodyA.get(), bodyB.get(), contactPoint, normal);
    
    if (invMassSum == 0.0f) return;
    
    float j = -(1 + restitution) * velAlongNormal / invMassSum;
    fungt::Vec3 impulse = normal * j;
    
    if (!bodyA->isStatic()) bodyA->m_vel += impulse * bodyA->m_invMass;
    if (!bodyB->isStatic()) bodyB->m_vel -= impulse * bodyB->m_invMass;
    
    applyAngularImpulse(bodyA.get(), bodyB.get(), contactPoint, impulse);
    
    // ----------------------------
    //  Friction
    // ----------------------------
    applyFriction(contact, j);
}
static void resolveContactEx(Contact& contact) {
    auto bodyA = contact.getBodyA();
    auto bodyB = contact.getBodyB();
    if (!bodyA || !bodyB) return;

    fungt::Vec3 normal = contact.colissionNormal;
    fungt::Vec3 contactPoint = contact.colissionPoint;

    // CRITICAL: If penetration is extreme, skip this contact (bodies are in invalid state)
    const float MAX_PENETRATION = 5.0f;  // 5 units is way too much
    if (contact.penetrationDepth > MAX_PENETRATION) {
        std::cerr << "WARNING: Extreme penetration " << contact.penetrationDepth << " detected! Skipping contact." << std::endl;
        return;
    }

    // ----------------------------
    // Separate overlapping bodies
    // ----------------------------
    const float percent = 0.3f; // Penetration correction factor
    const float slop = 0.01f;   // Small tolerance
    float penetration = contact.penetrationDepth - slop;
    if (penetration > 0.0001f) {
        fungt::Vec3 correction = normal * (penetration * percent);
        if (!bodyA->isStatic()) bodyA->m_pos += correction;  // Add check!
        if (!bodyB->isStatic()) bodyB->m_pos -= correction;  // Add check!
    }

    // ----------------------------
    // Relative velocity at contact
    // ----------------------------
    fungt::Vec3 rA = contact.colissionPoint - bodyA->m_pos;
    fungt::Vec3 rB = contact.colissionPoint - bodyB->m_pos;
    fungt::Vec3 velA = bodyA->m_vel + bodyA->m_angularVel.cross(rA);
    fungt::Vec3 velB = bodyB->m_vel + bodyB->m_angularVel.cross(rB);
    fungt::Vec3 relVel = velA - velB;
    float velAlongNormal = relVel.dot(normal);

    // ----------------------------
    // Skip if separating
    // ----------------------------
    if (velAlongNormal > 0.0f) return;

    // ----------------------------
    //  Restitution (bounce)
    // ----------------------------
    float restitution = std::min(bodyA->m_restitution, bodyB->m_restitution);
    if (std::fabs(velAlongNormal) < 1.0f) restitution = 0.0f;

    // ---------------------------
    // Impulse magnitude (linear + angular)
    // ----------------------------
    float invMassSum = bodyA->m_invMass + bodyB->m_invMass +
                       calculateAngularFactor(bodyA.get(), bodyB.get(), contactPoint, normal);

    if (invMassSum <= 0.0001f) return;  // Safety check for near-zero or invalid values

    float j = -(1 + restitution) * velAlongNormal / invMassSum;

    // Safety check: prevent NaN
    if (!std::isfinite(j)) return;

    fungt::Vec3 impulse = normal * j;

    // Safety check: ensure impulse is finite
    if (!std::isfinite(impulse.x) || !std::isfinite(impulse.y) || !std::isfinite(impulse.z)) return;

    // Apply linear velocity
    const float MAX_VELOCITY = 100.0f;  // Clamp to reasonable velocity
    if (!bodyA->isStatic()) {
        fungt::Vec3 deltaV = impulse * bodyA->m_invMass;
        if (std::isfinite(deltaV.x) && std::isfinite(deltaV.y) && std::isfinite(deltaV.z)) {
            bodyA->m_vel += deltaV;
            // Clamp velocity to prevent explosion
            float speed = bodyA->m_vel.length();
            if (speed > MAX_VELOCITY) {
                bodyA->m_vel = bodyA->m_vel * (MAX_VELOCITY / speed);
            }
        }
    }
    if (!bodyB->isStatic()) {
        fungt::Vec3 deltaV = impulse * bodyB->m_invMass;
        if (std::isfinite(deltaV.x) && std::isfinite(deltaV.y) && std::isfinite(deltaV.z)) {
            bodyB->m_vel -= deltaV;
            // Clamp velocity to prevent explosion
            float speed = bodyB->m_vel.length();
            if (speed > MAX_VELOCITY) {
                bodyB->m_vel = bodyB->m_vel * (MAX_VELOCITY / speed);
            }
        }
    }

    // Apply angular velocity
    applyAngularImpulse(bodyA.get(), bodyB.get(), contactPoint, impulse);

    // ----------------------------
    // Friction
    // ----------------------------
    float normalImpulseMagnitude = j;
    applyFriction(contact, normalImpulseMagnitude);

    // ----------------------------
    //  Optional sleep for small velocities
    // ----------------------------
    const float sleepThreshold = 0.05f;
    if (normal.y > 0.9f) { // Only ground-like contacts
        if (std::fabs(bodyA->m_vel.y) < sleepThreshold) bodyA->m_vel.y = 0.0f;
        if (std::fabs(bodyB->m_vel.y) < sleepThreshold) bodyB->m_vel.y = 0.0f;
    }
}
// In ContactHelpers
static void resolveSplitPenetration(Contact& contact) {
    auto bodyA = contact.getBodyA();
    auto bodyB = contact.getBodyB();
    if (!bodyA || !bodyB) return;

    float penetration = contact.penetrationDepth;
    if (penetration <= 0.01f) return;  // Small tolerance

    fungt::Vec3 normal = contact.colissionNormal;

    // Calculate how much to push apart
    float totalInvMass = bodyA->m_invMass + bodyB->m_invMass;
    if (totalInvMass == 0.0f) return;

    // Split impulse: add to pushVelocity instead of directly modifying position
    float correctionPerIteration = penetration * 0.2f;  // 20% per iteration
    fungt::Vec3 pushPerBody = normal * correctionPerIteration;

    // Add to correction velocity (accumulates across multiple contacts!)
    if (!bodyA->isStatic()) {
        bodyA->m_pushVelocity += pushPerBody * (bodyA->m_invMass / totalInvMass);
    }
    if (!bodyB->isStatic()) {
        bodyB->m_pushVelocity -= pushPerBody * (bodyB->m_invMass / totalInvMass);
    }
}
static void resolveContact(Contact& contact) {
    auto bodyA = contact.getBodyA();
    auto bodyB = contact.getBodyB();

    // Check if bodies are still valid
    if (!bodyA || !bodyB) return;
    // Separate overlapping bodies
    separateBodies(contact);
    fungt::Vec3 normal = contact.colissionNormal;

    // Calculate relative m_vel at contact point
    fungt::Vec3 relativeVelocity = getRelativeVelocity(bodyA.get(), bodyB.get(), contact.colissionPoint);

    // Check if objects are separating (don't resolve if they're already moving apart)
    float m_velAlongNormal = relativeVelocity.dot(normal);
    if (m_velAlongNormal > 0) return;

    // Calculate restitution (bounciness)
    //float restitution = std::min(bodyA->m_restitution, bodyB->m_restitution);
    float restitution = std::min(bodyA->m_restitution, bodyB->m_restitution);
    const float restitutionVelocityThreshold = 1.f; // tweak 0.5–2.0
    if (std::fabs(m_velAlongNormal) < restitutionVelocityThreshold) {
        restitution = 0.0f; // no bounce if slow
    }
    // Calculate impulse magnitude
    float impulseMagnitude = -(1 + restitution) * m_velAlongNormal;
    impulseMagnitude /= bodyA->m_invMass + bodyB->m_invMass +
                        calculateAngularFactor(bodyA.get(), bodyB.get(), contact.colissionPoint, normal);

    fungt::Vec3 impulse = normal * impulseMagnitude;

    // Apply linear impulse
    if (bodyA->m_invMass > 0) {
        bodyA->m_vel += impulse * (-bodyA->m_invMass);
    }
    if (bodyB->m_invMass > 0) {
        bodyB->m_vel += impulse * bodyB->m_invMass;
    }

    // Apply angular impulse
    applyAngularImpulse(bodyA.get(), bodyB.get(), contact.colissionPoint, impulse);

    // Apply m_friction
    applyFriction(contact, impulseMagnitude);

    
}

private:
    static void separateBodies(Contact & contact) {
        auto bodyA = contact.getBodyA();
        auto bodyB = contact.getBodyB();

        if (!bodyA || !bodyB) return;

        float totalinvMass = bodyA->m_invMass + bodyB->m_invMass;
        if (totalinvMass <= 0) return;

        // Use proper penetration correction with slop and percentage
        const float slop = 0.01;     // tolerance
        const float percent = 0.8f; // usually 20% to 80%
        
        float correctionAmount = std::max(0.0f, contact.penetrationDepth - slop);
        if (correctionAmount <= 0.0f) return; // No correction needed
        
        fungt::Vec3 correction = contact.colissionNormal * (correctionAmount / totalinvMass) * percent;

        if (bodyA->m_invMass > 0) {
            bodyA->m_pos += correction * (-bodyA->m_invMass);
        }
        if (bodyB->m_invMass > 0) {
            bodyB->m_pos += correction * bodyB->m_invMass;
        }
    }

    static fungt::Vec3 getRelativeVelocity(RigidBody* bodyA, RigidBody* bodyB, fungt::Vec3 contactPoint) {
        fungt::Vec3 velA = bodyA->m_vel;
        fungt::Vec3 velB = bodyB->m_vel;

        // Add rotational m_vel contribution
        fungt::Vec3 rA = contactPoint - bodyA->m_pos;
        fungt::Vec3 rB = contactPoint - bodyB->m_pos;

        velA += bodyA->m_angularVel.cross(rA);
        velB += bodyB->m_angularVel.cross(rB);

        return velB - velA;
    }

    static float calculateAngularFactor(RigidBody* bodyA, RigidBody* bodyB, fungt::Vec3 contactPoint, fungt::Vec3 normal) {
        fungt::Vec3 rA = contactPoint - bodyA->m_pos;
        fungt::Vec3 rB = contactPoint - bodyB->m_pos;

        fungt::Vec3 rACrossN = rA.cross(normal);
        fungt::Vec3 rBCrossN = rB.cross(normal);

        float angularFactorA = 0;
        float angularFactorB = 0;

        if (bodyA->m_invMass > 0) {
            fungt::Vec3 temp = bodyA->m_invInertiaTensorWorld * rACrossN;
            angularFactorA = rACrossN.dot(temp);
        }

        if (bodyB->m_invMass > 0) {
            fungt::Vec3 temp = bodyB->m_invInertiaTensorWorld * rBCrossN;
            angularFactorB = rBCrossN.dot(temp);
        }

        return angularFactorA + angularFactorB;
    }

    static void applyAngularImpulse(RigidBody* bodyA, RigidBody* bodyB, fungt::Vec3 contactPoint, fungt::Vec3 impulse) {
        fungt::Vec3 rA = contactPoint - bodyA->m_pos;
        fungt::Vec3 rB = contactPoint - bodyB->m_pos;

        if (bodyA->m_invMass > 0) {
            fungt::Vec3 angularImpulseA = rA.cross(impulse * -1.0f);
            bodyA->m_angularVel += bodyA->m_invInertiaTensorWorld * angularImpulseA;
        }

        if (bodyB->m_invMass > 0) {
            fungt::Vec3 angularImpulseB = rB.cross(impulse);
            bodyB->m_angularVel += bodyB->m_invInertiaTensorWorld * angularImpulseB;
        }
    }

    static void applyFriction(Contact& contact, float normalImpulseMagnitude) {
        auto bodyA = contact.getBodyA();
        auto bodyB = contact.getBodyB();

        if (!bodyA || !bodyB) return;

        fungt::Vec3 normal = contact.colissionNormal;
        fungt::Vec3 relativeVelocity = getRelativeVelocity(bodyA.get(), bodyB.get(), contact.colissionPoint);
        fungt::Vec3 tangent = relativeVelocity - normal * relativeVelocity.dot(normal);

        if (tangent.length() < 0.001f) return;
        tangent = tangent.normalize();

        float frictionCoefficient = std::sqrt(bodyA->m_friction * bodyB->m_friction);
        float tangentImpulseMagnitude = -relativeVelocity.dot(tangent);
        tangentImpulseMagnitude /= bodyA->m_invMass + bodyB->m_invMass +
                                  calculateAngularFactor(bodyA.get(), bodyB.get(), contact.colissionPoint, tangent);

        // Clamp m_friction impulse
        float maxFrictionImpulse = frictionCoefficient * normalImpulseMagnitude;
        if (std::abs(tangentImpulseMagnitude) > maxFrictionImpulse) {
            tangentImpulseMagnitude = tangentImpulseMagnitude > 0 ? maxFrictionImpulse : -maxFrictionImpulse;
        }

        fungt::Vec3 frictionImpulse = tangent * tangentImpulseMagnitude;

        // Apply m_friction impulse
        if (bodyA->m_invMass > 0) {
            bodyA->m_vel += frictionImpulse * (-bodyA->m_invMass);
        }
        if (bodyB->m_invMass > 0) {
            bodyB->m_vel += frictionImpulse * bodyB->m_invMass;
        }

        // Apply angular m_friction impulse
        applyAngularImpulse(bodyA.get(), bodyB.get(), contact.colissionPoint, frictionImpulse);
    }






};

#endif // _CONTACT_HELPERS_H_
