#if !defined(_MANIFOLD_COLLISION_H_)
#define _MANIFOLD_COLLISION_H_

#include "../Contact/contact_manifold.hpp"
#include "../Shapes/sphere.hpp"
#include "../Shapes/box.hpp"
#include "simple_collision.hpp"
#include <optional>
#include <memory>
#include <array>

using ManifoldFunc = std::optional<ContactManifold>(*)(const std::shared_ptr<RigidBody>&, const std::shared_ptr<RigidBody>&);

class ManifoldCollision {
private:
    static std::array<std::array<ManifoldFunc, ShapeTypeCount>, ShapeTypeCount> m_dispatchTable;

public:
    // Generate contact manifold for sphere-sphere collision
    static std::optional<ContactManifold> SphereSphere(
        const std::shared_ptr<RigidBody>& sphereA,
        const std::shared_ptr<RigidBody>& sphereB)
    {
        Sphere* shapeA = static_cast<Sphere*>(sphereA->m_shape.get());
        Sphere* shapeB = static_cast<Sphere*>(sphereB->m_shape.get());

        fungt::Vec3 direction = sphereB->m_pos - sphereA->m_pos;
        float distance = direction.length();
        float combinedRadius = shapeA->m_radius + shapeB->m_radius;

        // SAFETY: Check for invalid distance
        if (!std::isfinite(distance) || distance < 0.0f) return std::nullopt;

        if (distance < combinedRadius && distance > 0.0001f) {
            fungt::Vec3 normal = direction / distance;

            // SAFETY: Check for NaN in normal
            if (!std::isfinite(normal.x) || !std::isfinite(normal.y) || !std::isfinite(normal.z)) {
                return std::nullopt;
            }

            float penetration = combinedRadius - distance;

            // Contact point at surface of sphere A
            fungt::Vec3 worldPointA = sphereA->m_pos + normal * shapeA->m_radius;
            fungt::Vec3 worldPointB = sphereB->m_pos - normal * shapeB->m_radius;

            // Local points (relative to body centers)
            fungt::Vec3 localPointA = normal * shapeA->m_radius;
            fungt::Vec3 localPointB = normal * (-shapeB->m_radius);

            ContactPoint cp(localPointA, localPointB, worldPointA, worldPointB, normal, penetration);

            ContactManifold manifold(sphereA, sphereB);
            manifold.addContactPoint(cp);
            return manifold;
        }

        return std::nullopt;
    }

