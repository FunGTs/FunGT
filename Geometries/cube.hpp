#if !defined(_CUBE_H_)
#define _CUBE_H_
#include "primitives.hpp"

class Cube : public Primitive {
public:
    Cube();
    ~Cube();

    void draw() override;
    void setData() override;
    void InstancedDraw(Shader& shader, int instanceCount) override;
};

#endif // _CUBE_H_