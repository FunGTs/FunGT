#include "gpu_buffer.hpp"
#include "opengl/opengl_buffer.hpp"
// #include "vulkan/vk_buffer.hpp"  // uncomment when Vulkan backend is added

std::unique_ptr<GPUBuffer> GPUBuffer::create() {
    switch (DisplayGraphics::GetBackend()) {
        case Backend::OpenGL:
            return std::make_unique<OpenGLBuffer>();
        case Backend::Vulkan:
            // return std::make_unique<VKBuffer>();
            throw std::runtime_error("Vulkan backend not yet implemented");
        case Backend::Metal:
            throw std::runtime_error("Metal backend not yet implemented");
    }
    throw std::runtime_error("Unknown backend");
}
