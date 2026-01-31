#if !defined(_PLANE_H_)
#define _PLANE_H_

#include "primitives.hpp"

class Plane : public Primitive{

public:
        Plane();
        ~Plane();
        void draw() override;
        void setData() override;
};




#endif // _PLANE_H_
