#include "opengl_buffer.hpp"
#include "../vertex_format.hpp"

static uint32_t toGLType(AttrBaseType t) {
    switch (t) {
        case AttrBaseType::Float: return GL_FLOAT;
        case AttrBaseType::Int:   return GL_INT;
        case AttrBaseType::UInt:  return GL_UNSIGNED_INT;
    }
    return GL_FLOAT;
}

static void applyVertexFormat(const VertexFormat& fmt) {
    for (const auto& attr : fmt.attributes) {
        glEnableVertexAttribArray(attr.location);
        if (attr.isInt)
            glVertexAttribIPointer(attr.location, static_cast<GLint>(attr.count),
                                   toGLType(attr.baseType), static_cast<GLsizei>(fmt.stride),
                                   reinterpret_cast<const GLvoid*>(attr.offset));
        else
            glVertexAttribPointer(attr.location, static_cast<GLint>(attr.count),
                                  toGLType(attr.baseType), GL_FALSE, static_cast<GLsizei>(fmt.stride),
                                  reinterpret_cast<const GLvoid*>(attr.offset));
    }
}

OpenGLBuffer::~OpenGLBuffer() {
    destroy();
}

void OpenGLBuffer::create(BufferType type, const void* data, size_t size) {
    m_type = type;
    switch (type) {
        case BufferType::Vertex:
            m_vbo.genVB();
            m_vbo.bind();
            m_vbo.bufferData(data, static_cast<unsigned int>(size));
            break;
        case BufferType::Index:
            m_ebo.genVI();
            m_ebo.bind();
            m_ebo.indexData(static_cast<const unsigned int*>(data), static_cast<unsigned int>(size));
            break;
        case BufferType::Uniform:
            m_ubo.create(size);
            if (data)
                m_ubo.setBufferData(data, size);
            break;
    }
}

void OpenGLBuffer::bind() {
    switch (m_type) {
        case BufferType::Vertex:  m_vbo.bind(); break;
        case BufferType::Index:   m_ebo.bind(); break;
        case BufferType::Uniform: m_ubo.bind(); break;
    }
}

void OpenGLBuffer::unbind() {
    switch (m_type) {
        case BufferType::Vertex:  m_vbo.unbind(); break;
        case BufferType::Index:   m_ebo.unbind(); break;
        case BufferType::Uniform: m_ubo.unbind(); break;
    }
}

void OpenGLBuffer::destroy() {
}

void OpenGLBuffer::update(const void* data, size_t size, size_t offset) {
    m_ubo.setBufferData(data, size, offset);
}

void OpenGLBuffer::bindBase(unsigned int bindingPoint) {
    m_ubo.bindBase(static_cast<GLuint>(bindingPoint));
}

void OpenGLBuffer::genVAO()    { m_vao.genVAO(); }
void OpenGLBuffer::bindVAO()   { m_vao.bind();   }
void OpenGLBuffer::unbindVAO() { m_vao.unbind(); }

void OpenGLBuffer::applyFormat(const VertexFormat& fmt) {
    applyVertexFormat(fmt);
}

static GLenum toGLMode(DrawMode m) {
    switch (m) {
        case DrawMode::Triangles:     return GL_TRIANGLES;
        case DrawMode::Lines:         return GL_LINES;
        case DrawMode::LineStrip:     return GL_LINE_STRIP;
        case DrawMode::Points:        return GL_POINTS;
        case DrawMode::TriangleStrip: return GL_TRIANGLE_STRIP;
        case DrawMode::TriangleFan:   return GL_TRIANGLE_FAN;
    }
    return GL_TRIANGLES;
}

void OpenGLBuffer::drawIndexed(size_t indexCount, DrawMode mode) {
    glDrawElements(toGLMode(mode), static_cast<GLsizei>(indexCount), GL_UNSIGNED_INT, (void*)0);
}

void OpenGLBuffer::drawArrays(size_t vertexCount, DrawMode mode) {
    glDrawArrays(toGLMode(mode), 0, static_cast<GLsizei>(vertexCount));
}

void OpenGLBuffer::drawIndexedInstanced(size_t indexCount, int instanceCount, DrawMode mode) {
    glDrawElementsInstanced(toGLMode(mode), static_cast<GLsizei>(indexCount), GL_UNSIGNED_INT, (void*)0, instanceCount);
}

void OpenGLBuffer::drawArraysInstanced(size_t vertexCount, int instanceCount, DrawMode mode) {
    glDrawArraysInstanced(toGLMode(mode), 0, static_cast<GLsizei>(vertexCount), instanceCount);
}
