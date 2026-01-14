#if !defined(_PARTICLE_SIM_DEMO_WINDOW_H_)
#define _PARTICLE_SIM_DEMO_WINDOW_H_

#include "imgui_window.hpp"
#include "SceneManager/scene_manager.hpp"
#include "ParticleSimulation/particle_simulation.hpp"
#include "ParticleSimulation/particle_demos.hpp"
#include <memory>

class ParticleSimDemoWindow : public ImGuiWindow {
private:
    std::shared_ptr<SceneManager> m_sceneManager;
    int m_selectedDemoIndex;
    bool m_autoApply;

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
    {
    }

    void onImGuiRender() override {
        ImGui::Begin("Particle Demos");

        if (!m_sceneManager) {
            ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "No scene manager");
            ImGui::End();
            return;
        }

        const auto& allObjects = m_sceneManager->getRenderable();
        std::shared_ptr<ParticleSimulation> particleSim = nullptr;

        for (auto& obj : allObjects) {
            auto sim = std::dynamic_pointer_cast<ParticleSimulation>(obj);
            if (sim) {
                particleSim = sim;
                break;
            }
        }

        if (!particleSim) {
            ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "No particle simulation");
            ImGui::End();
            return;
        }

        // Current demo indicator (compact)
        ImGui::TextColored(ImVec4(0.4f, 0.7f, 1.0f, 1.0f), "Active:");
        ImGui::SameLine();
        ImGui::Text("%s", fgt::demoNames[particleSim->getCurrentDemo()].c_str());

        ImGui::Separator();

        // Demo selection
        if (ImGui::BeginCombo("##Demo", fgt::demoNames[m_selectedDemoIndex].c_str())) {
            for (int i = 0; i < fgt::demoNames.size(); i++) {
                bool isSelected = (m_selectedDemoIndex == i);
                if (ImGui::Selectable(fgt::demoNames[i].c_str(), isSelected)) {
                    m_selectedDemoIndex = i;
                    if (m_autoApply) {
                        particleSim->loadDemo(m_selectedDemoIndex);
                    }
                }
                if (isSelected) {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }

        // Compact description
        ImGui::TextWrapped("%s", m_descriptions[m_selectedDemoIndex].c_str());

        // Load button
        if (ImGui::Button("Load Demo", ImVec2(-1, 0))) {
            particleSim->loadDemo(m_selectedDemoIndex);
        }

        ImGui::Checkbox("Auto-load", &m_autoApply);

        // Collapsing info section
        if (ImGui::CollapsingHeader("Info")) {
            ImGui::Text("Particles: %zu", particleSim->getParticleCount());
            ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
            ImGui::Text("Recommended: %d", m_recommendedCounts[m_selectedDemoIndex]);
        }

        ImGui::End();
    }
};

#endif // _PARTICLE_SIM_DEMO_WINDOW_H_