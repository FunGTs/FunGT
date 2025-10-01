#if !defined(_SHAPE_H_)
#define _SHAPE_H_
#include "../../Matrix/matrix3x3f.hpp"
enum class ShapeType {
    BOX,
    SPHERE,
    COUNT
};
class Shape {

    public:
        ShapeType m_shapeType;

        Shape(ShapeType _shapeType): m_shapeType{_shapeType}{

        }
        virtual ~Shape(){

        }
        virtual fungt::Matrix3f getInertiaMatrix(float mass) const = 0;
        virtual float getVolume() const = 0 ;
        virtual ShapeType GetType() const { return m_shapeType; }
};


#endif // _SHAPE_H_
