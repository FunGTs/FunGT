#if !defined(_PLANE_H_)
#define _PLANE_H_

#include "primitives.hpp"

class Plane : public Primitive{

public:
        Plane();
        ~Plane();
        void draw() override;
        void setData() override;
        glm::mat4 getModelMatrix() const override; 
        void setPosition(glm::vec3 pos) override; 
        void setModelMatrix() override;
        void updateModelMatrix(float zrot) override;



};




#endif // _PLANE_H_
