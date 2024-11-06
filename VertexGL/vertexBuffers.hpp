#if !defined(_VERTEX_BUFFER_H_)
#define _VERTEX_BUFFER_H_
#ifdef __APPLE__
#define GLFW_INCLUDE_GLCOREARB
#include <OpenGL/gl3.h>
#include <OpenGL/gl3ext.h>
#else
#include <GL/glew.h>
#endif
class VertexBuffer{

    private: 
        unsigned int id_Render; // OpenGL  needs a numeric ID, keeps track of every type of object: texture, vertex, shader, etc. 
    public:
        VertexBuffer();
        VertexBuffer(int numBuffers); 
        VertexBuffer(const void* data, unsigned int size);
        ~VertexBuffer();

        void genVB();
        void bind();
        void unbind();
        void bufferData(const void* data, unsigned int size,GLenum usage = GL_STATIC_DRAW);
        unsigned int getId(); 


};


#endif // _VERTEX_BUFFER_H_
