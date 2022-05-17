#if !defined(_DISPLAY_H_)
#define _DISPLAY_H_
#include "../Mesh/fungtMesh.hpp"
#include "../Textures/textures.hpp"
#include "../Imgui_Setup/imgui_setup.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include<iostream>
#include<fstream>
#include<string>

class Display{

    GLFWwindow *window;
    int width,height;
    std::string Windowname;
    float colors[4];

public: 
      Display(int width, int height, std::string name);
      Display();
      ~Display();
      // Methods
      int build();
      int set();
      void setBackground(float red, float green, float blue, float alfa);
      void show();

};


#endif // _DISPLAY_H_
