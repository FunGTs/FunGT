#if !defined(_PREREQ_H_)
#define _PREREQ_H_

#include<iostream>
#include<string>
#include<fstream>
#include<sstream>

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
#define CHECK_GL_ERROR() { \
  GLenum err; \
  while ((err = glGetError()) != GL_NO_ERROR) { \
    std::cerr << "\033[1;31mOpenGL Error: " << err << " in file " << __FILE__ << " at line " << __LINE__ << "\033[0m" << std::endl; \
  } \
}
#endif // _PREREQ_H_

