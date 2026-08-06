#include "viewport.hpp"
#include "opengl/opengl_viewport.hpp"
// #include "vulkan/vulkan_viewport.hpp"

std::unique_ptr<ViewPort> ViewPort::create()
{
    switch (DisplayGraphics::GetBackend()) {
        case Backend::OpenGL: return std::make_unique<OpenGLViewPort>();
        case Backend::Vulkan: throw std::runtime_error("Vulkan viewport not yet implemented");
        case Backend::Metal:  throw std::runtime_error("Metal viewport not yet implemented");
    }
    throw std::runtime_error("Unknown backend");
}
