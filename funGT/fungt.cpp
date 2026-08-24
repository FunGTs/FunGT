#include "fungt.hpp"



FunGT::FunGT(int _width, int _height)
: GraphicsTool(_width,  _height){
    std::cout<<"FunGT constructor"<<std::endl;

    m_lastXmouse = _width/2; 
    m_lastYmouse = _height/2;
    m_firstMouse = true;

    m_sceneManager = std::make_shared<SceneManager>();
    m_ViewPortLayer = ViewPort::create();
    m_imguiLayer = std::make_unique<ImGuiLayer>();
    // Create simulation controller
    m_simController = std::make_shared<fungt::SimulationController>();

    //Progressive Path Tracer
    m_progressiveTracer = ProgressivePathTracer::create();
    
    
}
FunGT::~FunGT(){
    std::cout<<"FunGT destructor"<<std::endl; 
}
void FunGT::processKeyBoardInput()
{
   
    // if (glfwGetKey(m_Window, GLFW_KEY_W) == GLFW_PRESS)
    //      m_camera.move(deltaTime,0);
    // if (glfwGetKey(m_Window, GLFW_KEY_S) == GLFW_PRESS)
    //     m_camera.move(deltaTime,1);
    // if (glfwGetKey(m_Window, GLFW_KEY_A) == GLFW_PRESS)
    //     m_camera.move(deltaTime,2);
    // if (glfwGetKey(m_Window, GLFW_KEY_D) == GLFW_PRESS)
    //     m_camera.move(deltaTime,3);
    
}
void FunGT::processMouseInput(double xpos, double ypos)
{

    if (m_firstMouse) {
        m_lastXmouse = xpos;
        m_lastYmouse = ypos;
        m_firstMouse = false;
        return;
    }

    float xoffset = xpos - m_lastXmouse;
    float yoffset = ypos - m_lastYmouse;
    m_lastXmouse = xpos;
    m_lastYmouse = ypos;

    bool shiftPressed = (glfwGetKey(m_Window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS ||
        glfwGetKey(m_Window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS);

    if (shiftPressed) {
        m_camera.pan(xoffset, yoffset);
    }
    else {
        m_camera.orbit(xoffset, yoffset);
    }
}

// NEW: Override the virtual method from GraphicsTool
void FunGT::onMouseMove(double xpos, double ypos) {
    // Only process if middle mouse button is pressed
    if (glfwGetMouseButton(m_Window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS) {
        processMouseInput(xpos, ypos);
    }
}

void FunGT::setBackgroundColor(float red, float green, float blue, float alfa){

    m_colors[0] = red/255.f; m_colors[1] = green/255.f; m_colors[2] = blue/255.f; m_colors[3] = alfa; 

}

void FunGT::setBackgroundColor(float color){
     m_colors[0] = color/255.f; m_colors[1] = color/255.f; m_colors[2] = color/255.f; m_colors[3] = 1.0; 
}

Camera &FunGT::getCamera()
{
    return m_camera;
}

std::shared_ptr<SceneManager> FunGT::getSceneManager()
{
    return m_sceneManager;
}
void FunGT::set(const std::function<void()>& renderLambda){
    std::cout << "Starting FunGT Setting process..." << std::endl;
    //Lights:
    SceneLight defaultLight;
    defaultLight.name = "Point Light";
    defaultLight.type = SceneLightType::Point;
    defaultLight.position = glm::vec3(-5.0f, 8.0f, 4.0f);
    defaultLight.color = glm::vec3(1.f, 1.f, 1.f);
    defaultLight.power = 10.f;
    m_sceneManager->addLight(defaultLight);
    m_grid = std::make_shared<InfiniteGrid>();
    std::string grid_vs = getAssetPath("shaders/grid_vs.glsl");
    std::string grid_fs = getAssetPath("shaders/grid_fs.glsl");
    m_grid->init(grid_vs,grid_fs);
    m_grid->setPlanes(nearPlane, farPlane);

    m_sceneManager->addRenderableObj(m_grid);
    renderLambda();
    

    ProjectionMatrix = glm::perspective(glm::radians(m_camera.getFOV()),
                                        static_cast<float>(m_frameBufferWidth)/m_frameBufferHeight,nearPlane, farPlane);

   for(const auto& node : m_sceneManager->getRenderable()){

        node->getShader().Bind();
       

    }
   // CREATE PHYSICS DEBUG RENDERER (NOT a layer! Just a utility)
   if (m_collisionManager) {
      
       if(m_collisionManager->isShowingCollidableBodies()) {
           m_physicsDebugRenderer = std::make_unique<PhysicsDebugRenderer>(m_collisionManager, m_sceneManager, &m_camera);
           m_physicsDebugRenderer->setShowCollisionBoxes(true);
           std::cout << "Collidable bodies will be shown in debug renderer." << std::endl;
       }
       // CREATE ANIMATION CONTROLLER
       m_animationController = std::make_shared<fungt::AnimationController>(m_sceneManager);
       
   }
   //Light gizmo renderer
   m_lightGizmoRenderer = std::make_unique<LightGizmoRenderer>(m_sceneManager.get(), &m_camera);
   m_lightGizmoRenderer->init();
   // SETUP IMGUI LAYERS - ALWAYS (no m_useGUI flag!)
   if (m_imguiLayer) {
       m_imguiLayer->setNativeWindow(*m_Window, m_frameBufferWidth, m_frameBufferHeight);
       m_imguiLayer->addWindow(std::make_unique<SceneHierarchyWindow>(m_sceneManager, &m_camera));
       m_imguiLayer->addWindow(std::make_unique<PropertiesWindow>(&m_camera));
       m_imguiLayer->addWindow(std::make_unique<RenderInfoWindow>());
       m_imguiLayer->addWindow(std::make_unique<LightEditorWindow>(m_sceneManager));
       m_imguiLayer->addWindow(std::make_unique<MaterialEditorWindow>(m_sceneManager));
       //m_imguiLayer->addWindow(std::make_unique<ParticleRTCWindow>());
       if(m_ViewPortLayer){
           ViewPort* viewportPtr = m_ViewPortLayer.get();
           m_imguiLayer->addWindow(std::make_unique<RenderWindow>(m_sceneManager, &m_camera, viewportPtr, m_imguiLayer->getGPUManager()));
       }
       m_imguiLayer->addWindow(std::make_unique<ParticleSimDemoWindow>(m_sceneManager));
       m_imguiLayer->addWindow(std::make_unique<PhysicsControlWindow>(m_simController));
       if(m_animationController){
           auto animWindow = std::make_unique<AnimationControlWindow>(m_animationController, m_sceneManager, &m_camera);
           m_animWindow = animWindow.get();
           m_imguiLayer->addWindow(std::move(animWindow));
       }
       m_layerStack.PushLayer(std::move(m_imguiLayer));
   }

   if (m_ViewPortLayer) {
       m_ViewPortLayer->setRenderFunction([this]() {
           // Render all models managed by the SceneManager
                m_sceneManager->renderScene();
                // Second: Render wireframes ON TOP of scene (before ImGui!)
                if (m_physicsDebugRenderer) {
                    m_physicsDebugRenderer->render();
                }
                if (m_lightGizmoRenderer) {
                    m_lightGizmoRenderer->render(ProjectionMatrix);
                }
        });
        m_ViewPortLayer->setPathTraceInteropFunction([this](int width, int height, int sample) {
                auto* viewport = m_layerStack.get<ViewPort>();
                if (!viewport) return;

                if (!m_progressiveTracer->isInitialized()) {
                    m_progressiveTracer->initialize(
                        &m_camera, m_sceneManager, width, height, true);
                    m_sceneManager->clearRaySpaceDirty(
                        SceneManager::RaySpaceDirtyLights |
                        SceneManager::RaySpaceDirtyShading);
                } else if (sample == 0) {
                    m_progressiveTracer->updateCamera(
                        &m_camera, width, height);
                    if (m_sceneManager->isRaySpaceDirty(SceneManager::RaySpaceDirtyLights)) {
                        m_progressiveTracer->reloadLights(m_sceneManager);
                        m_sceneManager->clearRaySpaceDirty(SceneManager::RaySpaceDirtyLights);
                    }
                    if (m_sceneManager->isRaySpaceDirty(SceneManager::RaySpaceDirtyShading)) {
                        m_progressiveTracer->reloadSceneShading(m_sceneManager);
                        m_sceneManager->clearRaySpaceDirty(SceneManager::RaySpaceDirtyShading);
                    }
                }

                m_progressiveTracer->renderSampleInterop(
                    sample, viewport->getPathTracePBO());
                viewport->copyPBOToTexture(width, height);
            });
        m_ViewPortLayer->setPathTraceFunction([this](int width, int height, int sample) {
                auto* viewport = m_layerStack.get<ViewPort>();
                if (!viewport) return;

                if (sample == 0 || !m_progressiveTracer->isInitialized()) {
                    m_progressiveTracer->initialize(&m_camera, m_sceneManager, width, height);
                }
                //Render one Sample
                m_progressiveTracer->renderSample(sample, viewport->getPathTraceTexture());
            });
        m_ViewPortLayer->setPathTraceReleaseInteropFunction([this]() {
            m_progressiveTracer->releaseOpenGLInteropResources();
        });
       m_layerStack.PushLayer(std::move(m_ViewPortLayer));
   }

   // UPDATE PROJECTION MATRIX BASED ON VIEWPORT SIZE
   auto* view_port = m_layerStack.get<ViewPort>();
   if (view_port)   
   {
       auto view_port_size = view_port->getViewPortSize();
       if (view_port_size.x > 0 && view_port_size.y > 0) {
           ProjectionMatrix = glm::perspective(glm::radians(m_camera.getFOV()),
               view_port_size.x / view_port_size.y, nearPlane, farPlane);
       }
   }
    std::cout << "Finished Setting process..." << std::endl;

}

std::unique_ptr<FunGT> FunGT::createScene(int _width, int _height)
{
    return std::make_unique<FunGT>(_width, _height);
}

void FunGT::update(const std::function<void()> &renderLambda)
{


    processKeyBoardInput();
    glm::mat4 ViewMatrix = glm::mat4(glm::mat3(m_camera.getViewMatrix()));
    glm::mat4 currentView = m_camera.getViewMatrix();
    if (currentView != m_lastViewMatrix) {
        auto* viewport = m_layerStack.get<ViewPort>();
        if (viewport && viewport->isPathTracing()) {
            viewport->resetAccumulation();  // Reset sample count to 0
        }
        m_lastViewMatrix = currentView;
    }
    // GET VIEWPORT SIZE FROM VIEWPORT LAYER
    auto* view_port = m_layerStack.get<ViewPort>();
    if (view_port) {
        auto view_port_size = view_port->getViewPortSize();
        if (view_port_size.x > 0 && view_port_size.y > 0) {
            ProjectionMatrix = glm::perspective(glm::radians(m_camera.getFOV()),
                view_port_size.x / view_port_size.y, nearPlane, farPlane);
        }
    }

    m_sceneManager->updateViewMatrix(m_camera.getViewMatrix());
    m_sceneManager->updateViewPos(m_camera.getPosition());
    m_sceneManager->updateProjectionMatrix(ProjectionMatrix);

    renderLambda();

    glFlush();
}
void FunGT::renderGUI()
{
    // --- Update all layers ---
    for (auto& layer : m_layerStack) {
        layer->onUpdate();
    }

    // --- Start ImGui frame ---
    for (auto& layer : m_layerStack) {
        layer->begin();
    }

    // --- Render ImGui content ---
    for (auto& layer : m_layerStack) {
        layer->onImGuiRender();
    }

    // --- End ImGui frame ---
    for (auto& layer : m_layerStack) {
        layer->end();
    }
}
void FunGT::onUpdate(float dt) {
    deltaTime = dt;
    // This is called by base class, but we're not using it much yet
}

void FunGT::onMouseScroll(double xoffset, double yoffset)
{
    m_camera.zoom(-yoffset);
}
