#if !defined(_SSBO_H_)
#define _SSBO_H_
#ifdef __APPLE__
#define GLFW_INCLUDE_GLCOREARB
#include <OpenGL/gl3.h>
#include <OpenGL/gl3ext.h>
#else
#include <GL/glew.h>
#endif

class SSBO{

    unsigned int m_bufferId;
    unsigned int m_bindingPoint;
    unsigned int m_capacity;
public:
    SSBO();
    ~SSBO();
    void bindToBase(unsigned int bindingPoint);
    void create(unsigned int size);
    void setBuffData(const void* data, unsigned int size, unsigned int offset = 0);
    GLuint getId() const { return m_bufferId; }
};

#endif // _SSBO_H_