    // Generate contact manifold for sphere-box collision
    static std::optional<ContactManifold> SphereBox(
        const std::shared_ptr<RigidBody>& sphere,
        const std::shared_ptr<RigidBody>& box)
    {
        Sphere* sphere_shape = static_cast<Sphere*>(sphere->m_shape.get());
        Box* box_shape = static_cast<Box*>(box->m_shape.get());

        // Find closest point on box to sphere center
        fungt::Vec3 relativePos = sphere->m_pos - box->m_pos;
        fungt::Vec3 halfSize = box_shape->size * 0.5f;

        // Clamp to box bounds (assumes axis-aligned for now)
        fungt::Vec3 closestPoint;
        closestPoint.x = std::max(-halfSize.x, std::min(halfSize.x, relativePos.x));
        closestPoint.y = std::max(-halfSize.y, std::min(halfSize.y, relativePos.y));
        closestPoint.z = std::max(-halfSize.z, std::min(halfSize.z, relativePos.z));

        // Convert back to world space
        fungt::Vec3 worldClosestPoint = box->m_pos + closestPoint;
        fungt::Vec3 direction = sphere->m_pos - worldClosestPoint;
        float distance = direction.length();

        // FIXED: Check if sphere center is TRULY inside box
        bool sphereCenterInsideBox = (std::abs(relativePos.x) <= halfSize.x) &&
            (std::abs(relativePos.y) <= halfSize.y) &&
            (std::abs(relativePos.z) <= halfSize.z);

        // Check for collision: center inside OR surface penetrating
        if (sphereCenterInsideBox || distance < sphere_shape->m_radius) {
            // // DEBUG OUTPUT
            // static int debugCount = 0;
            // if (debugCount++ < 3) {  // Only print first 3 collisions
            //     std::cout << "=== SPHERE-BOX COLLISION ===" << std::endl;
            //     std::cout << "Sphere pos: (" << sphere->m_pos.x << ", " << sphere->m_pos.y << ", " << sphere->m_pos.z << ")" << std::endl;
            //     std::cout << "Sphere radius: " << sphere_shape->m_radius << std::endl;
            //     std::cout << "Box pos: (" << box->m_pos.x << ", " << box->m_pos.y << ", " << box->m_pos.z << ")" << std::endl;
            //     std::cout << "Box halfSize: (" << halfSize.x << ", " << halfSize.y << ", " << halfSize.z << ")" << std::endl;
            //     std::cout << "relativePos: (" << relativePos.x << ", " << relativePos.y << ", " << relativePos.z << ")" << std::endl;
            //     std::cout << "closestPoint: (" << closestPoint.x << ", " << closestPoint.y << ", " << closestPoint.z << ")" << std::endl;
            //     std::cout << "distance: " << distance << std::endl;
            //     std::cout << "sphereCenterInsideBox (NEW): " << sphereCenterInsideBox << std::endl;
            // }

            fungt::Vec3 normal;
            float penetration;

            if (sphereCenterInsideBox) {
                // Sphere center is INSIDE the box - find nearest face
                fungt::Vec3 diff = relativePos;
                float dx = halfSize.x - std::abs(diff.x);
                float dy = halfSize.y - std::abs(diff.y);
                float dz = halfSize.z - std::abs(diff.z);

                // // DEBUG OUTPUT
                // if (debugCount <= 3) {
                //     std::cout << "INSIDE BOX BRANCH" << std::endl;
                //     std::cout << "diff: (" << diff.x << ", " << diff.y << ", " << diff.z << ")" << std::endl;
                //     std::cout << "dx=" << dx << " dy=" << dy << " dz=" << dz << std::endl;
                // }

                // Normal points FROM box TO sphere (separation direction)
                if (dx < dy && dx < dz) {
                    normal = fungt::Vec3((diff.x > 0) ? 1 : -1, 0, 0);
                    penetration = sphere_shape->m_radius - dx;
                    // if (debugCount <= 3) std::cout << "Chose X axis, normal=(" << normal.x << "," << normal.y << "," << normal.z << ") pen=" << penetration << std::endl;
                }
                else if (dy < dz) {
                    // For ground collision (box is static), ALWAYS push sphere UP
                    if (box->isStatic()) {
                        normal = fungt::Vec3(0, 1, 0);  // Always push UP for ground
                        penetration = sphere_shape->m_radius - dy;
                    }
                    else {
                        // For non-static boxes, use nearest face logic
                        float distToTop = halfSize.y - diff.y;
                        float distToBottom = halfSize.y + diff.y;

                        if (distToTop < distToBottom) {
                            normal = fungt::Vec3(0, 1, 0);
                            penetration = sphere_shape->m_radius - dy;
                        }
                        else {
                            normal = fungt::Vec3(0, -1, 0);
                            penetration = sphere_shape->m_radius - dy;
                        }
                    }
                    // if (debugCount <= 3) std::cout << "Chose Y axis, normal=(" << normal.x << "," << normal.y << "," << normal.z << ") pen=" << penetration << std::endl;
                }
                else {
                    normal = fungt::Vec3(0, 0, (diff.z > 0) ? 1 : -1);
                    penetration = sphere_shape->m_radius - dz;
                    // if (debugCount <= 3) std::cout << "Chose Z axis, normal=(" << normal.x << "," << normal.y << "," << normal.z << ") pen=" << penetration << std::endl;
                }
            }
            else {
                // Sphere surface touching/penetrating box from outside
                if (distance < 0.0001f) {
                    normal = fungt::Vec3(0, 1, 0);
                    penetration = sphere_shape->m_radius;
                }
                else {
                    normal = direction / distance;
                    penetration = sphere_shape->m_radius - distance;

                    // HACK FIX: If this is a collision with ground (static box) and normal points down, flip it
                    if (box->isStatic() && normal.y < 0) {
                        normal.y = -normal.y;  // Force normal to point UP
                    }
                }
            }

            fungt::Vec3 worldPointA = sphere->m_pos - normal * (sphere_shape->m_radius - penetration);
            fungt::Vec3 worldPointB = worldClosestPoint;

            // Local points
            fungt::Vec3 localPointA = normal * (-(sphere_shape->m_radius - penetration));
            fungt::Vec3 localPointB = worldPointB - box->m_pos;

            ContactPoint cp(localPointA, localPointB, worldPointA, worldPointB, normal, penetration);

            ContactManifold manifold(sphere, box);
            manifold.addContactPoint(cp);
            // std::cout << "SphereBox RETURNING: normal=("
            //     << cp.normal.x << "," << cp.normal.y << "," << cp.normal.z
            //     << ") pen=" << cp.penetrationDepth << std::endl;

            return manifold;
        }

        return std::nullopt;
    }
    // Box-Sphere collision
    static std::optional<ContactManifold> BoxSphere(
        const std::shared_ptr<RigidBody>& box,
        const std::shared_ptr<RigidBody>& sphere)
    {
        // SphereBox creates manifold with (sphere, box)
        // We need (box, sphere), so create a new manifold with swapped bodies
        auto sphereBoxManifold = SphereBox(sphere, box);
        if (sphereBoxManifold) {
            ContactManifold newManifold(box, sphere);  // Bodies in correct order
            for (int i = 0; i < sphereBoxManifold->getNumPoints(); ++i) {
                ContactPoint& pt = sphereBoxManifold->getPoint(i);
                // std::cout << "BoxSphere: BEFORE swap, normal=("
                //     << pt.normal.x << "," << pt.normal.y << "," << pt.normal.z << ")" << std::endl;

                ContactPoint swappedPt(
                    pt.localPointB, pt.localPointA,
                    pt.worldPointB, pt.worldPointA,
                    pt.normal,  // Don't flip!
                    pt.penetrationDepth
                );

                // std::cout << "BoxSphere: AFTER swap, normal=("
                //     << swappedPt.normal.x << "," << swappedPt.normal.y << "," << swappedPt.normal.z << ")" << std::endl;
                newManifold.addContactPoint(swappedPt);
            }
            return newManifold;
        }
        return std::nullopt;
    }

