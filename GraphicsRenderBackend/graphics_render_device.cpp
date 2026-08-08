#include "graphics_render_device.hpp"
#include "opengl/opengl_device.hpp"
// #include "vulkan/vk_device.hpp"

GraphicsRenderDevice* GraphicsRenderDevice::s_instance = nullptr;

GraphicsRenderDevice* GraphicsRenderDevice::Get() { return s_instance; }
void GraphicsRenderDevice::Register(GraphicsRenderDevice* device) { s_instance = device; }

std::unique_ptr<GraphicsRenderDevice> GraphicsRenderDevice::create(Backend backend) {
    switch (backend) {
        case Backend::OpenGL:  return std::make_unique<OpenGLDevice>();
        case Backend::Vulkan:  throw std::runtime_error("Vulkan backend not yet implemented");
        case Backend::Metal:   throw std::runtime_error("Metal backend not yet implemented");
    }
    throw std::runtime_error("Unknown backend");
}
