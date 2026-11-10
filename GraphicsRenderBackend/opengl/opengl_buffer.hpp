#if !defined(_OPENGL_BUFFER_HPP_)
#define _OPENGL_BUFFER_HPP_
#include "../gpu_buffer.hpp"
#include "../vertex_format.hpp"
#include "../../VertexGL/vertexArrayObjects.hpp"
#include "../../VertexGL/vertexBuffers.hpp"
#include "../../VertexGL/vertexIndices.hpp"
#include "../../VertexGL/uniform_buffer_object.hpp"
#include <vector>
struct OpenGLVertexStream {
    std::unique_ptr<VertexBuffer> m_buffer;
    GraphicsRenderBackendNext::VertexFormat m_format;
};

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
    void createVertexStream(uint32_t streamIndex, const void* data, size_t size,
                            const GraphicsRenderBackendNext::VertexFormat& format) override;
    void createIndexBuffer(const uint32_t* data, size_t count) override;
    void bindVertexStreams(const Shader& shader) override;
    void drawIndexed(size_t indexCount, DrawMode mode = DrawMode::Triangles) override;
    void drawIndexedRange(size_t indexCount, size_t indexOffset,
                          DrawMode mode = DrawMode::Triangles) override;
    void drawArrays(size_t vertexCount, DrawMode mode = DrawMode::Triangles) override;
    void drawIndexedInstanced(size_t indexCount, int instanceCount, DrawMode mode = DrawMode::Triangles) override;
    void drawArraysInstanced(size_t vertexCount, int instanceCount, DrawMode mode = DrawMode::Triangles) override;

    void update(const void* data, size_t size, size_t offset = 0) override;
    void bindBase(unsigned int bindingPoint) override;

private:
    BufferType         m_type;
    VertexArrayObject  m_vao;
    VertexBuffer       m_vbo;
    VertexIndex        m_ebo;
    UniformBufferObject m_ubo;
    std::vector<OpenGLVertexStream> m_vertexStreams;
    bool m_hasNextIndexBuffer = false;
};

#endif // _OPENGL_BUFFER_HPP_
