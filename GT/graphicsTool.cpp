#include "graphicsTool.hpp"


GraphicsTool::GraphicsTool(int _width, int _height)
: m_width{_width}, m_height{_height},m_Windowname{"FunGT"}{
    std::cout<<"GraphicsTool constructor "<<std::endl; 
}

GraphicsTool::~GraphicsTool(){
    std::cout<<"GraphicsTool destructor"<<std::endl; 
    // Delete window before ending the program
    glfwDestroyWindow(m_Window);
    // Terminate GLFW before ending the program
    glfwTerminate();
}
int GraphicsTool::initGL(){
    std::cout<<"Init OpenGL "<<std::endl; 
    if (!glfwInit())
    return -1;
    // Tell GLFW what version of OpenGL we are using 
    // In this case we are using OpenGL 3.3 
  
     #ifdef __APPLE__
    /* We need to explicitly ask for a 3.2 context on OS X */
    glfwWindowHint (GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint (GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint (GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint (GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    #else
    //OpenGL version string: 4.6.0 NVIDIA 390.141 <--- ACTUAL ON MY PC
    //OpenGL version : 4.4.0 <--- Using in this code (First number is MAJOR, second is the MINOR)
    glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,6);
    glfwWindowHint(GLFW_RESIZABLE,GL_TRUE);
    #endif
     GLint maxVertexUniforms;
    glGetIntegerv(GL_MAX_VERTEX_UNIFORM_COMPONENTS, &maxVertexUniforms);
   // Add DPI awareness hints BEFORE creating the window
    /*glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE);


    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    float xscale, yscale;
    glfwGetMonitorContentScale(monitor, &xscale, &yscale);

    // Adjust window dimensions based on content scale
    int scaledWidth = (int)(m_width * xscale);
    int scaledHeight = (int)(m_height * yscale);*/

    //FOR MAC USERS:
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT,GL_TRUE);
    m_Window = glfwCreateWindow(m_width,m_height,m_Windowname.c_str(), NULL, NULL);
    if (!m_Window)
    {
        glfwTerminate();
        return -1;
    }
    
    setWindowUserPointer(this);

    glfwGetFramebufferSize(m_Window,&m_frameBufferWidth,&m_frameBufferHeight);
    
    glViewport(0,0,m_frameBufferWidth,m_frameBufferHeight);
    /* Make the window's context current */
    glfwMakeContextCurrent(m_Window);
    glfwSetInputMode(m_Window,GLFW_CURSOR,GLFW_CURSOR_NORMAL);
    
    // Lambda callback that bridges GLFW C callback to our C++ virtual method
    glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xpos, double ypos) {
        GraphicsTool* instance = static_cast<GraphicsTool*>(glfwGetWindowUserPointer(window));
        if (instance) {
            instance->onMouseMove(xpos, ypos);
        }
    });

    glClearColor(m_colors[0], m_colors[1],m_colors[2],m_colors[3]);
    glfwSwapBuffers(m_Window);

  

    // Clean the back buffer and assign the new color to it
    //OPENGL OPTIONS
    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_CULL_FACE);
    // glCullFace(GL_BACK);
    glFrontFace(GL_CW);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    glPolygonMode(GL_FRONT_AND_BACK,GL_FILL); //GL_LINE for just lines //GL_FILL for fill color

        #ifdef __APPLE__
        if(glfwInit()!=GL_TRUE)
        {
            std::cout<<"ERROR"<<std::endl;
        }
    #else
        if(glewInit()!=GLEW_OK)
        {
            std::cout<<"ERROR"<<std::endl;
        }
    #endif

    glVersion = reinterpret_cast<const char*>(glGetString(GL_VERSION));
    glVendor = reinterpret_cast<const char*>(glGetString(GL_VENDOR));
    glRenderer = reinterpret_cast<const char*>(glGetString(GL_RENDERER));
 

    return 1; 

}


void GraphicsTool::setWindowUserPointer(void* pointer) {
        glfwSetWindowUserPointer(m_Window, pointer);
}

void GraphicsTool::render(const std::function<void()> &renderLambda, const std::function<void()> &guiRender)
{
     while (!glfwWindowShouldClose(m_Window)){
        /* Render here */
         glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
        //
    
        this->update(renderLambda);
                 
        /*IMGUI*/
                
        if (guiRender) {
            this->guiUpdate(guiRender);
        }
        GLenum err;
            while ((err = glGetError()) != GL_NO_ERROR) {
            //std::cerr << "OpenGL error in RenderScene: " << err << std::endl;
        }
      
        /*END IMGUI*/
        
        /* Swap front and back buffers */
        glfwSwapBuffers(m_Window);
        /* Poll for and process events */
        glfwPollEvents();
        
    }    
}


void GraphicsTool::update(const std::function<void()> &renderLambda)
{
    renderLambda();
}


void GraphicsTool::guiUpdate(const std::function<void()> &guiRender)
{
    guiRender();
}
