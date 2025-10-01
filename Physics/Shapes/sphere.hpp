#if !defined(_SHPERE_H_)
#define _SHPERE_H_
#include "shape.hpp"
class Sphere : public Shape {

public:
    float m_radius;

    Sphere(float _radius)
    : Shape(ShapeType::SPHERE), m_radius{_radius}{
    }

    fungt::Matrix3f getInertiaMatrix(float mass) const override {
        fungt::Matrix3f inertia;
        float I = 0.4f * mass * m_radius * m_radius; // (2/5) * m * r²
        
        inertia.m[0][0] = I;
        inertia.m[1][1] = I;
        inertia.m[2][2] = I;
        
        return inertia;
    }
    float getVolume() const override{

        float volume = (3.0f/4.0f) * M_PI * (m_radius*m_radius*m_radius);
        return volume;
    }



};

#endif // _SHPERE_H_
