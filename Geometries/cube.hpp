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
        void draw() override ;
        void setData() override;
        glm::mat4 getModelMatrix() const override; 
        void setPosition(glm::vec3 pos) override; 
        void setModelMatrix() override;
        void updateModelMatrix(float zrot) override;
        void setScale(glm::vec3 scale) override;    
      

};

#endif // _CUBE_H_
