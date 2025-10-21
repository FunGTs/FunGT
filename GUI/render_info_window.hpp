#if !defined(_RENDER_INFO_WINDOW_H_)
#define _RENDER_INFO_WINDOW_H_
#include "imgui_window.hpp"
#if defined(_WIN32) || defined(_WIN64)
#define OS_NAME "Windows"
#elif defined(__APPLE__) || defined(__MACH__)
#define OS_NAME "macOS"
#elif defined(__linux__)
#define OS_NAME "Linux"
#elif defined(__unix__)
#define OS_NAME "Unix"
#elif defined(__FreeBSD__)
#define OS_NAME "FreeBSD"
#else
#define OS_NAME "Unknown OS"
#endif

class RenderInfoWindow : public ImGuiWindow
{
    std::string glVersion;
    std::string glVendor;
    std::string glRenderer;

public:
    void onImGuiRender() override
    {
        glVersion = reinterpret_cast<const char *>(glGetString(GL_VERSION));
        glVendor = reinterpret_cast<const char *>(glGetString(GL_VENDOR));
        glRenderer = reinterpret_cast<const char *>(glGetString(GL_RENDERER));
        {
            ImGui::Begin("Render Info");
            ImGui::Text("Running on: %s", OS_NAME); // Display OS name
            ImGui::Text("OpenGL Version: %s", glVersion.c_str());
            ImGui::Text("Vendor: %s", glVendor.c_str());
            ImGui::Text("Renderer: %s", glRenderer.c_str());
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();
        }
    }
};

#endif //