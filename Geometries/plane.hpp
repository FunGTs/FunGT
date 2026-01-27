#if !defined(_PLANE_H_)
#define _PLANE_H_

#include "primitives.hpp"

class Plane : public Primitive {
private:
        float m_width;
        float m_depth;

public:
        Plane(float width = 2.0f, float depth = 2.0f);
        ~Plane();

        void draw() override;
        void setData() override;
};

#endif // _PLANE_H_