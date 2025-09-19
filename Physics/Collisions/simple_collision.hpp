#if !defined(_SIMPLE_COLLISION_H_)
#define _SIMPLE_COLLISION_H_
#include <functional>
#include <optional>
#include "../Shapes/sphere.hpp"
#include "../Shapes/box.hpp"
#include "../Contact/contact.hpp"


//By writing (*),turns into a function pointer type, which is valid to store in an array.
using ContactFunc = std::optional<Contact>(*)(const std::shared_ptr<RigidBody>& sphere, const std::shared_ptr<RigidBody>& box);
//using ContactFunc = std::function<std::optional<Colission>>(const RigidBody& A, const RigidBody& B);
constexpr std::size_t ShapeTypeCount = static_cast<std::size_t>(ShapeType::COUNT);


class SimpleCollision{
      //Std::array does not accept an array of std::functions
    static std::array<std::array<ContactFunc, ShapeTypeCount>, ShapeTypeCount> m_dispatchTable;
    
    static std::optional<Contact> SphereBox(const std::shared_ptr<RigidBody>& sphere, const std::shared_ptr<RigidBody>& box);
    static std::optional<Contact> BoxSphere(const std::shared_ptr<RigidBody>& box, const std::shared_ptr<RigidBody>& sphere);
    static std::optional<Contact> SphereSphere(const std::shared_ptr<RigidBody>& sphereA, const std::shared_ptr<RigidBody>& sphereB);
    static std::optional<Contact> BoxBox(const std::shared_ptr<RigidBody>& boxA, const std::shared_ptr<RigidBody>& boxB);

public:
  

    static std::optional<Contact> Detect(const std::shared_ptr<RigidBody>& bodyA,const std::shared_ptr<RigidBody>& bodyB); 
    static void Init();
    

};





#endif // _SIMPLE_COLLISION_H_
