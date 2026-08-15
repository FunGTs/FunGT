#if !defined(_MESH_GPU_HPP_)
#define _MESH_GPU_HPP_
#include <memory>
#include "../GraphicsRenderBackend/gpu_buffer.hpp"

class MeshGPU {
public:
    std::unique_ptr<GPUBuffer> m_buffer;
};


void RegisterMeshGPUCallbacks();

#endif // _MESH_GPU_HPP_
