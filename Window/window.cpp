#include "window.hpp"


Window::Window()
:    width{1600}, height{100}, Windowname{"FunGL"},GLVersionMajor{4},GLVersionMinor{4}{
     std::cout<<"Window def constructor\n";
        this->initWindow(); 
}
Window::Window(int _width, int _height, std::string _name)
:   width{_width}, height{_height}, Windowname{_name},GLVersionMajor{4},GLVersionMinor{4}{
        std::cout<<"Window constructor\n";
        this->initGLFW();
        this->initWindow();
        this->initiGlew();
        this->openGLOptions(); 
}
void Window::initGLFW(){
      if (!glfwInit()){
        std::cout<<"ERROR::INIT::GLFW\n";
        glfwTerminate();
      }
}
int Window::getWindowShouldClose(){

    return glfwWindowShouldClose(this->window); 
}
void Window::initWindow(){

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
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,GLVersionMajor);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,GLVersionMinor);
    glfwWindowHint(GLFW_RESIZABLE,GL_FALSE);
    #endif
    //FOR MAC USERS:

    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT,GL_TRUE);
    window = glfwCreateWindow(width,height,Windowname.c_str(), NULL, NULL);
    if (!window)
    {
        std::cout<<"ERROR CREATING WINDOW\n";
        glfwTerminate();
    }
    glfwGetFramebufferSize(window,&frameBufferWidth,&frameBufferHeight);
    glViewport(0,0,frameBufferWidth,frameBufferWidth);
    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    

}
void Window::openGLOptions(){
    glClearColor(colors[0], colors[1],colors[2],colors[3]);
    glfwSwapBuffers(window);
    // Clean the back buffer and assign the new color to it
    //OPENGL OPTIONS
    glEnable(GL_DEPTH_TEST);
    // glEnable(GL_CULL_FACE);
    // glCullFace(GL_BACK);
    glFrontFace(GL_CW);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    glPolygonMode(GL_FRONT_AND_BACK,GL_FILL); //GL_LINE for just lines //GL_FILL for fill color
}
void Window::initiGlew(){

    #ifdef __APPLE__
        if(glfwInit()!=GL_TRUE)
        {
            std::cout<<"ERROR"<<std::endl;
        }
    #else
        if(glewInit()!=GLEW_OK)
        {
            std::cout<<"ERROR::GLEWINIT"<<std::endl;
            glfwTerminate();
        }
    #endif
}
Window::~Window(){
    std::cout<<"Window destructor\n";
    // Delete window before ending the program
    glfwDestroyWindow(window);
    // Terminate GLFW before ending the program
    glfwTerminate();
}