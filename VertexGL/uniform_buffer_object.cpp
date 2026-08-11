#include "uniform_buffer_object.hpp"

UniformBufferObject::UniformBufferObject()
{
    m_bufferId = 0;
    m_size = 0;
    m_isBound = false;
}

UniformBufferObject::~UniformBufferObject()
{
    if (m_bufferId) {
        glDeleteBuffers(1, &m_bufferId);
    }
}

void UniformBufferObject::create(std::size_t size)
{
    m_size = size;
    glGenBuffers(1, &m_bufferId);
    glBindBuffer(GL_UNIFORM_BUFFER, m_bufferId);
    glBufferData(GL_UNIFORM_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void UniformBufferObject::bind()
{
    glBindBuffer(GL_UNIFORM_BUFFER, m_bufferId);
    m_isBound = true;
}

void UniformBufferObject::unbind()
{
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    m_isBound = false;
}

void UniformBufferObject::bindBase(GLuint bindingPoint)
{
    glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, m_bufferId);
}

void UniformBufferObject::setBufferData(const void* data, std::size_t size, std::size_t offset)
{
    glBindBuffer(GL_UNIFORM_BUFFER, m_bufferId);
    glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);
}

GLuint UniformBufferObject::getId() const
{
    return m_bufferId;
}
