#if !defined(_CONTACT_HELPERS_H_)
#define _CONTACT_HELPERS_H_
#include "../Contact/contact.hpp"
class ContactHelpers{

public:
static void resolveContactEx2(Contact& contact) {
    auto bodyA = contact.getBodyA();
    auto bodyB = contact.getBodyB();
    if (!bodyA || !bodyB) return;

    fungt::Vec3 normal = contact.colissionNormal;
    fungt::Vec3 contactPoint = contact.colissionPoint;

    // ----------------------------
    // 1️⃣ Separate overlapping bodies
    // ----------------------------
    const float percent = 0.2f; // Penetration correction factor
    const float slop = 0.01f;   // Small tolerance
    float penetration = contact.penetrationDepth - slop;
    if (penetration > 0.0f) {
        fungt::Vec3 correction = normal * (penetration * percent);
        bodyA->m_pos += correction;
        bodyB->m_pos -= correction;
    }

    // ----------------------------
    // 2️⃣ Relative velocity at contact
    // ----------------------------
    fungt::Vec3 rA = contact.colissionPoint - bodyA->m_pos;
    fungt::Vec3 rB = contact.colissionPoint - bodyB->m_pos;
    fungt::Vec3 velA = bodyA->m_vel + bodyA->m_angularVel.cross(rA);
    fungt::Vec3 velB = bodyB->m_vel + bodyB->m_angularVel.cross(rB);
    fungt::Vec3 relVel = velA - velB;
    float velAlongNormal = relVel.dot(normal);

    // ----------------------------
    // 3️⃣ Skip if separating
    // ----------------------------
    if (velAlongNormal > 0.0f) return;

    // ----------------------------
    // 4️⃣ Restitution (bounce)
    // ----------------------------
    float restitution = std::min(bodyA->m_restitution, bodyB->m_restitution);
    if (std::fabs(velAlongNormal) < 1.0f) restitution = 0.0f;

    // ----------------------------
    // 5️⃣ Impulse magnitude (linear + angular)
    // ----------------------------
    float invMassSum = bodyA->m_invMass + bodyB->m_invMass +
                       calculateAngularFactor(bodyA.get(), bodyB.get(), contactPoint, normal);

    if (invMassSum == 0.0f) return;

    float j = -(1 + restitution) * velAlongNormal / invMassSum;

    fungt::Vec3 impulse = normal * j;

    // Apply linear velocity
    if (!bodyA->isStatic()) bodyA->m_vel += impulse * bodyA->m_invMass;
    if (!bodyB->isStatic()) bodyB->m_vel -= impulse * bodyB->m_invMass;

    // Apply angular velocity
    applyAngularImpulse(bodyA.get(), bodyB.get(), contactPoint, impulse);

    // ----------------------------
    // 6️⃣ Friction
    // ----------------------------
    float normalImpulseMagnitude = j;
    applyFriction(contact, normalImpulseMagnitude);

    // ----------------------------
    // 7️⃣ Optional sleep for small velocities
    // ----------------------------
    const float sleepThreshold = 0.05f;
    if (normal.y > 0.9f) { // Only ground-like contacts
        if (std::fabs(bodyA->m_vel.y) < sleepThreshold) bodyA->m_vel.y = 0.0f;
        if (std::fabs(bodyB->m_vel.y) < sleepThreshold) bodyB->m_vel.y = 0.0f;
    }
}

  static void resolveContactEx(Contact& contact) {
    auto bodyA = contact.getBodyA();
    auto bodyB = contact.getBodyB();
    if (!bodyA || !bodyB) return;

    fungt::Vec3 normal = contact.colissionNormal;
    const fungt::Vec3 contactPoint = contact.colissionPoint;
    // ----------------------------
    // 1️⃣ Separate overlapping bodies
    // ----------------------------
    const float percent = 0.2f;   // Penetration correction factor
    const float slop = 0.01f;     // Small tolerance to avoid jitter
    float penetration = contact.penetrationDepth - slop;
    if (penetration > 0.0f) {
        fungt::Vec3 correction = normal * (penetration * percent);
        bodyA->m_pos += correction;
        bodyB->m_pos -= correction;
    }

    // ----------------------------
    // 2️⃣ Compute relative velocity
    // ----------------------------
    //fungt::Vec3 relVel = bodyA->m_vel - bodyB->m_vel;
    fungt::Vec3 rA = contactPoint - bodyA->m_pos;
    fungt::Vec3 rB = contactPoint - bodyB->m_pos;
    fungt::Vec3 relVel = (bodyA->m_vel + bodyA->m_angularVel.cross(rA)) - 
                         (bodyB->m_vel + bodyB->m_angularVel.cross(rB));
    float velAlongNormal = relVel.dot(normal);
    //fungt::Vec3 relVel = getRelativeVelocity(bodyA.get(), bodyB.get(), contact.colissionPoint);
    //float velAlongNormal = relVel.dot(normal);

    // ----------------------------
    // 3️⃣ Skip if separating
    // ----------------------------
    if (velAlongNormal > 0.0f) return;

    // ----------------------------
    // 4️⃣ Restitution (bounce)
    // ----------------------------
    float restitution = std::min(bodyA->m_restitution, bodyB->m_restitution);
    if (std::fabs(velAlongNormal) < 1.0f) restitution = 0.0f; // tiny collisions

    // Impulse scalar
    float invMassSum = bodyA->m_invMass + bodyB->m_invMass;
    if (invMassSum == 0.0f) return; // Both static

    float j = -(1 + restitution) * velAlongNormal;
    j /= invMassSum;

    // Apply impulse
    fungt::Vec3 impulse = normal * j;
    bodyA->m_vel += impulse * bodyA->m_invMass;
    bodyB->m_vel -= impulse * bodyB->m_invMass;

    // Apply angular impulse
    applyAngularImpulse(bodyA.get(), bodyB.get(), contact.colissionPoint, impulse);

    // ----------------------------
    // 5️⃣ Apply friction
    // ----------------------------
    applyFriction(contact, j);


    // ----------------------------
    // 5️⃣ Sleep tiny velocities to stop jitter
    // ----------------------------
    const float sleepThreshold = 0.05f;
    if (normal.y > 0.9f) { // Only ground-like contacts
        if (std::fabs(bodyA->m_vel.y) < sleepThreshold) bodyA->m_vel.y = 0.0f;
        if (std::fabs(bodyB->m_vel.y) < sleepThreshold) bodyB->m_vel.y = 0.0f;
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
            fungt::Vec3 temp = bodyA->m_invInertiaTensor * rACrossN;
            angularFactorA = rACrossN.dot(temp);
        }

        if (bodyB->m_invMass > 0) {
            fungt::Vec3 temp = bodyB->m_invInertiaTensor * rBCrossN;
            angularFactorB = rBCrossN.dot(temp);
        }

        return angularFactorA + angularFactorB;
    }

    static void applyAngularImpulse(RigidBody* bodyA, RigidBody* bodyB, fungt::Vec3 contactPoint, fungt::Vec3 impulse) {
        fungt::Vec3 rA = contactPoint - bodyA->m_pos;
        fungt::Vec3 rB = contactPoint - bodyB->m_pos;

        if (bodyA->m_invMass > 0) {
            fungt::Vec3 angularImpulseA = rA.cross(impulse * -1.0f);
            bodyA->m_angularVel += bodyA->m_invInertiaTensor * angularImpulseA;
        }

        if (bodyB->m_invMass > 0) {
            fungt::Vec3 angularImpulseB = rB.cross(impulse);
            bodyB->m_angularVel += bodyB->m_invInertiaTensor * angularImpulseB;
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
