#if !defined(_VIEWPORT_H_)
#define _VIEWPORT_H_
#include "../Layer/layer.hpp"
#include "../Renders/framebuffer.hpp"
#include "../include/imgui_headers.hpp"
#include <memory>
#include<cmath>
#include <functional>
class ViewPort : public Layer {
private:
    std::shared_ptr<FrameBuffer> m_frameBuffer;
    std::shared_ptr<FrameBuffer> m_resizeBuffer;  //  Note: Second buffer for resizing
    ImVec2 m_viewportSize = ImVec2(1280, 720);
    std::function<void()> m_RenderFunc;
    std::function<void(int, int, int)> m_PathTraceFunc; // New function for path tracing with sample count
    bool m_pathTraceMode = false;           // Check : Is path tracing enabled?
    int m_currentSample = 0;                // Current sample count
    int m_maxPreviewSamples = 5;           // Stop after this many samples
    GLuint m_pathTraceTexture = 0;  
public:
    ViewPort();
    void onAttach() override;
    void onDetach() override;
    void onUpdate() override;
    void onImGuiRender() override;
    void setRenderFunction(const std::function<void()>& func) {
        m_RenderFunc = func;
    }
    void setPathTraceFunction(const std::function<void(int, int, int)>& func) {
        m_PathTraceFunc = func;
    }
    GLuint getPathTraceTexture() const {
        return m_pathTraceTexture;
    }
    ImVec2 getViewPortSize();
    // Path tracing related functions
    void enablePathTracing(bool enable) {
        m_pathTraceMode = enable;
    }

    bool isPathTracing() const {
        return m_pathTraceMode;
    }

    void setMaxPreviewSamples(int samples) {
        m_maxPreviewSamples = samples;
    }

    void resetAccumulation() {
        m_currentSample = 0;
    }
};


#endif // _VIEWPORT_H_
