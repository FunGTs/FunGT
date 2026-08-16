#include "primitive_gpu.hpp"
#include "../Geometries/primitives.hpp"

void BuildPrimitiveGPU(Primitive& prim, PrimitiveGPU*& cache) {
    cache = new PrimitiveGPU();
    cache->m_buffer = GPUBuffer::create();

    cache->m_buffer->genVAO();
    cache->m_buffer->bindVAO();

    cache->m_buffer->create(BufferType::Vertex, prim.getVertices(), prim.sizeOfVertices());

    if (prim.getNumOfIndices() > 0)
        cache->m_buffer->create(BufferType::Index, prim.getIndices(), prim.sizeOfIndices());

    cache->m_buffer->applyFormat(PrimitiveVertex::getFormat());

    cache->m_buffer->unbindVAO();
}

void FreePrimitiveGPU(PrimitiveGPU* cache) {
    delete cache;
}

void DrawPrimitiveGPU(PrimitiveGPU& cache, unsigned indexCount, unsigned vertexCount) {
    cache.m_buffer->bindVAO();
    if (indexCount > 0) {
        cache.m_buffer->drawIndexed(indexCount);
    } else {
        cache.m_buffer->drawArrays(vertexCount);
    }
    cache.m_buffer->unbindVAO();
}

void DrawPrimitiveGPUInstanced(PrimitiveGPU& cache, unsigned indexCount, unsigned vertexCount, int instanceCount) {
    cache.m_buffer->bindVAO();
    if (indexCount > 0) {
        cache.m_buffer->drawIndexedInstanced(indexCount, instanceCount);
    } else {
        cache.m_buffer->drawArraysInstanced(vertexCount, instanceCount);
    }
    cache.m_buffer->unbindVAO();
}

void RegisterPrimitiveGPUCallbacks() {
    Primitive::s_gpuBuild = &BuildPrimitiveGPU;
    Primitive::s_gpuFree = &FreePrimitiveGPU;
    Primitive::s_gpuDraw = &DrawPrimitiveGPU;
    Primitive::s_gpuDrawInstanced = &DrawPrimitiveGPUInstanced;
}
