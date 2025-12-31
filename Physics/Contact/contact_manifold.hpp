#if !defined(_CONTACT_MANIFOLD_H_)
#define _CONTACT_MANIFOLD_H_
#include <memory>
#include <array>
#include "../RigidBody/rigid_body.hpp"

// Single contact point within a manifold
struct ContactPoint {
    fungt::Vec3 localPointA;      // Contact point in body A's local space
    fungt::Vec3 localPointB;      // Contact point in body B's local space
    fungt::Vec3 worldPointA;      // Contact point in world space (on A)
    fungt::Vec3 worldPointB;      // Contact point in world space (on B)
    fungt::Vec3 normal;           // Contact normal (from A to B)
    float penetrationDepth;       // How deep objects penetrate

    float normalImpulse;          // Accumulated impulse (for warm starting)
    float tangentImpulse1;        // Friction impulse 1
    float tangentImpulse2;        // Friction impulse 2

    int lifeTime;                 // Frames this contact has existed

    ContactPoint()
        : localPointA(0,0,0), localPointB(0,0,0),
          worldPointA(0,0,0), worldPointB(0,0,0),
          normal(0,0,0), penetrationDepth(0),
          normalImpulse(0), tangentImpulse1(0), tangentImpulse2(0),
          lifeTime(0) {}

    ContactPoint(const fungt::Vec3& lpA, const fungt::Vec3& lpB,
                 const fungt::Vec3& wpA, const fungt::Vec3& wpB,
                 const fungt::Vec3& n, float depth)
        : localPointA(lpA), localPointB(lpB),
          worldPointA(wpA), worldPointB(wpB),
          normal(n), penetrationDepth(depth),
          normalImpulse(0), tangentImpulse1(0), tangentImpulse2(0),
          lifeTime(0) {}
};

// Contact manifold - stores up to 4 contact points between two bodies
class ContactManifold {
private:
    static constexpr int MAX_CONTACT_POINTS = 4;

    std::weak_ptr<RigidBody> m_bodyA;
    std::weak_ptr<RigidBody> m_bodyB;
    std::array<ContactPoint, MAX_CONTACT_POINTS> m_points;
    int m_numPoints;

    // Contact breaking/processing thresholds (matching Bullet3 defaults)
    static constexpr float CONTACT_BREAKING_THRESHOLD = 0.02f;  // Distance to remove contact (Bullet3 default)
    static constexpr float CONTACT_MERGE_THRESHOLD = 0.01f;     // Distance to merge contacts

public:
    // Default constructor (needed for std::map)
    ContactManifold() : m_numPoints(0) {}

    ContactManifold(std::weak_ptr<RigidBody> bodyA, std::weak_ptr<RigidBody> bodyB)
        : m_bodyA(bodyA), m_bodyB(bodyB), m_numPoints(0) {}

    // Getters
    std::shared_ptr<RigidBody> getBodyA() const { return m_bodyA.lock(); }
    std::shared_ptr<RigidBody> getBodyB() const { return m_bodyB.lock(); }
    int getNumPoints() const { return m_numPoints; }
    ContactPoint& getPoint(int index) { return m_points[index]; }
    const ContactPoint& getPoint(int index) const { return m_points[index]; }

    // Check if manifold is still valid
    bool isValid() const {
        return !m_bodyA.expired() && !m_bodyB.expired();
    }

    // Add a new contact point (with automatic reduction if > 4 points)
    void addContactPoint(const ContactPoint& newPoint) {
        if (m_numPoints < MAX_CONTACT_POINTS) {
            m_points[m_numPoints++] = newPoint;
        } else {
            // Manifold is full - replace least important point
            int replaceIndex = findLeastImportantPoint(newPoint);
            m_points[replaceIndex] = newPoint;
        }
    }

