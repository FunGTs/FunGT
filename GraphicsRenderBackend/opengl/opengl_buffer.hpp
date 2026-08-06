#if !defined(_OPENGL_BUFFER_HPP_)
#define _OPENGL_BUFFER_HPP_
#include "../gpu_buffer.hpp"
#include "../vertex_format.hpp"
#include "../../VertexGL/vertexArrayObjects.hpp"
#include "../../VertexGL/vertexBuffers.hpp"
#include "../../VertexGL/vertexIndices.hpp"

class OpenGLBuffer : public GPUBuffer {
public:
    OpenGLBuffer() = default;
    ~OpenGLBuffer() override;

    void create(BufferType type, const void* data, size_t size) override;
    void bind() override;
    void unbind() override;
    void destroy() override;

    void genVAO()    override;
    void bindVAO()   override;
    void unbindVAO() override;

    void applyFormat(const VertexFormat& fmt) override;
    void drawIndexed(size_t indexCount, DrawMode mode = DrawMode::Triangles) override;
    void drawArrays(size_t vertexCount, DrawMode mode = DrawMode::Triangles) override;
    void drawIndexedInstanced(size_t indexCount, int instanceCount, DrawMode mode = DrawMode::Triangles) override;
    void drawArraysInstanced(size_t vertexCount, int instanceCount, DrawMode mode = DrawMode::Triangles) override;

private:
    BufferType        m_type;
    VertexArrayObject m_vao;
    VertexBuffer      m_vbo;
    VertexIndex       m_ebo;
};

#endif // _OPENGL_BUFFER_HPP_
