#if !defined(_VIEWPORT_H_)
#define _VIEWPORT_H_
#include "../Layer/layer.hpp"
#include "../Renders/display_graphics.hpp"
#include "PBR/Render/include/compute_backends.hpp"
#include "../include/imgui_headers.hpp"
#include <memory>
#include <cmath>
#include <functional>

class ViewPort : public Layer {
protected:
    ImVec2 m_viewportSize = ImVec2(1280, 720);
    std::function<void()> m_RenderFunc;
    std::function<void(int, int, int)> m_PathTraceFunc;
    std::function<void(int, int, int)> m_PathTraceInteropFunc;
    std::function<void(int, int, int)> m_ActivePathTraceFunc;
    std::function<void()> m_PathTraceReleaseInteropFunc;
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
    void setPathTraceInteropFunction(const std::function<void(int, int, int)>& func) {
        m_PathTraceInteropFunc = func;
    }
    void setPathTraceReleaseInteropFunction(const std::function<void()>& func) {
        m_PathTraceReleaseInteropFunc = func;
    }
    virtual uint32_t getPathTraceTexture() const { return 0; }
    virtual uint32_t getPathTracePBO() const { return 0; }
    virtual void copyPBOToTexture(int width, int height) {}

    ImVec2 getViewPortSize() { return m_viewportSize; }

    void enablePathTracing(bool enable) {
        m_pathTraceMode = enable;
        if (enable) {
            m_ActivePathTraceFunc =
                ComputeRender::GetBackend() == Compute::Backend::OPENCL
                    ? m_PathTraceInteropFunc
                    : m_PathTraceFunc;
        }
    }
    bool isPathTracing() const          { return m_pathTraceMode; }
    void setMaxPreviewSamples(int s)    { m_maxPreviewSamples = s; }
    void resetAccumulation()            { m_currentSample = 0; }
};

#endif // _VIEWPORT_H_
