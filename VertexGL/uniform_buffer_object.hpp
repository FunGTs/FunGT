#if !defined(_UNIFORM_BUFFER_OBJECT_H_)
#define _UNIFORM_BUFFER_OBJECT_H_
#ifdef __APPLE__
#define GLFW_INCLUDE_GLCOREARB
#include <OpenGL/gl3.h>
#include <OpenGL/gl3ext.h>
#else
#include <GL/glew.h>
#endif
#include <cstddef>

class UniformBufferObject {

    GLuint m_bufferId;
    std::size_t m_size;
    bool m_isBound;

    public:
        UniformBufferObject();
        ~UniformBufferObject();

        void create(std::size_t size);
        void bind();
        void unbind();
        void bindBase(GLuint bindingPoint);
        void setBufferData(const void* data, std::size_t size, std::size_t offset = 0);
        GLuint getId() const;

};

#endif // _UNIFORM_BUFFER_OBJECT_H_
