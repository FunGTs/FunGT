#if !defined(_GRAPHICS_TOOL_H_)
#define _GRAPHICS_TOOL_H_
#include<iostream>
#include<fstream>
#include<string>
#include "../Textures/textures.hpp"
#include "../Imgui_Setup/imgui_setup.hpp"
#include "../Geometries/square.hpp"
#include "../Geometries/cube_2.hpp"
#include "../Geometries/pyramid.hpp"
#include "../Camera/camera.hpp"
#include "../Model/model.hpp"

//This class uses the Curiously Recurring Template Pattern (CRTP) 
template<typename Derived> class GraphicsTool{

    protected:
        GLFWwindow *m_Window;
        int m_width,m_height;
        std::string m_Windowname;
        float m_colors[4];
        int m_frameBufferWidth = 0;
        int m_frameBufferHeight = 0;
    
        

    public: 
        GraphicsTool(int _width, int _height);
        virtual ~GraphicsTool();


    private: 
        int initGL(); //initialize OpenGL stuff
        void render();
        void setWindowUserPointer(void* pointer);  
    protected:
        /* implemented in the derived class*/
        void update(); // update scenes
        void set(); //Set all the textures, meshes, shaders
        static void mouse_callback(GLFWwindow* window, double xpos, double ypos);
    public:
        void run(); //Run the program


}; 
#include "graphicsTool.cpp"
#endif // _GRAPHICS_TOOL_H_


