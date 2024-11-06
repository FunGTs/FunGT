#if !defined(_RENDERABLE_H_)
#define _RENDERABLE_H_
#include "../include/prerequisites.hpp" 
#include "../include/glmath.hpp"
#include "../Shaders/shader.hpp"

class Renderable{ //Abstract class

    public: 


        //Pure virtual functions
        virtual void draw() = 0;
        virtual Shader& getShader() = 0; 
        virtual glm::mat4 getViewMatrix(){
            return glm::mat4(0.0);
        };
        virtual void setViewMatrix(const glm::mat4 &viewMatrix){

        };
        //Virtual functions
        virtual glm::mat4 getProjectionMatrix(){
            return glm::mat4(0.0);
        }
        virtual void updateTime(float deltaTime){
            
        }
        virtual void enableDepthFunc(){

        }
        virtual void disableDepthFunc(){

        }
        virtual ~Renderable() = default;

}; 

#endif // _RENDERABLE_H_
