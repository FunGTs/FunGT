#include "fungt.hpp"



FunGT::FunGT(int _width, int _height)
: GraphicsTool(_width,  _height){
    std::cout<<"FunGT constructor"<<std::endl;

    m_lastXmouse = _width/2; 
    m_lastYmouse = _height/2;
    m_firstMouse = true;

    m_sceneManager  = std::make_shared<SceneManager>();
    m_infoWindow    = std::make_shared<InfoWindow>();
    m_ViewPortLayer = std::make_unique<ViewPort>();

    m_layerStack.PushLayer(std::move(m_ViewPortLayer)); //Owns now to the stack Layer;

}
FunGT::~FunGT(){
    std::cout<<"FunGT destructor"<<std::endl; 
}

void FunGT::set() {
    std::cout<<"Setting OpenGL ************************** "<<std::endl; 
    // #ifdef __APPLE__
    //     #define GLFW_INCLUDE_GLCOREARB
    //          m_shader.create("../resources/vertex_core_OSX.glsl","../resources/fragment_core_OSX.glsl");
    //     #else
    //         // m_shader.create("../resources/pixar_vs.glsl","../resources/pixar_fs.glsl");
    //          m_shader.create("../resources/animation_material_vs.glsl","../resources/animation_material_fs.glsl");
    //          //m_shader.create("../resources/model_loading_vs.glsl","../resources/model_loading_fs.glsl");
    //         // m_shader.create("../resources/vertex_cube.glsl","../resources/fragment_cube.glsl");
    //     #endif

    //glm::vec3 scale = glm::vec3(0.1);
    std::vector<std::string> faces
    {
        "../img/sky/right.jpg",
        "../img/sky/left.jpg",
        "../img/sky/top.jpg",
        "../img/sky/bottom.jpg",
        "../img/sky/front.jpg",
        "../img/sky/back.jpg"
    };
    //m_shader = m_sceneManager->getShader();
    //m_aModel = std::make_shared<AnimatedModel>(); 
    //m_model = std::make_unique<Model>();
    //m_aModel->loadModel("../Animations/bob/boblampclean.md5mesh");
    //m_model->loadModel("../Obj/backpack.obj");
    //m_model->loadModel("../Obj/luxo/Luxo.obj");
    //m_aModel->loadModel("../Animations/Luxo/Luxo-Jr-Model-Anim.dae"); 
    //m_aModel->loadModel("../Animations/SillyDancing/SillyDancing.dae");
    // if(m_animation==nullptr || m_sceneManager==nullptr){
    //     std::cout<<"Invalid animation pointer"<<std::endl;
    //     exit(0);
    // }else{
    //     std::cout<<"**** Correct animation pointer ********"<<std::endl;
    // }
    // m_animation->load("../Animations/trashcan/trash-can-color2.gltf");
    //animation.load("../Animations/SillyDancing/SillyDancing.dae"); 
    //m_aModel->loadModel("../Animations/FBX/def.fbx"); 
    //m_aModel->loadModel("../Animations/PF/Pointing_Forward.dae"); 
    //m_aModel->loadModel("../Animations/vampire/dancing_vampire.dae"); 
    //m_aModel->loadModel("../Animations/Jump/Jump.dae"); 
    //m_aModel->loadModel("../Animations/raptoid/scene.gltf"); 
    //m_aModel->loadModel("../Animations/car3/source/FC-6.fbx"); 
    //m_aModel->loadModel("../Animations/Capoeira/Capoeira.dae"); 

    // float delta = 1.0f;
    // animation.update(delta);
    // auto transforms = animation.getFinalBoneMatrices(); 
    // std::cout<<"SIZE OF MATRIX TRANSFORMS VECTOR : "<< transforms.size() <<std::endl;
    // //Shape 2
    // plane = std::make_unique<Plane>(0.0,0.0,1.0);
    // plane->create("../img/wood.png");
    // plane->setScale(scale);
    // plane->setModelMatrix();
  
    // //Cube
    //m_cube_map = std::make_unique<CubeMap>(0.0,0.0,0.0);
    
    for(std::size_t i = 0; i<faces.size(); i++){
        std::cout<<faces[i]<<std::endl;
    }
    //m_cube_map->create(faces);
    // Cube Map
    // cube = std::make_unique<Cube>(-1.0,0.5,1.0);
    // cube->create("../img/box.jpg");
    // cube->setModelMatrix();
    
   

    //position.z = -20; 
    //Projection matrix 

    ProjectionMatrix = glm::perspective(glm::radians(fov),
                                        static_cast<float>(m_frameBufferWidth)/m_frameBufferHeight,nearPlane, farPlane);
    //Model Matrix
   // rotation.y = -20.f; 
    ModelMatrix = glm::translate(ModelMatrix, position);
    ModelMatrix = glm::rotate(ModelMatrix, glm::radians(rotation.x), glm::vec3(1.f, 0.f, 0.f));
    ModelMatrix = glm::rotate(ModelMatrix, glm::radians(rotation.y), glm::vec3(0.f, 1.f, 0.f)); 
    ModelMatrix = glm::rotate(ModelMatrix, glm::radians(rotation.z), glm::vec3(0.f, 0.f, 1.f));
    ModelMatrix = glm::scale(ModelMatrix, scale);


    //once the matrices have been created, let's send them to the shader
    
    //plane.setModelMatrix();
     
    m_sceneManager->getShader().Bind(); //Important to bind shader
        //m_sceneManager->getShader().setUniform1i("skybox",0);
        m_sceneManager->getShader().setUniformMat4fv("ModelMatrix",ModelMatrix);
        //m_shader.setUniformMat4fv("ModelMatrix",cube->getModelMatrix());
        //m_sceneManager->getShader().setUniformVec3f(m_camera.getPosition(),"ViewPos");
        //m_sceneManager->getShader().setUniformMat4fv("ModelMatrix",ModelMatrix);
        glm::mat4 ViewMatrix = glm::mat4(glm::mat3(m_camera.getViewMatrix()));
        m_sceneManager->getShader().setUniformMat4fv("ViewMatrix",ViewMatrix);
        m_sceneManager->getShader().setUniformMat4fv("ProjectionMatrix",ProjectionMatrix);
    m_sceneManager->getShader().unBind();
    
    //m_model.draw(m_shader); 
    
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

void FunGT::mouse_callback(GLFWwindow *window, double xpos, double ypos){
    //std::cout<<" fungT: mouse callback "<<std::endl; 
    FunGT * fungtInstance =  static_cast<FunGT*>(glfwGetWindowUserPointer(window));
    if (fungtInstance != nullptr) {
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS){
            fungtInstance->processMouseInput(xpos, ypos);
        }
        
    }
    else{
        std::cout<< "nullptr "<<std::endl;
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

std::shared_ptr<GUI> FunGT::getInfoWindow()
{
    return m_infoWindow;
}

void FunGT::set(const std::function<void()>& renderLambda){
    std::cout << "Starting FunGT Setting process..." << std::endl;

    renderLambda();


    ProjectionMatrix = glm::perspective(glm::radians(fov),
                                        static_cast<float>(m_frameBufferWidth)/m_frameBufferHeight,nearPlane, farPlane);

   for(const auto& node : m_sceneManager->getRenderable()){

        node->getShader().Bind();
       

    }

    m_infoWindow->setup(*m_Window);

    std::cout << "Finished Setting process..." << std::endl;

}

std::unique_ptr<FunGT> FunGT::createScene(int _width, int _height)
{
    return std::make_unique<FunGT>(_width, _height);
}

void FunGT::update() {

    for(const auto& node : m_sceneManager->getRenderable()){

        node->getShader().Bind();
       

    }
    //m_sceneManager->getCubeMap().getShader().Bind(); //To give instructions to the gpu 
    glDepthFunc(GL_LEQUAL);
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame-lastFrame; 
    lastFrame = currentFrame; 

    //animation.update(deltaTime);
    rotation.y = (float)glfwGetTime()*10.0;
    //rotation.x = (float)glfwGetTime()*10.0;
    // rotation.z = (float)glfwGetTime()*10.0;
    ModelMatrix = glm::mat4(1.f);
    ModelMatrix = glm::translate(ModelMatrix, position);
    ModelMatrix = glm::rotate(ModelMatrix, glm::radians(rotation.x), glm::vec3(1.f, 0.f, 0.f));
    ModelMatrix = glm::rotate(ModelMatrix, glm::radians(rotation.y), glm::vec3(0.f, 1.f, 0.f)); 
    ModelMatrix = glm::rotate(ModelMatrix, glm::radians(rotation.z), glm::vec3(0.f, 0.f, 1.f));
    ModelMatrix = glm::scale(ModelMatrix, scale);
     
    //m_sceneManager->getShader().setUniformMat4fv("ModelMatrix",ModelMatrix);

    // m_animation->updateTime(deltaTime);
    // m_animation->display( m_sceneManager->getShader() );

    //m_model->draw(m_shader);   
    //m_cube_map->draw();
    
         

    //   plane->updateModelMatrix(rotation.y);
    //    m_shader.setUniformMat4fv("ModelMatrix",plane->getModelMatrix());
    //    plane->draw();

    processKeyBoardInput();
    glm::mat4 ViewMatrix = glm::mat4(glm::mat3(m_camera.getViewMatrix()));

    ProjectionMatrix = glm::perspective(glm::radians(fov),
                                        static_cast<float>(m_frameBufferWidth)/m_frameBufferHeight,nearPlane, farPlane);
        //ViewMatrix = glm::lookAt(cameraPos,cameraPos+cameraFront, cameraUP);
    m_sceneManager->updateViewMatrix(m_camera.getViewMatrix());
    m_sceneManager->updateProjectionMatrix(ProjectionMatrix);
    m_sceneManager->renderScene();
    //  m_sceneManager->getCubeMap().getShader().setUniformMat4fv("ViewMatrix",ViewMatrix);
    //  m_sceneManager->getCubeMap().getShader().setUniformMat4fv("ProjectionMatrix",ProjectionMatrix);
        //m_shader.setUniformVec2f(mouseInput,"mouseInput");
        //m_shader.setUniform1f("time",currentFrame);
    //shape->draw();
    //m_sceneManager->getCubeMap().draw();
    glDepthFunc(GL_LESS);
    glFlush();
}

void FunGT::update(const std::function<void()> &renderLambda)
{
    // for(const auto& node : m_sceneManager->getRenderable()){

    //     node->getShader().Bind();
       

    // }
    //m_sceneManager->getCubeMap().getShader().Bind(); //To give instructions to the gpu 
    //glDepthFunc(GL_LEQUAL);
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame-lastFrame; 
    lastFrame = currentFrame; 

    m_sceneManager->setDeltaTime(deltaTime);

    //animation.update(deltaTime);
    //rotation.y = (float)glfwGetTime()*10.0;
    //rotation.x = (float)glfwGetTime()*10.0;
    // rotation.z = (float)glfwGetTime()*10.0;
    /*ModelMatrix = glm::mat4(1.f);
    ModelMatrix = glm::translate(ModelMatrix, position);
    ModelMatrix = glm::rotate(ModelMatrix, glm::radians(rotation.x), glm::vec3(1.f, 0.f, 0.f));
    ModelMatrix = glm::rotate(ModelMatrix, glm::radians(rotation.y), glm::vec3(0.f, 1.f, 0.f)); 
    ModelMatrix = glm::rotate(ModelMatrix, glm::radians(rotation.z), glm::vec3(0.f, 0.f, 1.f));
    ModelMatrix = glm::scale(ModelMatrix, scale);*/
     

    processKeyBoardInput();
    glm::mat4 ViewMatrix = glm::mat4(glm::mat3(m_camera.getViewMatrix()));

    ProjectionMatrix = glm::perspective(glm::radians(fov),
                                        static_cast<float>(m_frameBufferWidth)/m_frameBufferHeight,nearPlane, farPlane);

    m_sceneManager->updateViewMatrix(m_camera.getViewMatrix());
    m_sceneManager->updateProjectionMatrix(ProjectionMatrix);
    //m_sceneManager->updateModelMatrix(ModelMatrix);
    
    renderLambda();



    //glDepthFunc(GL_LESS);
    glFlush();
}

void FunGT::guiUpdate(const std::function<void()> &guiRender)
{
    guiRender();
}