    // Update existing contact points with new world positions
    void refreshContactPoints() {
        auto bodyA = m_bodyA.lock();
        auto bodyB = m_bodyB.lock();
        if (!bodyA || !bodyB) {
            m_numPoints = 0;  // Clear all points if bodies are gone
            return;
        }

        // Remove old/invalid contacts and refresh positions
        int writeIndex = 0;
        for (int i = 0; i < m_numPoints; ++i) {
            ContactPoint& pt = m_points[i];

            // Safety check: ensure normal is valid
            float normalLen = pt.normal.length();
            if (normalLen < 0.0001f || !std::isfinite(normalLen)) {
                continue;  // Skip invalid contact
            }

            // Safety check: ensure body positions are valid
            if (!std::isfinite(bodyA->m_pos.x) || !std::isfinite(bodyA->m_pos.y) || !std::isfinite(bodyA->m_pos.z)) {
                continue;  // Skip if body A position is NaN
            }
            if (!std::isfinite(bodyB->m_pos.x) || !std::isfinite(bodyB->m_pos.y) || !std::isfinite(bodyB->m_pos.z)) {
                continue;  // Skip if body B position is NaN
            }

            // Transform local points to world space
            fungt::Vec3 rotatedA = bodyA->m_orientation.rotateVector(pt.localPointA);
            fungt::Vec3 rotatedB = bodyB->m_orientation.rotateVector(pt.localPointB);

            // CRITICAL SAFETY: Check if quaternion rotation produced NaN
            if (!std::isfinite(rotatedA.x) || !std::isfinite(rotatedA.y) || !std::isfinite(rotatedA.z)) {
                continue;  // Skip if rotation A produced NaN
            }
            if (!std::isfinite(rotatedB.x) || !std::isfinite(rotatedB.y) || !std::isfinite(rotatedB.z)) {
                continue;  // Skip if rotation B produced NaN
            }

            pt.worldPointA = bodyA->m_pos + rotatedA;
            pt.worldPointB = bodyB->m_pos + rotatedB;

            // Recalculate penetration depth
            fungt::Vec3 diff = pt.worldPointB - pt.worldPointA;
            float distance = diff.dot(pt.normal);

            // Keep contact only if:
            // - Negative distance (penetrating): always keep
            // - Small positive distance (separated but close): keep if within threshold
            if (distance < CONTACT_BREAKING_THRESHOLD) {
                // CRITICAL: penetration is negative of signed distance
                // If distance < 0: bodies penetrating → penetrationDepth > 0
                // If 0 < distance < 0.5: bodies separated but within threshold → penetrationDepth < 0 (but small)
                pt.penetrationDepth = -distance;
                pt.lifeTime++;

                // Move to write position if we're compacting
                if (writeIndex != i) {
                    m_points[writeIndex] = pt;
                }
                writeIndex++;
            }
            // Otherwise contact is removed (don't increment writeIndex)
        }
        m_numPoints = writeIndex;
    }

    // Clear all contact points
    void clear() {
        m_numPoints = 0;
    }

    // Merge nearby contact points
    void mergeNearbyPoints() {
        for (int i = 0; i < m_numPoints - 1; ++i) {
            for (int j = i + 1; j < m_numPoints; ++j) {
                fungt::Vec3 diff = m_points[i].worldPointA - m_points[j].worldPointA;
                if (diff.length() < CONTACT_MERGE_THRESHOLD) {
                    // Keep deeper contact
                    if (m_points[j].penetrationDepth > m_points[i].penetrationDepth) {
                        m_points[i] = m_points[j];
                    }
                    // Remove point j
                    for (int k = j; k < m_numPoints - 1; ++k) {
                        m_points[k] = m_points[k + 1];
                    }
                    m_numPoints--;
                    j--;
                }
            }
        }
    }

private:
    // Find which point to replace when manifold is full
    // Strategy: Keep deepest point + maximize contact area (like Bullet3)
    int findLeastImportantPoint(const ContactPoint& newPoint) {
        // Find deepest penetration
        int deepestIndex = 0;
        float maxPenetration = m_points[0].penetrationDepth;
        for (int i = 1; i < MAX_CONTACT_POINTS; ++i) {
            if (m_points[i].penetrationDepth > maxPenetration) {
                maxPenetration = m_points[i].penetrationDepth;
                deepestIndex = i;
            }
        }

        // Calculate contact area for each possible configuration
        // (keep new point + 3 existing, remove 1 existing)
        float maxArea = -1.0f;
        int replaceIndex = 0;

        for (int i = 0; i < MAX_CONTACT_POINTS; ++i) {
            if (i == deepestIndex) continue;  // Never replace deepest point

            // Calculate area of quadrilateral formed by:
            // newPoint + 3 existing points (excluding point i)
            float area = calculateContactArea(newPoint, i);

            if (area > maxArea) {
                maxArea = area;
                replaceIndex = i;
            }
        }

        return replaceIndex;
    }

    // Calculate contact area when replacing point 'excludeIndex' with newPoint
    float calculateContactArea(const ContactPoint& newPoint, int excludeIndex) {
        // Simple heuristic: sum of distances between points
        // (More sophisticated: actual polygon area)
        float totalDist = 0.0f;
        int count = 0;

        for (int i = 0; i < MAX_CONTACT_POINTS; ++i) {
            if (i == excludeIndex) continue;

            for (int j = i + 1; j < MAX_CONTACT_POINTS; ++j) {
                if (j == excludeIndex) continue;

                fungt::Vec3 diff = m_points[i].worldPointA - m_points[j].worldPointA;
                totalDist += diff.length();
                count++;
            }

            // Distance to new point
            fungt::Vec3 diff = m_points[i].worldPointA - newPoint.worldPointA;
            totalDist += diff.length();
            count++;
        }

        return count > 0 ? totalDist / count : 0.0f;
    }
};

#endif // _CONTACT_MANIFOLD_H_