    // Generate contact manifold for box-box collision (AABB only for now)
    // TODO: This needs SAT for rotated boxes
    static std::optional<ContactManifold> BoxBox(
        const std::shared_ptr<RigidBody>& boxA,
        const std::shared_ptr<RigidBody>& boxB)
    {
        Box* shapeA = static_cast<Box*>(boxA->m_shape.get());
        Box* shapeB = static_cast<Box*>(boxB->m_shape.get());

        fungt::Vec3 halfSizeA = shapeA->size * 0.5f;
        fungt::Vec3 halfSizeB = shapeB->size * 0.5f;

        fungt::Vec3 distance = boxB->m_pos - boxA->m_pos;
        fungt::Vec3 absDistance = fungt::Vec3(std::abs(distance.x), std::abs(distance.y), std::abs(distance.z));
        fungt::Vec3 combinedHalfSizes = halfSizeA + halfSizeB;

        // Check overlap on all axes
        if (absDistance.x < combinedHalfSizes.x &&
            absDistance.y < combinedHalfSizes.y &&
            absDistance.z < combinedHalfSizes.z) {

            // Find axis of minimum penetration
            fungt::Vec3 penetrations = combinedHalfSizes - absDistance;

            fungt::Vec3 normal;
            float minPenetration;
            int axis;  // 0=x, 1=y, 2=z

            if (penetrations.x <= penetrations.y && penetrations.x <= penetrations.z) {
                minPenetration = penetrations.x;
                normal = fungt::Vec3(distance.x > 0 ? 1.0f : -1.0f, 0, 0);
                axis = 0;
            } else if (penetrations.y <= penetrations.z) {
                minPenetration = penetrations.y;
                normal = fungt::Vec3(0, distance.y > 0 ? 1.0f : -1.0f, 0);
                axis = 1;
            } else {
                minPenetration = penetrations.z;
                normal = fungt::Vec3(0, 0, distance.z > 0 ? 1.0f : -1.0f);
                axis = 2;
            }

            // Generate up to 4 contact points at the corners of the contact face
            ContactManifold manifold(boxA, boxB);

            // For axis-aligned boxes, we can generate multiple contact points
            // on the separating axis by finding the overlapping rectangle
            if (axis == 1) {  // Y-axis collision (most common: floor/stacking)
                // Find overlapping rectangle in XZ plane
                float minX = std::max(boxA->m_pos.x - halfSizeA.x, boxB->m_pos.x - halfSizeB.x);
                float maxX = std::min(boxA->m_pos.x + halfSizeA.x, boxB->m_pos.x + halfSizeB.x);
                float minZ = std::max(boxA->m_pos.z - halfSizeA.z, boxB->m_pos.z - halfSizeB.z);
                float maxZ = std::min(boxA->m_pos.z + halfSizeA.z, boxB->m_pos.z + halfSizeB.z);

                // Create 4 contact points at corners of overlapping rectangle
                float contactY = (normal.y > 0) ?
                    (boxA->m_pos.y + halfSizeA.y) :
                    (boxA->m_pos.y - halfSizeA.y);

                addBoxContactPoint(manifold, boxA, boxB, minX, contactY, minZ, normal, minPenetration);
                addBoxContactPoint(manifold, boxA, boxB, maxX, contactY, minZ, normal, minPenetration);
                addBoxContactPoint(manifold, boxA, boxB, minX, contactY, maxZ, normal, minPenetration);
                addBoxContactPoint(manifold, boxA, boxB, maxX, contactY, maxZ, normal, minPenetration);
            } else {
                // For X or Z axis, generate just 1 contact point for now
                // (Full implementation would clip polygon faces)
                fungt::Vec3 contactPoint = boxA->m_pos + distance * 0.5f;
                fungt::Vec3 localA = contactPoint - boxA->m_pos;
                fungt::Vec3 localB = contactPoint - boxB->m_pos;

                ContactPoint cp(localA, localB, contactPoint, contactPoint, normal, minPenetration);
                manifold.addContactPoint(cp);
            }

            return manifold;
        }

        return std::nullopt;
    }

private:
    // Helper to add a box-box contact point
    static void addBoxContactPoint(ContactManifold& manifold,
                                   const std::shared_ptr<RigidBody>& boxA,
                                   const std::shared_ptr<RigidBody>& boxB,
                                   float x, float y, float z,
                                   const fungt::Vec3& normal,
                                   float penetration)
    {
        fungt::Vec3 worldPoint(x, y, z);
        fungt::Vec3 localA = worldPoint - boxA->m_pos;
        fungt::Vec3 localB = worldPoint - boxB->m_pos;

        ContactPoint cp(localA, localB, worldPoint, worldPoint, normal, penetration);
        manifold.addContactPoint(cp);
    }

public:
    static std::optional<ContactManifold> Detect(const std::shared_ptr<RigidBody>& bodyA,
                                                  const std::shared_ptr<RigidBody>& bodyB)
    {
        int typeA = static_cast<int>(bodyA->m_shape->GetType());
        int typeB = static_cast<int>(bodyB->m_shape->GetType());

        ManifoldFunc func = m_dispatchTable[typeA][typeB];
        if (func != nullptr) {
            return func(bodyA, bodyB);
        }

        return std::nullopt;
    }

    static void Init()
    {
        for (auto& row : m_dispatchTable) {
            for (auto& func : row) {
                func = nullptr;
            }
        }

        m_dispatchTable[static_cast<int>(ShapeType::SPHERE)][static_cast<int>(ShapeType::BOX)] = SphereBox;
        m_dispatchTable[static_cast<int>(ShapeType::BOX)][static_cast<int>(ShapeType::SPHERE)] = BoxSphere;
        m_dispatchTable[static_cast<int>(ShapeType::SPHERE)][static_cast<int>(ShapeType::SPHERE)] = SphereSphere;
        m_dispatchTable[static_cast<int>(ShapeType::BOX)][static_cast<int>(ShapeType::BOX)] = BoxBox;
    }
};

#endif // _MANIFOLD_COLLISION_H_