#if !defined(_GPU_BUFFER_HPP_)
#define _GPU_BUFFER_HPP_
#include <cstddef>
#include <memory>
#include "vertex_format.hpp"
#include "../Renders/display_graphics.hpp"

enum class BufferType {
    Vertex,
    Index,
    Uniform
};

enum class DrawMode {
    Triangles,
    Lines,
    LineStrip,
    Points,
    TriangleStrip,
    TriangleFan
};

class GPUBuffer {
public:
    virtual ~GPUBuffer() = default;
    virtual void create(BufferType type, const void* data, size_t size) = 0;
    virtual void bind() = 0;
    virtual void unbind() = 0;
    virtual void destroy() = 0;

    // VAO — OpenGL only, Vulkan impl leaves these empty
    virtual void genVAO()   {}
    virtual void bindVAO()  {}
    virtual void unbindVAO(){}

    // Vertex layout — Vulkan uses it for pipeline, OpenGL bakes it into the VAO
    virtual void applyFormat(const VertexFormat& fmt) {}

    // Draw — backend translates to gl/vk draw calls
    virtual void drawIndexed(size_t indexCount, DrawMode mode = DrawMode::Triangles) {}
    virtual void drawArrays(size_t vertexCount, DrawMode mode = DrawMode::Triangles) {}
    virtual void drawIndexedInstanced(size_t indexCount, int instanceCount, DrawMode mode = DrawMode::Triangles) {}
    virtual void drawArraysInstanced(size_t vertexCount, int instanceCount, DrawMode mode = DrawMode::Triangles) {}

    static std::unique_ptr<GPUBuffer> create();
};


#endif // _GPU_BUFFER_HPP_
