#if !defined(_PLANE_H_)
#define _PLANE_H_

#include "primitives.hpp"

class Plane : public Primitive{

public:
        Plane();
        Plane(glm::vec3 cubePos);
        Plane(float xpos, float ypos, float zpos);
        ~Plane();
        void create(const std::string &pathToTexture) override;
        void draw() override;
        void setData() override;



};




#endif // _PLANE_H_
