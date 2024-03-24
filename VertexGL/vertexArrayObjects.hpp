#if !defined(_VERTEX_ARRAY_OBJECS_H_)
#define _VERTEX_ARRAY_OBJECS_H_
#ifdef __APPLE__
#define GLFW_INCLUDE_GLCOREARB
#include <OpenGL/gl3.h>
#include <OpenGL/gl3ext.h>
#else
#include <GL/glew.h>
#endif

class VertexArrayObject{
    unsigned int id_vao;


    public:
        VertexArrayObject(int);
        VertexArrayObject();
        ~VertexArrayObject();
        void genVAO();
        void bind();
        void unbind();
}; 

#endif // _VERTEX_ARRAY_OBJECS_H_
