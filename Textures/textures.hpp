#if !defined(_TEXTURES_H_)
#define _TEXTURES_H_
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
#include<iostream>
#include<string>
class Texture{

    private:
        unsigned int txt_ID;
        std::string txt_Path;
        unsigned char* txt_localBuffer;
        int txt_width, txt_height,txt_BBP;
    public: 
        Texture(const std::string  &path );
        ~Texture();


        void bind(unsigned int slot=0);
        void unBind();


        inline int getWidth() const {return txt_width;}
        inline int getHeight() const {return txt_height;};

};


#endif // _TEXTURES_H_
