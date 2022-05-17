#if !defined(_VERTEX_INDICES_H_)
#define _VERTEX_INDICES_H_
#ifdef __APPLE__
#define GLFW_INCLUDE_GLCOREARB
#include <OpenGL/gl3.h>
#include <OpenGL/gl3ext.h>
#else
#include <GL/glew.h>
#endif
class VI{

    unsigned int id_rnd;
    unsigned int numId_rnd;

    public:
        VI();
        VI(const unsigned int *data, unsigned int totIndices);
        ~VI();


        void build();
        void genVI(const unsigned int *data, unsigned int totIndices);
        void release();
        unsigned int getNumIndices() const ; 

};

#endif // _VERTEX_INDICES_H_
