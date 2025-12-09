#if !defined(_FUNGT_H_)
#define _FUNGT_H_
#include "GT/graphicsTool.hpp"
#include "SceneManager/scene_manager.hpp"
#include "CubeMap/cube_map.hpp"
#include "Geometries/inf_grid.hpp"
#include "ParticleSimulation/particle_simulation.hpp"
#include "Path_Manager/path_manager.hpp"
#include "Physics/Clothing/clothing.hpp"
#include "Physics/Clothing/clothing.hpp"
#include "ViewPort/viewport.hpp"              
#include "Layer/layer_stack.hpp"              
#include "GUI/imgui_layer.hpp"                
#include "GUI/render_info_window.hpp"   
#include "GUI/scene_hierarchy_window.hpp"
#include "GUI/properties_window.hpp"
#include "GUI/material_editor_window.hpp"      
#include "GUI/lights_editor_window.hpp"
#include <memory>
#include <unordered_map>

class FunGT : public GraphicsTool{

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
    // IMGUI LAYER SYSTEM
    std::unique_ptr<ViewPort> m_ViewPortLayer;     
    std::unique_ptr<ImGuiLayer> m_imguiLayer;      
    LayerStack m_layerStack;                       

    std::shared_ptr<InfiniteGrid> m_grid;  // ← ADD (shared_ptr for SceneManager)

    public: 
        FunGT(int _width, int _height); 
        ~FunGT();

        void update(const std::function<void()> &renderLambda) override;
        void renderGUI() override;
        void processKeyBoardInput();
        void processMouseInput(double xpos, double ypos);
        void setBackgroundColor(float red, float green, float blue, float alfa);
        void setBackgroundColor(float color = 0.f);
        Camera& getCamera(); 
        std::shared_ptr<SceneManager> getSceneManager();
        void set(const std::function<void()>& renderLambda);
        static std::unique_ptr<FunGT> createScene(int _width, int _height);

    protected:
        // Override virtual methods from GraphicsTool
        void onMouseMove(double xpos, double ypos) override;
        void onUpdate(float deltaTime) override;
        void onMouseScroll(double xoffset, double yoffset) override;

};
typedef std::shared_ptr<CubeMap> FunGTCubeMap; //cubemap shared pointer
typedef std::shared_ptr<Animation> FunGTAnimation;
typedef std::unique_ptr<FunGT> FunGTScene;
typedef std::shared_ptr<SceneManager> FunGTSceneManager; //returns a shared pointer
typedef std::shared_ptr<SimpleModel> FunGTSModel;


#endif // _FUNGT_H_
