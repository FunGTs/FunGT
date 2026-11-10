#if !defined(_MESH_GEOM_GPU_HPP_)
#define _MESH_GEOM_GPU_HPP_

#include <cstddef>
#include <memory>
#include <vector>
#include "GraphicsRenderBackend/gpu_buffer.hpp"
#include "GraphicsRenderBackendNext/vertex_format.hpp"
#include "mesh_geometry.hpp"

struct MeshGeomGPUVertex {
    glm::vec3 vertex_position;
    glm::vec3 normal_position;
    glm::vec2 texture_position;

    FUNGT_NEXT_VERTEX_FORMAT(MeshGeomGPUVertex, vertex_position, normal_position, texture_position)
};

struct MeshGeomGPUSkin {
    glm::uvec4 jointIds;
    glm::vec4 weights;

    FUNGT_NEXT_VERTEX_FORMAT(MeshGeomGPUSkin, jointIds, weights)
};

struct MeshGeomGPUMaterialRange {
    uint32_t m_materialIndex;
    uint32_t m_indexOffset;
    uint32_t m_indexCount;
};

class MeshGeomGPU {
public:
    std::unique_ptr<GPUBuffer> m_buffer;
    std::vector<MeshGeomGPUMaterialRange> m_materialRanges;
};

void RegisterMeshGeomGPUCallbacks();

#endif // _MESH_GEOM_GPU_HPP_
