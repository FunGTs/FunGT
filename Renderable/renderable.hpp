#if !defined(_RENDERABLE_H_)
#define _RENDERABLE_H_
#include "../include/prerequisites.hpp" 
#include "../include/glmath.hpp"
#include "Shaders/shader.hpp"
#include "Physics/RigidBody/rigid_body.hpp"

class Renderable{ //Abstract class

    protected:
        bool m_isEditMode = false;  // NEW: Edit mode flag
        bool m_isEditable  = false; // Can this renderable be edited?

    public: 


        //Pure virtual functions
        virtual void draw() = 0;
        virtual Shader& getShader() = 0;
        virtual glm::mat4 getModelMatrix() const {
            return glm::mat4(0.0);
        }
        virtual void updateModelMatrix(){
            //Default implementation does nothing
        };
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
        // Physics support - returns empty weak_ptr by default
        virtual std::weak_ptr<RigidBody> getRigidBody() const {
            return std::weak_ptr<RigidBody>();
        }

        virtual void setEditMode(bool enabled) { m_isEditMode = enabled; }
        virtual bool isEditMode() const { return m_isEditMode; }
        virtual bool isEditable() const { return m_isEditable; }   

        virtual ~Renderable() = default;

}; 

#endif // _RENDERABLE_H_
