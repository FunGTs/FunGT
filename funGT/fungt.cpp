#include "fungt.hpp"



FunGT::FunGT(int _width, int _height)
: GraphicsTool(_width,  _height){
    std::cout<<"FunGT constructor"<<std::endl;

    m_lastXmouse = _width/2; 
    m_lastYmouse = _height/2;
    m_firstMouse = true;

    m_sceneManager = std::make_shared<SceneManager>();
    m_ViewPortLayer = std::make_unique<ViewPort>();
    m_imguiLayer = std::make_unique<ImGuiLayer>();
}
FunGT::~FunGT(){
    std::cout<<"FunGT destructor"<<std::endl; 
}
void FunGT::processKeyBoardInput()
{
   
    if (glfwGetKey(m_Window, GLFW_KEY_W) == GLFW_PRESS)
         m_camera.move(deltaTime,0);
    if (glfwGetKey(m_Window, GLFW_KEY_S) == GLFW_PRESS)
        m_camera.move(deltaTime,1);
    if (glfwGetKey(m_Window, GLFW_KEY_A) == GLFW_PRESS)
        m_camera.move(deltaTime,2);
    if (glfwGetKey(m_Window, GLFW_KEY_D) == GLFW_PRESS)
        m_camera.move(deltaTime,3);
    
}
void FunGT::processMouseInput(double xpos, double ypos)
{

    if (m_firstMouse)
    {
        m_lastXmouse = xpos;
        m_lastYmouse = ypos;
        m_firstMouse = false;
    }
    float xoffset = xpos - m_lastXmouse;
    float yoffset = m_lastYmouse - ypos; // reversed since y-coordinates go from bottom to top

    m_lastXmouse = xpos; 
    m_lastYmouse = ypos; 

    m_camera.updateMouseInput(xoffset,yoffset); 
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

Camera FunGT::getCamera()
{
    return m_camera;
}

std::shared_ptr<SceneManager> FunGT::getSceneManager()
{
    return m_sceneManager;
}
void FunGT::set(const std::function<void()>& renderLambda){
    std::cout << "Starting FunGT Setting process..." << std::endl;

    renderLambda();


    ProjectionMatrix = glm::perspective(glm::radians(fov),
                                        static_cast<float>(m_frameBufferWidth)/m_frameBufferHeight,nearPlane, farPlane);

   for(const auto& node : m_sceneManager->getRenderable()){

        node->getShader().Bind();
       

    }


   // SETUP IMGUI LAYERS - ALWAYS (no m_useGUI flag!)
   if (m_imguiLayer) {
       m_imguiLayer->setNativeWindow(*m_Window, m_frameBufferWidth, m_frameBufferHeight);
       m_imguiLayer->addWindow(std::make_unique<RenderInfoWindow>());
       m_layerStack.PushLayer(std::move(m_imguiLayer));
   }

   if (m_ViewPortLayer) {
       m_ViewPortLayer->setRenderFunction([this]() {
           // Render all models managed by the SceneManager
                m_sceneManager->renderScene();
        });
       m_layerStack.PushLayer(std::move(m_ViewPortLayer));
   }

   // UPDATE PROJECTION MATRIX BASED ON VIEWPORT SIZE
   auto* view_port = m_layerStack.get<ViewPort>();
   if (view_port)
   {
       auto view_port_size = view_port->getViewPortSize();
       if (view_port_size.x > 0 && view_port_size.y > 0) {
           ProjectionMatrix = glm::perspective(glm::radians(fov),
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

    float currentFrame = glfwGetTime();
    deltaTime = currentFrame-lastFrame; 
    lastFrame = currentFrame; 

    m_sceneManager->setDeltaTime(deltaTime);


    processKeyBoardInput();
    glm::mat4 ViewMatrix = glm::mat4(glm::mat3(m_camera.getViewMatrix()));

    // GET VIEWPORT SIZE FROM VIEWPORT LAYER
    auto* view_port = m_layerStack.get<ViewPort>();
    if (view_port) {
        auto view_port_size = view_port->getViewPortSize();
        if (view_port_size.x > 0 && view_port_size.y > 0) {
            ProjectionMatrix = glm::perspective(glm::radians(fov),
                view_port_size.x / view_port_size.y, nearPlane, farPlane);
        }
    }

    m_sceneManager->updateViewMatrix(m_camera.getViewMatrix());
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