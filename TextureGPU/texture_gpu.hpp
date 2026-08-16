#if !defined(_TEXTURE_GPU_HPP_)
#define _TEXTURE_GPU_HPP_
#include <memory>
#include "../GraphicsRenderBackend/gpu_texture.hpp"

class TextureGPU {
public:
    std::unique_ptr<GPUTexture> m_gpu;
};

void RegisterTextureGPUCallbacks();

#endif // _TEXTURE_GPU_HPP_
