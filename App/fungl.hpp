#if !defined(_FUNGL_AP_H_)
#define _FUNGL_AP_H_
#include "../Mesh/fungtMesh.hpp"
#include "../Textures/textures.hpp"
#include "../Imgui_Setup/imgui_setup.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include<iostream>
#include<fstream>
#include<string>
#include "../Geometries/square.hpp"
#include "../Geometries/cube_2.hpp"
#include "../Geometries/pyramid.hpp"

class FunGL{

    GLFWwindow *window;
    int width,height;
    std::string Windowname;
    float colors[4];


public: 
      FunGL(int width, int height, std::string name);
      FunGL();
      ~FunGL();
      // Methods
      int build();
      int set();
      void setBackground(float red, float green, float blue, float alfa);
      void show();
      void processInput(glm::vec3 &cameraUp,glm::vec3 &cameraFront,glm::vec3 &cameraPos,float &currentFrame);
      

};

void mouse_callback(GLFWwindow *window,  double xpos, double ypos);
#endif // _FUNGL_AP_H_
