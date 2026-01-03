#if !defined(_CUBE_H_)
#define _CUBE_H_
#include "primitives.hpp"
class Cube : public Primitive {
    public:


        Cube();
        Cube(glm::vec3 cubePos);
        Cube(float xpos, float ypos, float zpos);
        ~Cube();

        void create(const std::string &pathToTexture) override;
        void draw() override;
        void setData() override;


};

#endif // _CUBE_H_
