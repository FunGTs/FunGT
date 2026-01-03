#if !defined(_PYRAMID_H_)
#define _PYRAMID_H_

#include "primitives.hpp"
//For texturized pyramids

class Pyramid : public Primitive{

public:
    Pyramid();
    Pyramid(glm::vec3 pos);
    Pyramid(float x, float y, float z);
    ~Pyramid();


    void draw() override;
    void create(const std::string &path) override;
    void setData() override;



};

#endif // _PYRAMID_H_
