#if !defined(_VIEWPORT_H_)
#define _VIEWPORT_H_
#include "../Layer/layer.hpp"
#include "../Renders/display_graphics.hpp"
#include "../include/imgui_headers.hpp"
#include <memory>
#include <cmath>
#include <functional>

class ViewPort : public Layer {
protected:
    ImVec2 m_viewportSize = ImVec2(1280, 720);
    std::function<void()> m_RenderFunc;
    std::function<void(int, int, int)> m_PathTraceFunc;
    bool m_pathTraceMode = false;
    int m_currentSample = 0;
    int m_maxPreviewSamples = 32;

public:
    ViewPort() : Layer("ViewPort") {}
    virtual ~ViewPort() = default;

    static std::unique_ptr<ViewPort> create();

    void setRenderFunction(const std::function<void()>& func) {
        m_RenderFunc = func;
    }
    void setPathTraceFunction(const std::function<void(int, int, int)>& func) {
        m_PathTraceFunc = func;
    }
    virtual uint32_t getPathTraceTexture() const { return 0; }

    ImVec2 getViewPortSize() { return m_viewportSize; }

    void enablePathTracing(bool enable) { m_pathTraceMode = enable; }
    bool isPathTracing() const          { return m_pathTraceMode; }
    void setMaxPreviewSamples(int s)    { m_maxPreviewSamples = s; }
    void resetAccumulation()            { m_currentSample = 0; }
};

#endif // _VIEWPORT_H_
