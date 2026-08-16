#if !defined(_PRIMITIVE_GPU_HPP_)
#define _PRIMITIVE_GPU_HPP_
#include <memory>
#include "../GraphicsRenderBackend/gpu_buffer.hpp"

class PrimitiveGPU {
public:
    std::unique_ptr<GPUBuffer> m_buffer;
};

void RegisterPrimitiveGPUCallbacks();

#endif // _PRIMITIVE_GPU_HPP_
