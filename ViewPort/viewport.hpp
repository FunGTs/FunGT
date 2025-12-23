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
    std::shared_ptr<FrameBuffer> m_resizeBuffer;  // ← ADD: Second buffer for resizing
    ImVec2 m_viewportSize = ImVec2(1280, 720);
    std::function<void()> m_RenderFunc;

public:
    ViewPort();
    void onAttach() override;
    void onDetach() override;
    void onUpdate() override;
    void onImGuiRender() override;
    void setRenderFunction(const std::function<void()>& func) {
        m_RenderFunc = func;
    }
    ImVec2 getViewPortSize();
};


#endif // _VIEWPORT_H_
