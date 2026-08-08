#include "shader.hpp"
#include "opengl/opengl_shader.hpp"
// #include "vulkan/vulkan_shader.hpp"

std::unique_ptr<Shader> Shader::create()
{
    switch (DisplayGraphics::GetBackend()) {
        case Backend::OpenGL: return std::make_unique<OpenGLShader>();
        case Backend::Vulkan: throw std::runtime_error("Vulkan shader not yet implemented");
        case Backend::Metal:  throw std::runtime_error("Metal shader not yet implemented");
    }
    throw std::runtime_error("Unknown backend");
}
