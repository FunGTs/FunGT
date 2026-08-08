#if !defined(_GRAPHICS_RENDER_DEVICE_HPP_H)
#define _GRAPHICS_RENDER_DEVICE_HPP_H
#include <memory>
#include "../include/prerequisites.hpp"
#include "../Renders/display_graphics.hpp"

enum class DepthFunc { Less, LessEqual, Equal, Greater, Always };
enum class BlendFactor { SrcAlpha, OneMinusSrcAlpha, One, Zero };

class GraphicsRenderDevice {
public:
    virtual ~GraphicsRenderDevice() = default;
    virtual void setWindowHints() = 0;
    virtual void init(GLFWwindow*, int width, int height, const float clearColor[4]) = 0;
    virtual void shutdown() = 0;
    virtual void beginFrame() = 0;
    virtual void endFrame(GLFWwindow*) = 0;

    virtual void setDepthMask(bool write) = 0;
    virtual void setDepthFunc(DepthFunc func) = 0;
    virtual void setBlendFunc(BlendFactor src, BlendFactor dst) = 0;

    static std::unique_ptr<GraphicsRenderDevice> create(Backend backend);
    static GraphicsRenderDevice* Get();
    static void Register(GraphicsRenderDevice* device);

private:
    static GraphicsRenderDevice* s_instance;
};


#endif // _GRAPHICS_RENDER_DEVICE_HPP_H
