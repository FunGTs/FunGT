#if !defined(_IMGUI_WINDOW_H_)
#define _IMGUI_WINDOW_H_
#include "../include/prerequisites.hpp"
#include "../include/imgui_headers.hpp"
class ImGuiWindow
{
public:
    virtual ~ImGuiWindow() = default;
    virtual void onImGuiRender() = 0; // Every window implements this
};

#endif // _IMGUI_WINDOW_H_
