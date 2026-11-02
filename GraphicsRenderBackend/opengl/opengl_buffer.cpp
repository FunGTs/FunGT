#include "opengl_buffer.hpp"
#include "../vertex_format.hpp"
#include "../../Shaders/shader.hpp"

static uint32_t toGLType(AttrBaseType t) {
    switch (t) {
        case AttrBaseType::Float: return GL_FLOAT;
        case AttrBaseType::Int:   return GL_INT;
        case AttrBaseType::UInt:  return GL_UNSIGNED_INT;
    }
    return GL_FLOAT;
}

static uint32_t toGLType(GraphicsRenderBackendNext::AttrBaseType t) {
    switch (t) {
        case GraphicsRenderBackendNext::AttrBaseType::Float: return GL_FLOAT;
        case GraphicsRenderBackendNext::AttrBaseType::Int:   return GL_INT;
        case GraphicsRenderBackendNext::AttrBaseType::UInt:  return GL_UNSIGNED_INT;
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

void OpenGLBuffer::createVertexStream(
    uint32_t streamIndex,
    const void* data,
    size_t size,
    const GraphicsRenderBackendNext::VertexFormat& format)
{
    if (m_vertexStreams.size() <= streamIndex) {
        m_vertexStreams.resize(streamIndex + 1);
    }

    OpenGLVertexStream& stream = m_vertexStreams[streamIndex];
    if (!stream.m_buffer) {
        stream.m_buffer = std::make_unique<VertexBuffer>();
        stream.m_buffer->genVB();
    }

    stream.m_format = format;
    stream.m_buffer->bind();
    stream.m_buffer->bufferData(data, static_cast<unsigned int>(size));
}

void OpenGLBuffer::createIndexBuffer(const uint32_t* data, size_t count)
{
    if (!m_hasNextIndexBuffer) {
        m_ebo.genVI();
        m_hasNextIndexBuffer = true;
    }

    m_ebo.bind();
    m_ebo.indexData(data, static_cast<unsigned int>(count * sizeof(uint32_t)));
}

void OpenGLBuffer::bindVertexStreams(const Shader& shader)
{
    for (const OpenGLVertexStream& stream : m_vertexStreams) {
        if (!stream.m_buffer) {
            continue;
        }

        stream.m_buffer->bind();

        for (const GraphicsRenderBackendNext::VertexAttribute& attribute :
             stream.m_format.m_attributes) {
            const int location = shader.getVertexAttributeLocation(attribute.m_name);
            if (location < 0) {
                continue;
            }

            glEnableVertexAttribArray(static_cast<GLuint>(location));

            if (attribute.m_isInt) {
                glVertexAttribIPointer(static_cast<GLuint>(location),
                                       static_cast<GLint>(attribute.m_count),
                                       toGLType(attribute.m_baseType),
                                       static_cast<GLsizei>(stream.m_format.m_stride),
                                       reinterpret_cast<const GLvoid*>(attribute.m_offset));
            } else {
                glVertexAttribPointer(static_cast<GLuint>(location),
                                      static_cast<GLint>(attribute.m_count),
                                      toGLType(attribute.m_baseType),
                                      GL_FALSE,
                                      static_cast<GLsizei>(stream.m_format.m_stride),
                                      reinterpret_cast<const GLvoid*>(attribute.m_offset));
            }
        }
    }
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
