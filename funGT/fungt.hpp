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
#include "GUI/fungt_gui_headers.hpp"
#include "SimpleGeometry/simple_geometry.hpp"
#include "Physics/PhysicsWorld/physics_world.hpp"
#include <memory>
#include <unordered_map>

class FunGT : public GraphicsTool{

    Camera m_camera;
    //Shader m_shader; 

    //Projection matrix 
   
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
    
    std::shared_ptr<SceneManager> m_sceneManager;
    // IMGUI LAYER SYSTEM
    std::unique_ptr<ViewPort> m_ViewPortLayer;     
    std::unique_ptr<ImGuiLayer> m_imguiLayer;
    std::shared_ptr<fungt::SimulationController> m_simController;
    std::unique_ptr<PhysicsDebugRenderer> m_physicsDebugRenderer;   
    LayerStack m_layerStack;

    std::shared_ptr<InfiniteGrid> m_grid;  // ← ADD (shared_ptr for SceneManager)

    //Collisions
    spCollisionManager m_collisionManager;
    std::shared_ptr<PhysicsWorld> m_physicsWorld;


    //Editing meshes:

    bool m_editMode = false;
    Primitive* m_activePrimitive = nullptr;  // Which mesh are we editing

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
        std::shared_ptr<fungt::SimulationController> getSimulationController() { return m_simController; }
        std::shared_ptr<CollisionManager> getCollisionManager() {
            if(!m_collisionManager) {
                std::cerr << "Warning: CollisionManager not initialized!" << std::endl;
                return nullptr;
            }
            return m_collisionManager;
        }
        void createPhysicsWorld() {
            m_physicsWorld = std::make_shared<PhysicsWorld>();
            if(m_physicsWorld) {
                m_collisionManager = m_physicsWorld->getCollisionManager();
            }
        }
        std::shared_ptr<PhysicsWorld> getPhysicsWorld() {
            if(!m_physicsWorld) {
                std::cerr << "Warning: PhysicsWorld not initialized!" << std::endl;
                return nullptr;
            }
            return m_physicsWorld;
        }
        //Mesh editing functions
        bool isEditMode() const { return m_editMode; }
        void setEditMode(bool enabled) { m_editMode = enabled; }
        void setActivePrimitive(Primitive* prim) { m_activePrimitive = prim; }
        Primitive* getActivePrimitive() { return m_activePrimitive; }

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
typedef std::shared_ptr<SimpleGeometry> FunGTSGeom;
typedef std::shared_ptr<fungt::SimulationController> FunGTSimController;
typedef std::shared_ptr<PhysicsWorld> FunGTPhysicsWorld;

#endif // _FUNGT_H_
