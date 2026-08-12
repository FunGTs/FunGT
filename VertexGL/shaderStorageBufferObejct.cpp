#include "shaderStorageBufferObejct.hpp"

SSBO::SSBO()
{
    m_bufferId = 0;
    m_bindingPoint = 0;
    m_capacity = 0;
}

SSBO::~SSBO()
{
    if (m_bufferId) {
        glDeleteBuffers(1, &m_bufferId);
    }
}

void SSBO::bindToBase(unsigned int bindingPoint)
{
    m_bindingPoint = bindingPoint;
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bindingPoint, m_bufferId);
}

void SSBO::create(unsigned int size)
{
    m_capacity = size;
    glGenBuffers(1, &m_bufferId);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_bufferId);
    glBufferData(GL_SHADER_STORAGE_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void SSBO::setBuffData(const void* data, unsigned int size, unsigned int offset)
{
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_bufferId);
    if (size + offset > m_capacity) {
        m_capacity = size + offset;
        glBufferData(GL_SHADER_STORAGE_BUFFER, m_capacity, data, GL_DYNAMIC_DRAW);
    } else {
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, offset, size, data);
    }
}
