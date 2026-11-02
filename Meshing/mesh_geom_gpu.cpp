#include "mesh_geom_gpu.hpp"
#include "Shaders/shader.hpp"

#include <vector>

void (*MeshGeometry::s_gpuBuild)(MeshGeometry&, MeshGeomGPU*&) = nullptr;
void (*MeshGeometry::s_gpuFree)(MeshGeomGPU*) = nullptr;
void (*MeshGeometry::s_gpuDraw)(MeshGeomGPU&, const MeshGeometry&, Shader&) = nullptr;

void BuildMeshGeomGPU(MeshGeometry& meshGeometry, MeshGeomGPU*& cache)
{
    if (!meshGeometry.isValid()) {
        return;
    }

    std::vector<MeshGeomGPUVertex> vertices;
    std::vector<uint32_t> indices;

    vertices.reserve(meshGeometry.cornerCount());

    for (size_t faceIndex = 0; faceIndex < meshGeometry.faceCount(); ++faceIndex) {
        const MeshGeometry::FaceCorners face = meshGeometry.faceCorners(faceIndex);
        if (face.count < 3) {
            continue;
        }

        const uint32_t firstVertex = static_cast<uint32_t>(vertices.size());
        const size_t cornerOffset = meshGeometry.m_faceOffsets[faceIndex];

        for (size_t cornerIndex = 0; cornerIndex < face.count; ++cornerIndex) {
            const size_t sourceCorner = cornerOffset + cornerIndex;
            const uint32_t positionIndex = face.vertices[cornerIndex];

            MeshGeomGPUVertex vertex;
            vertex.vertex_position = meshGeometry.m_positions[positionIndex];
            vertex.vertex_normal = meshGeometry.m_cornerNormals.empty()
                ? glm::vec3(0.0f)
                : meshGeometry.m_cornerNormals[sourceCorner];
            vertex.vertex_texcoord = meshGeometry.m_cornerUVs.empty()
                ? glm::vec2(0.0f)
                : meshGeometry.m_cornerUVs[sourceCorner];
            vertices.push_back(vertex);
        }

        for (uint32_t cornerIndex = 1; cornerIndex + 1 < face.count; ++cornerIndex) {
            indices.push_back(firstVertex);
            indices.push_back(firstVertex + cornerIndex);
            indices.push_back(firstVertex + cornerIndex + 1);
        }
    }

    if (vertices.empty() || indices.empty()) {
        return;
    }

    cache = new MeshGeomGPU();
    cache->m_buffer = GPUBuffer::create();

    cache->m_buffer->genVAO();
    cache->m_buffer->bindVAO();
    cache->m_buffer->createVertexStream(0, vertices.data(),
                                        vertices.size() * sizeof(MeshGeomGPUVertex),
                                        MeshGeomGPUVertex::getNextFormat());
    cache->m_buffer->createIndexBuffer(indices.data(), indices.size());
    cache->m_buffer->unbindVAO();

    meshGeometry.attachGPUCache(cache);
}

void FreeMeshGeomGPU(MeshGeomGPU* cache)
{
    delete cache;
}

void DrawMeshGeomGPU(MeshGeomGPU& cache, const MeshGeometry& meshGeometry, Shader& shader)
{
    size_t indexCount = 0;

    for (size_t faceIndex = 0; faceIndex < meshGeometry.faceCount(); ++faceIndex) {
        const MeshGeometry::FaceCorners face = meshGeometry.faceCorners(faceIndex);
        if (face.count >= 3) {
            indexCount += (face.count - 2) * 3;
        }
    }

    cache.m_buffer->bindVAO();
    cache.m_buffer->bindVertexStreams(shader);
    cache.m_buffer->drawIndexed(indexCount);
    cache.m_buffer->unbindVAO();
}

void RegisterMeshGeomGPUCallbacks()
{
    MeshGeometry::s_gpuBuild = &BuildMeshGeomGPU;
    MeshGeometry::s_gpuFree = &FreeMeshGeomGPU;
    MeshGeometry::s_gpuDraw = &DrawMeshGeomGPU;
}
