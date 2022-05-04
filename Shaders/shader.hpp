#if !defined(_SHADERS_H_)
#define _SHADERS_H_
#include<iostream>
#include<string>
#include<fstream>
#include<sstream>
#include <glm/glm.hpp>
#include<glm/vec2.hpp>
#include <glm/vec3.hpp>
#include<glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>
#ifndef __APPLE__
//int glewInit();
#endif
#ifdef __APPLE__
#define GLFW_INCLUDE_GLCOREARB
#include <OpenGL/gl3.h>
#include <OpenGL/gl3ext.h>
//#include <OpenGL/glu.h>
//#include <GLUT/glut.h>
#include <GLFW/glfw3.h>
#else
#ifdef _WIN32
  #include <windows.h>
#endif
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#endif
class Shader{

    GLuint idP; 

    public:
        Shader(std::string pathVert, std::string pathFrag, std::string pathgeom);
        Shader(std::string pathVert, std::string pathFrag);
        ~Shader();

      std::string loadShaderFromSource(std::string& source);
      GLuint loadShader(GLenum type, std::string& source);
      void linkProgram(GLuint vShader, GLuint geomShader, GLuint fShader);
      void Bind();
      void unBind();
      //Setting the uniforms
      void setUniform1i(const std::string &name, int value);
      void setUniformVec3f(glm::fvec3 value,std::string name);
      void setUniformVec2f(glm::fvec2 value,std::string name);
      void setUniformVec1f(GLfloat value,std::string name);
      void setMat4fv(glm::mat4 value, std::string name,GLboolean transpose);
      void setUniformMat4fv(std::string name, const glm::mat4 &proj);



};

#endif // _SHADERS_H_
