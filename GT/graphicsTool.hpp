#if !defined(_GRAPHICS_TOOL_H_)
#define _GRAPHICS_TOOL_H_
#include<iostream>
#include<fstream>
#include<string>
#include <functional>   
#include "../Textures/textures.hpp"
#include "../Imgui_Setup/imgui_setup.hpp"
#include "../Geometries/square.hpp"
#include "../Geometries/cube.hpp"
#include "../Geometries/plane.hpp"
#include "../Geometries/pyramid.hpp"
#include "../Camera/camera.hpp"
#include "../Model/model.hpp"
#include "../AnimatedModel/animated_model.hpp"
#include "../Animation/animation.hpp"


//This class uses the Curiously Recurring Template Pattern (CRTP) 
template<typename Derived> class GraphicsTool{

    protected:
        GLFWwindow *m_Window;
        int m_width,m_height;
        std::string m_Windowname;
        float m_colors[4];
        int m_frameBufferWidth = 0;
        int m_frameBufferHeight = 0;
        std::string glVersion;
        std::string glVendor;
        std::string glRenderer;
        

    public: 
        GraphicsTool(int _width, int _height);
        virtual ~GraphicsTool();
        int initGL(); //initialize OpenGL stuff


    private: 
        
        void render();
        void setWindowUserPointer(void* pointer);  
    protected:
        /* implemented in the derived class*/
        void update(); // update scenes
        void set(); //Set all the textures, meshes, shaders
        
        void update(const std::function<void()> &renderLambda);
        void guiUpdate(const std::function<void()> &guiRender);
        static void mouse_callback(GLFWwindow* window, double xpos, double ypos);
    public:
        void run(); //Run the program
        void run(const std::function<void()>& renderLambda);
        void render(const std::function<void()> &renderLambda, const std::function<void()> &guiRender = nullptr);
      
        


}; 
#include "graphicsTool.cpp"
#endif // _GRAPHICS_TOOL_H_


