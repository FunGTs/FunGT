#if !defined(_INTEGRATOR_H_)
#define _INTEGRATOR_H_
#include <string>
#include "../RigidBody/rigid_body.hpp"
class Integrator {
public:
    virtual ~Integrator() = default;
    virtual void integrate(std::shared_ptr<RigidBody> body, float dt) = 0;
    virtual std::string getName() const = 0;
};

#endif // _INTEGRATOR_H_
