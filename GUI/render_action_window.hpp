#if !defined(_RENDER_WINDOW_H_)
#define _RENDER_WINDOW_H_

#include "imgui_window.hpp"
#include "Camera/camera.hpp"
#include "SceneManager/scene_manager.hpp"
#include "PBR/PBRCamera/pbr_camera.hpp"
#include "PBR/Render/include/compute_backends.hpp"
#include "ViewPort/viewport.hpp"
#include "InfoDevice/gpu_device_info.hpp"
#include <memory>
#include <chrono>

class RenderWindow : public ImGuiWindow {
private:
    std::shared_ptr<SceneManager> m_sceneManager;
    std::shared_ptr<GPUDeviceManager> m_gpuManager;
    Camera* m_camera;

    int m_samples = 128;
    int m_renderWidth = 1920;
    int m_renderHeight = 1080;
    bool m_useViewportSize = false;
    bool m_isRendering = false;
    bool m_viewportPathTrace = false;
    int m_previewSamples = 32;
    ViewPort* m_viewport = nullptr;

    const char* m_resolutionPresets[5] = {
        "Custom",
        "HD (1920x1080)",
        "2K (2560x1440)",
        "4K (3840x2160)",
        "Viewport Size"
    };
    int m_selectedPreset = 1;

    int m_viewportWidth = 1920;
    int m_viewportHeight = 1080;

    void triggerRender();

public:
    RenderWindow(std::shared_ptr<SceneManager> sceneManager, Camera* camera, ViewPort* viewport,
        std::shared_ptr<GPUDeviceManager> gpuManager)
        : m_sceneManager(sceneManager)
        , m_gpuManager(gpuManager)
        , m_camera(camera)
        , m_viewport(viewport)
    {
    }

    void setViewportSize(int width, int height) {
        m_viewportWidth = width;
        m_viewportHeight = height;
    }

    void onImGuiRender() override;
};

#endif // _RENDER_WINDOW_H_