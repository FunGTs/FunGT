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
        ImVec2 m_viewportSize;
        std::function<void()> m_RenderFunc; // <-- store scene render 

    public:
        ViewPort();

        void onAttach() override;
        void onDetach() override;
        void onUpdate() override;
        void onImGuiRender() override;
        void setRenderFunction(const std::function<void()>& func){
            m_RenderFunc = func; 
        }
        ImVec2 getViewPortSize(); //Returns a Imgui vec
};


#endif // _VIEWPORT_H_
