#include "mesh_gpu.hpp"
#include "../Mesh/mesh.hpp"

void BuildMeshGPU(Mesh& mesh)
{
    auto* cache = new MeshGPU();

    cache->m_buffer = GPUBuffer::create();
    cache->m_buffer->genVAO();
    cache->m_buffer->bindVAO();

    cache->m_buffer->create(BufferType::Vertex, mesh.m_vertex.data(),
                             mesh.m_vertex.size() * sizeof(funGTVERTEX));
    cache->m_buffer->create(BufferType::Index, mesh.m_index.data(),
                             mesh.m_index.size() * sizeof(unsigned int));

    cache->m_buffer->applyFormat(funGTVERTEX::getFormat());
    cache->m_buffer->unbindVAO();

    mesh.attachGPUCache(cache);
}

void FreeMeshGPU(MeshGPU* cache)
{
    delete cache;
}

void DrawMeshGPU(MeshGPU& cache, size_t indexCount)
{
    cache.m_buffer->bindVAO();
    cache.m_buffer->drawIndexed((indexCount / 3) * 3);
    cache.m_buffer->unbindVAO();
}

void RegisterMeshGPUCallbacks()
{
    Mesh::s_gpuCacheBuild = &BuildMeshGPU;
    Mesh::s_gpuCacheFree  = &FreeMeshGPU;
    Mesh::s_gpuCacheDraw  = &DrawMeshGPU;
}
