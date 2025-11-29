#if !defined(_GRAPHICS_TOOL_H_)
#define _GRAPHICS_TOOL_H_
#include<iostream>
#include<fstream>
#include<string>
#include <functional>   
#include "Textures/textures.hpp"
#include "Imgui_Setup/imgui_setup.hpp"
#include "Geometries/square.hpp"
#include "Geometries/cube.hpp"
#include "Geometries/plane.hpp"
#include "Geometries/pyramid.hpp"
#include "Camera/camera.hpp"
#include "Model/model.hpp"
#include "AnimatedModel/animated_model.hpp"
#include "Animation/animation.hpp"

class GraphicsTool{

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
        void render(const std::function<void()>& renderLambda, const std::function<void()>& guiRender = nullptr);

    private: 
        void setWindowUserPointer(void* pointer);  
    protected:
        // Virtual methods that derived classes can override
        virtual void onMouseMove(double xpos, double ypos) {}
        virtual void onUpdate(float deltaTime) {}
        virtual void onRender() {}
        /* implemented in the derived class*/
        virtual void update(const std::function<void()> &renderLambda);
        void guiUpdate(const std::function<void()> &guiRender);
    public:
      
        


}; 

#endif // _GRAPHICS_TOOL_H_


