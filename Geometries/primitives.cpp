#include "primitives.hpp"

void (*Primitive::s_gpuBuild)(Primitive&, PrimitiveGPU*&) = nullptr;
void (*Primitive::s_gpuFree)(PrimitiveGPU*) = nullptr;
void (*Primitive::s_gpuDraw)(PrimitiveGPU&, unsigned, unsigned) = nullptr;
void (*Primitive::s_gpuDrawInstanced)(PrimitiveGPU&, unsigned, unsigned, int) = nullptr;

Primitive::Primitive() {}

Primitive::~Primitive() {
    if (s_gpuFree && m_gpuCache) {
        s_gpuFree(m_gpuCache);
    }
}

void Primitive::set(const PrimitiveVertex* vertices, unsigned numOfvert, const uint32_t* indices, unsigned numOfindices) {
    for (size_t i = 0; i < numOfvert;    i++) m_vertex.push_back(vertices[i]);
    for (size_t i = 0; i < numOfindices; i++) m_index.push_back(indices[i]);
}

void Primitive::set(const PrimitiveVertex* vertices, unsigned numOfvert) {
    for (size_t i = 0; i < numOfvert; i++) m_vertex.push_back(vertices[i]);
}

PrimitiveVertex* Primitive::getVertices()  { return m_vertex.data(); }
uint32_t*        Primitive::getIndices()   { return m_index.data();  }
unsigned         Primitive::getNumOfVertices() { return m_vertex.size(); }
unsigned         Primitive::getNumOfIndices()  { return m_index.size();  }
long unsigned    Primitive::sizeOfVertices()   { return sizeof(PrimitiveVertex) * m_vertex.size(); }
long unsigned    Primitive::sizeOfIndices()    { return sizeof(uint32_t) * m_index.size(); }

const std::vector<PrimitiveVertex>& Primitive::getVertices() const { return m_vertex; }
const std::vector<uint32_t>&        Primitive::getIndices()  const { return m_index;  }

void Primitive::setTexture(const std::string& pathToTexture) {
    texture.genTexture(pathToTexture);
}

void Primitive::InitGraphics() {
    if (s_gpuBuild && !m_gpuCache) {
        s_gpuBuild(*this, m_gpuCache);
    }
}
