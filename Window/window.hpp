#if !defined(_WINDOW_H_)
#define _WINDOW_H_
#include "../Mesh/fungtMesh.hpp"
#include "../Textures/textures.hpp"
#include "../Imgui_Setup/imgui_setup.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include<iostream>
#include<fstream>
#include<string>
class Window {

    public:
        //window stuff
        GLFWwindow *window;
        int width,height;
        int frameBufferWidth = 0;
        int frameBufferHeight = 0;
        std::string Windowname;
        //OpenGL context
        const int GLVersionMajor; 
        const int GLVersionMinor;
        float colors[4];
        Window();
        Window(int _width, int _height, std::string _name);
        ~Window();
        //Accessors
        int getWindowShouldClose();
        //Funct
        void update();
        void render();
    private: 
        void initGLFW();
        void initWindow();
        void initiGlew(); //After context creation
        void openGLOptions();
        //Function

 

};
#endif // _WINDOW_H_
