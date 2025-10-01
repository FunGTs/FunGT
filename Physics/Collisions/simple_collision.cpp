#include "simple_collision.hpp"

std::optional<Contact> SimpleCollision::SphereBox(const std::shared_ptr<RigidBody>& sphere, 
     const std::shared_ptr<RigidBody>& box)
{

    //This is the function to retunr a Contact 
    
    Sphere *sphere_shape = static_cast<Sphere *>(sphere->m_shape.get());
    Box    *box_shape    = static_cast<Box *>(box->m_shape.get());
    
    // Find closest point on box to sphere center
    fungt::Vec3 relativePos = sphere->m_pos - box->m_pos;

    // Clamp to box bounds
    fungt::Vec3 halfSize = box_shape->size * 0.5f;
    fungt::Vec3 closestPoint;
    closestPoint.x = std::max(-halfSize.x, std::min(halfSize.x, relativePos.x));
    closestPoint.y = std::max(-halfSize.y, std::min(halfSize.y, relativePos.y));
    closestPoint.z = std::max(-halfSize.z, std::min(halfSize.z, relativePos.z));
     // Convert back to world space
    fungt::Vec3 worldClosestPoint = box->m_pos + closestPoint;

    fungt::Vec3 direction = sphere->m_pos - worldClosestPoint;
    float distance = direction.length();
    
   if (distance < sphere_shape->m_radius) {
        fungt::Vec3 normal;
        if (distance > 1e-6f) {
            normal = direction.normalize();
        } else {
            // Sphere is exactly at closestPoint (center inside box)
            // Pick axis of deepest penetration
            fungt::Vec3 diff = relativePos;
            float dx = halfSize.x - std::abs(diff.x);
            float dy = halfSize.y - std::abs(diff.y);
            float dz = halfSize.z - std::abs(diff.z);

            if (dx < dy && dx < dz)
                normal = fungt::Vec3((diff.x > 0) ? 1 : -1, 0, 0);
            else if (dy < dz)
                normal = fungt::Vec3(0, (diff.y > 0) ? 1 : -1, 0);
            else
                normal = fungt::Vec3(0, 0, (diff.z > 0) ? 1 : -1);
        }

        float penetration = sphere_shape->m_radius - distance;
        fungt::Vec3 contactPoint = worldClosestPoint;

        return Contact(sphere, box, contactPoint, normal, penetration);
    }
    return std::nullopt;
}
std::optional<Contact> SimpleCollision::BoxSphere(const std::shared_ptr<RigidBody> &box, const std::shared_ptr<RigidBody> &sphere)
{
     // Just swap parameters and flip normal
    auto Contact = SphereBox(sphere, box);
    if (Contact) {
        // Flip the normal direction since we swapped the order
        Contact->colissionNormal = Contact->colissionNormal * -1.0f;
        // Swap the bodies
        std::swap(Contact->bodyA, Contact->bodyB);
    }
    return Contact;
}
std::optional<Contact> SimpleCollision::SphereSphere(const std::shared_ptr<RigidBody> &sphereA, const std::shared_ptr<RigidBody> &sphereB)
{
    Sphere* shapeA = static_cast<Sphere*>(sphereA->m_shape.get());
    Sphere* shapeB = static_cast<Sphere*>(sphereB->m_shape.get());
    
    fungt::Vec3 direction = sphereB->m_pos - sphereA->m_pos;
    float distance = direction.length();
    float combinedRadius = shapeA->m_radius + shapeB->m_radius;
    
    if (distance < combinedRadius && distance > 0) {
        fungt::Vec3 normal = direction.normalize();
        float penetration = combinedRadius - distance;
        fungt::Vec3 contactPoint = sphereA->m_pos + normal * (shapeA->m_radius - penetration * 0.5f);
        
        Contact contact(sphereA, sphereB, contactPoint, normal, penetration);
        return contact; // automatically wrapped into std::optional
    }
    
    return std::nullopt;
}
std::optional<Contact> SimpleCollision::BoxBox(const std::shared_ptr<RigidBody> &boxA, const std::shared_ptr<RigidBody> &boxB)
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
        
        if (penetrations.x <= penetrations.y && penetrations.x <= penetrations.z) {
            minPenetration = penetrations.x;
            normal = fungt::Vec3(distance.x > 0 ? 1.0f : -1.0f, 0, 0);
        } else if (penetrations.y <= penetrations.z) {
            minPenetration = penetrations.y;
            normal = fungt::Vec3(0, distance.y > 0 ? 1.0f : -1.0f, 0);
        } else {
            minPenetration = penetrations.z;
            normal = fungt::Vec3(0, 0, distance.z > 0 ? 1.0f : -1.0f);
        }
        
        fungt::Vec3 contactPoint = boxA->m_pos + distance * 0.5f;
        Contact contact(boxA, boxB, contactPoint, normal, minPenetration);
        return contact;
    }
    return std::nullopt;
}
std::optional<Contact> SimpleCollision::Detect(const std::shared_ptr<RigidBody> &bodyA, const std::shared_ptr<RigidBody> &bodyB)
{
    int typeA = static_cast<int>(bodyA->m_shape->GetType());
    int typeB = static_cast<int>(bodyB->m_shape->GetType());
    
    ContactFunc func = m_dispatchTable[typeA][typeB];
    if (func != nullptr) {
        return func(bodyA, bodyB);
    }
    
    return std::nullopt; // No Contact function available
}
void SimpleCollision::Init()
{
    for (auto& row : m_dispatchTable) {
        for (auto& func : row) {
            func = nullptr;
        }
    }
     // Set up the Contact function mappings
    m_dispatchTable[static_cast<int>(ShapeType::SPHERE)][static_cast<int>(ShapeType::BOX)]  = SphereBox;
    m_dispatchTable[static_cast<int>(ShapeType::BOX)][static_cast<int>(ShapeType::SPHERE)]  = BoxSphere;  // Need this function
    m_dispatchTable[static_cast<int>(ShapeType::SPHERE)][static_cast<int>(ShapeType::SPHERE)] = SphereSphere;
    m_dispatchTable[static_cast<int>(ShapeType::BOX)][static_cast<int>(ShapeType::BOX)] = BoxBox;     

}


// Definition of static member
std::array<std::array<ContactFunc, ShapeTypeCount>, ShapeTypeCount> SimpleCollision::m_dispatchTable = {};