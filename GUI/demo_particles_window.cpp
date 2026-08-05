#include "demo_particles_window.hpp"
#include "ParticleSimulation/particle_simulation.hpp"
#include "ParticleSimulation/particle_simulation_sycl_ops.hpp"

void ParticleSimDemoWindow::onImGuiRender() {
    m_frameCount++;
    auto currentTime = std::chrono::high_resolution_clock::now();
    float elapsed = std::chrono::duration<float>(currentTime - m_lastFPSTime).count();

    if (elapsed >= 0.5f) {
        m_measuredFPS = m_frameCount / elapsed;
        m_frameCount = 0;
        m_lastFPSTime = currentTime;
    }

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

    ImGui::TextColored(ImVec4(0.4f, 0.7f, 1.0f, 1.0f), "Active:");
    ImGui::SameLine();
    ImGui::Text("%s", particleSim_getDemoName(particleSim->getCurrentDemo()).c_str());
    ImGui::Separator();

    if (ImGui::BeginCombo("##Demo", particleSim_getDemoName(m_selectedDemoIndex).c_str())) {
        for (int i = 0; i < particleSim_getDemoCount(); i++) {
            bool isSelected = (m_selectedDemoIndex == i);
            if (ImGui::Selectable(particleSim_getDemoName(i).c_str(), isSelected)) {
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

    ImGui::TextWrapped("%s", m_descriptions[m_selectedDemoIndex].c_str());

    if (ImGui::Button("Load Demo", ImVec2(-1, 0))) {
        particleSim->loadDemo(m_selectedDemoIndex);
    }

    ImGui::Checkbox("Auto-load", &m_autoApply);

    if (ImGui::CollapsingHeader("Info")) {
        ImGui::Text("Particles: %zu", particleSim->getParticleCount());
        ImGui::Text("Measured FPS: %.1f", m_measuredFPS);
        ImGui::Text("ImGui FPS: %.1f", ImGui::GetIO().Framerate);
        ImGui::Text("Frame time: %.2f ms", 1000.0f / m_measuredFPS);
        ImGui::Text("Recommended: %d", m_recommendedCounts[m_selectedDemoIndex]);
    }

    ImGui::End();
}