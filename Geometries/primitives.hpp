#if !defined(_GEOMETRIES_H_)
#define _GEOMETRIES_H_
#include <vector>
#include <memory>
#include "../include/glmath.hpp"
#include "../Textures/textures.hpp"
#include "../GraphicsRenderBackend/gpu_buffer.hpp"
#include "../GraphicsRenderBackend/vertex_format.hpp"
#include "Shaders/shader.hpp"

struct PrimitiveVertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texcoord;

    FUNGT_VERTEX_FORMAT(PrimitiveVertex, position, normal, texcoord)
};

class Primitive {
private:
    std::vector<PrimitiveVertex> m_vertex;
    std::vector<uint32_t>        m_index;

protected:
    std::unique_ptr<GPUBuffer> m_buffer;

public:
    Texture texture;

    Primitive();
    virtual ~Primitive();

    void set(const PrimitiveVertex* vertices, unsigned numOfvert, const uint32_t* indices, unsigned numOfindices);
    void set(const PrimitiveVertex* vertices, unsigned numOfvert);

    PrimitiveVertex*              getVertices();
    uint32_t*                     getIndices();
    unsigned                      getNumOfVertices();
    unsigned                      getNumOfIndices();
    long unsigned                 sizeOfVertices();
    long unsigned                 sizeOfIndices();

    const std::vector<PrimitiveVertex>& getVertices() const;
    const std::vector<uint32_t>&        getIndices()  const;

    virtual void setData() = 0;

    void setTexture(const std::string& pathToTexture);
    void InitGraphics();

    virtual void InstancedDraw(Shader& shader, int instanceCount) {}
    virtual void draw() = 0;
};

#endif // _GEOMETRIES_H_

