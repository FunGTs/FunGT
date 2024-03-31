#if !defined(_VERTEX_INDICES_H_)
#define _VERTEX_INDICES_H_
#ifdef __APPLE__
#define GLFW_INCLUDE_GLCOREARB
#include <OpenGL/gl3.h>
#include <OpenGL/gl3ext.h>
#else
#include <GL/glew.h>
#endif
class VertexIndex{

    unsigned int id_rnd;
    unsigned int numId_rnd;

    public:
        VertexIndex();
        VertexIndex(const unsigned int *data, unsigned int totIndices);
        ~VertexIndex();


        void bind();
        void genVI();
        void unbind();
        void indexData(const unsigned int *data, unsigned int totIndices);
        unsigned int getNumIndices() const ; 

};

#endif // _VERTEX_INDICES_H_
