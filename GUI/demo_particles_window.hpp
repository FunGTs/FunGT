#if !defined(_PARTICLE_SIM_DEMO_WINDOW_H_)
#define _PARTICLE_SIM_DEMO_WINDOW_H_
#include "imgui_window.hpp"
#include "SceneManager/scene_manager.hpp"
#include <memory>
#include <chrono>
#include <vector>
#include <string>

class ParticleSimulation;

class ParticleSimDemoWindow : public ImGuiWindow {
private:
    std::shared_ptr<SceneManager> m_sceneManager;
    int m_selectedDemoIndex;
    bool m_autoApply;

    std::chrono::high_resolution_clock::time_point m_lastFPSTime;
    int m_frameCount;
    float m_measuredFPS;

    const std::vector<std::string> m_descriptions = {
        "Spiral outward with radial expansion",
        "Gravitational orbit around center",
        "Spinning tornado-like motion",
        "Explosive burst with gravity",
        "Undulating wave patterns",
        "Rising smoke with turbulence"
    };

    const std::vector<int> m_recommendedCounts = {
        10000, 15000, 8000, 5000, 12000, 10000
    };

public:
    ParticleSimDemoWindow(std::shared_ptr<SceneManager> sceneManager)
        : m_sceneManager(sceneManager)
        , m_selectedDemoIndex(4)
        , m_autoApply(false)
        , m_lastFPSTime(std::chrono::high_resolution_clock::now())
        , m_frameCount(0)
        , m_measuredFPS(0.0f)
    {
    }

    void onImGuiRender() override;
};

#endif // _PARTICLE_SIM_DEMO_WINDOW_H_