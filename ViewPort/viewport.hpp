#if !defined(_VIEWPORT_H_)
#define _VIEWPORT_H_
#include "../Layer/layer.hpp"
#include "../Renders/framebuffer.hpp"
#include "../include/imgui_headers.hpp"
#include <memory>
class ViewPort : public Layer {

    private:

        std::shared_ptr<FrameBuffer> m_frameBuffer;

    public:
        ViewPort();

        void onAttach() override {}
        void onDetach() override {}
        void onUpdate() override{}
        void onImGuiRender() override{}



};


#endif // _VIEWPORT_H_
