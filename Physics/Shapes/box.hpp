#if !defined(_BOX_H_)
#define _BOX_H_
#include "shape.hpp"
// Box shape
class Box : public Shape {
public:
    fungl::Vec3 size; // width, height, depth
    
    Box(float width, float height, float depth) 
        : Shape(ShapeType::BOX), size(width, height, depth) {}
    
     fungl::Matrix3f getInertiaMatrix(float mass) const override {
        fungl::Matrix3f inertia;
        float w2 = size.x * size.x;
        float h2 = size.y * size.y;
        float d2 = size.z * size.z;
        
        inertia.m[0][0] = mass * (h2 + d2) / 12.0f;
        inertia.m[1][1] = mass * (w2 + d2) / 12.0f;
        inertia.m[2][2] = mass * (w2 + h2) / 12.0f;
        
        return inertia;
    }
    
    float getVolume() const override {
        return size.x * size.y * size.z;
    }
};


#endif // _BOX_H_
