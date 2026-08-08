#if !defined(_OPENGL_DEVICE_HPP_)
#define _OPENGL_DEVICE_HPP_
#include "../graphics_render_device.hpp"

class OpenGLDevice : public GraphicsRenderDevice {
public:
    void setWindowHints() override;
    void init(GLFWwindow* window, int width, int height, const float clearColor[4]) override;
    void shutdown() override;
    void beginFrame() override;
    void endFrame(GLFWwindow* window) override;

    void setDepthMask(bool write) override;
    void setDepthFunc(DepthFunc func) override;
    void setBlendFunc(BlendFactor src, BlendFactor dst) override;
};


#endif // _OPENGL_DEVICE_HPP_
