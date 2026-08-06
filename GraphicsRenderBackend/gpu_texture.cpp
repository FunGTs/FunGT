#include "gpu_texture.hpp"
#include "opengl/opengl_texture.hpp"
// #include "vulkan/vk_texture.hpp"  // uncomment when Vulkan backend is added

std::unique_ptr<GPUTexture> GPUTexture::create(TextureType type) {
    switch (DisplayGraphics::GetBackend()) {
        case Backend::OpenGL:
            return std::make_unique<OpenGLTexture>(type);
        case Backend::Vulkan:
            // return std::make_unique<VKTexture>(type);
            throw std::runtime_error("Vulkan backend not yet implemented");
        case Backend::Metal:
            throw std::runtime_error("Metal backend not yet implemented");
    }
    throw std::runtime_error("Unknown backend");
}
