#if !defined(_FUNGT_H_)
#define _FUNGT_H_
#include "../GT/graphicsTool.hpp"
#include "../SceneManager/scene_manager.hpp"
#include "../CubeMap/cube_map.hpp"
#include <memory> 
#include <unordered_map>



class FunGT : public GraphicsTool<FunGT>{

    Camera m_camera;
    //Shader m_shader; 

    //Projection matrix 
    float fov = 45.f; 
    float nearPlane = 0.1f; 
    float farPlane = 500.f; 

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
    

    std::unique_ptr<Model> m_model; //Static model
    std::unique_ptr<Primitive> cube; 
    std::unique_ptr<Primitive> plane;
   
    
    //Creates an animation:
    
    std::shared_ptr<SceneManager> m_sceneManager;
    


  
        

    public: 
        FunGT(int _width, int _height); 
        ~FunGT();

        virtual void update(); 
        virtual void update(const std::function<void()> &renderLambda);
        void set(); 
        void processKeyBoardInput();
        void processMouseInput(double xpos, double ypos);
        static void mouse_callback(GLFWwindow *window, double xpos, double ypos); 
        void setBackgroundColor(float red, float green, float blue, float alfa);
        void setBackgroundColor(float color = 0.f);
        void addShader();
        Camera getCamera(); 
      
        std::shared_ptr<SceneManager> getSceneManager();
        void set(const std::function<void()>& renderLambda);
        static std::unique_ptr<FunGT> createScene(int _width, int _height); 
};

typedef std::shared_ptr<CubeMap> FunGTCubeMap; //cubemap shared pointer
typedef std::shared_ptr<Animation> FunGTAnimation;
typedef std::unique_ptr<FunGT> FunGTScene;
typedef std::shared_ptr<SceneManager> FunGTSceneManager; //returns a shared pointer 

void validate(std::shared_ptr<SceneManager>){

}

#endif // _FUNGT_H_
