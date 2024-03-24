#include "fungt.hpp"



FunGT::FunGT(int _width, int _height)
: GraphicsTool(_width,  _height){
    std::cout<<"FunGT constructor"<<std::endl;

    m_lastXmouse = _width/2; 
    m_lastYmouse = _height/2;
    m_firstMouse = true;
}
FunGT::~FunGT(){
    std::cout<<"FunGT destructor"<<std::endl; 
}

void FunGT::set() {
    std::cout<<"Setting OpenGL "<<std::endl; 
    #ifdef __APPLE__
        #define GLFW_INCLUDE_GLCOREARB
             m_shader.create("../resources/vertex_core_OSX.glsl","../resources/fragment_core_OSX.glsl");
        #else
             m_shader.create("../resources/vertex_cube.glsl","../resources/fragment_cube.glsl");
        #endif


    // /* Here it starts to show an image*/
    //Square square{};
    // Square cube{};
    // for(int i = 0; i<2; i++){
    //     Cube input{"../img/box.jpg"};
    //     cube.push_back(input); 
    // }
    cube.create("../img/box.jpg");
    pyr.create("../img/stone2.jpg");
    //m_model.loadModel("../Obj/backpack.obj");

    
    //Creating model matrix to perform movements 
     //INIT MATRICES
     position.z = 1.0;
   

    //Projection matrix 

    ProjectionMatrix = glm::perspective(glm::radians(fov),
                                        static_cast<float>(m_frameBufferWidth)/m_frameBufferHeight,nearPlane, farPlane);
    //Model Matrix
    ModelMatrix = glm::translate(ModelMatrix, position);
    ModelMatrix = glm::rotate(ModelMatrix, glm::radians(rotation.x), glm::vec3(1.f, 0.f, 0.f));
    ModelMatrix = glm::rotate(ModelMatrix, glm::radians(rotation.y), glm::vec3(0.f, 1.f, 0.f)); 
    ModelMatrix = glm::rotate(ModelMatrix, glm::radians(rotation.z), glm::vec3(0.f, 0.f, 1.f));
    ModelMatrix = glm::scale(ModelMatrix, scale);


    //once the matrices have been created, let's send them to the shader
     m_shader.Bind(); //Important to bind shader
        m_shader.setUniformMat4fv("ModelMatrix",ModelMatrix);
        m_shader.setUniformMat4fv("ViewMatrix",m_camera.getViewMatrix());
        m_shader.setUniformMat4fv("ProjectionMatrix",ProjectionMatrix);
    m_shader.unBind();

                                    
    
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
    if(fungtInstance!=nullptr){
        fungtInstance->processMouseInput(xpos,ypos); 
        //std::cout<< "not nullptr "<<std::endl;
    }
    else{
        std::cout<< "nullptr "<<std::endl;
    }


}

void FunGT::setBackground(float red, float green, float blue, float alfa){

    m_colors[0] = red; m_colors[1] = green; m_colors[2] = blue; m_colors[3] = alfa; 

}

void FunGT::update() {
    
    m_shader.Bind(); //To give instructions to the gpu 

    rotation.y = (float)glfwGetTime()*10.0;
    rotation.x = (float)glfwGetTime()*10.0;
    rotation.z = (float)glfwGetTime()*10.0;
    ModelMatrix = glm::mat4(1.f);
    ModelMatrix = glm::translate(ModelMatrix, position);
    ModelMatrix = glm::rotate(ModelMatrix, glm::radians(rotation.x), glm::vec3(1.f, 0.f, 0.f));
    ModelMatrix = glm::rotate(ModelMatrix, glm::radians(rotation.y), glm::vec3(0.f, 1.f, 0.f)); 
    ModelMatrix = glm::rotate(ModelMatrix, glm::radians(rotation.z), glm::vec3(0.f, 0.f, 1.f));
    ModelMatrix = glm::scale(ModelMatrix, scale); 

    m_shader.setUniformMat4fv("ModelMatrix",ModelMatrix);
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame-lastFrame; 
    lastFrame = currentFrame; 
    processKeyBoardInput();
    glm::mat4 ViewMatrix(1.f);
        //ViewMatrix = glm::lookAt(cameraPos,cameraPos+cameraFront, cameraUP);
    m_shader.setUniformMat4fv("ViewMatrix",m_camera.getViewMatrix());
        //m_shader.setUniformVec2f(mouseInput,"mouseInput");
        //m_shader.setUniform1f("time",currentFrame);
    cube.draw();

    glFlush();
}

