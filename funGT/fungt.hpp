#if !defined(_FUNGT_H_)
#define _FUNGT_H_
#include "../GT/graphicsTool.hpp"
#include <memory> 

class FunGT : public GraphicsTool<FunGT>{

    Camera m_camera;
    Shader m_shader; 

    //Projection matrix 
    float fov = 45.f; 
    float nearPlane = 0.1f; 
    float farPlane = 100.f; 

    glm::vec3 position = glm::vec3(0.f);
    glm::vec3 rotation = glm::vec3(0.f);
    glm::vec3 scale =  glm::vec3(1.0);

    //Matrices
     glm::mat4 ProjectionMatrix = glm::mat4(1.f);
     glm::mat4 ModelMatrix = glm::mat4(1.f);

     //Time and frames
    float deltaTime = 0.0f; 
    float lastFrame = 0.0f;

    //mouse frames

     float m_lastXmouse; 
     float m_lastYmouse;
     bool  m_firstMouse;
    

    Model m_model;
    std::unique_ptr<Primitive> cube; 
    std::unique_ptr<Primitive> plane;
    


  
        

    public: 
        FunGT(int _width, int _height); 
        ~FunGT();

        void update(); 
        void set(); 
        void processKeyBoardInput();
        void processMouseInput(double xpos, double ypos);
        static void mouse_callback(GLFWwindow *window, double xpos, double ypos); 
        void setBackground(float red, float green, float blue, float alfa);


     



};



#endif // _FUNGT_H_
