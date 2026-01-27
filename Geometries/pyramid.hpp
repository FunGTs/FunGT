#if !defined(_PYRAMID_H_)
#define _PYRAMID_H_

#include "primitives.hpp"
//For texturized pyramids

class Pyramid : public Primitive{

public:
    Pyramid();
    ~Pyramid();
    void draw() override;
    void setData() override;
};

#endif // _PYRAMID_H_
