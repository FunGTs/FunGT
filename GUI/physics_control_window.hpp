#if !defined(_PHYSICS_CONTROL_WINDOW_H_)
#define _PHYSICS_CONTROL_WINDOW_H_

#include "imgui_window.hpp"
#include "GUI/physics/simulation_controller_window.hpp"
#include <memory>

class PhysicsControlWindow : public ImGuiWindow {
private:
    std::shared_ptr<fungt::SimulationController> m_controller;

public:
    PhysicsControlWindow(std::shared_ptr<fungt::SimulationController> controller)
        : m_controller(controller)
    {
    }

    void onImGuiRender() override {
        ImGui::Begin("Physics Controls");

        if (!m_controller) {
            ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), "No controller");
            ImGui::End();
            return;
        }

        // Transport Controls Section
        ImGui::Text("Transport Controls");
        ImGui::Separator();
        ImGui::Spacing();

        // Button row with icons
        float buttonWidth = 80.0f;

        // Play button (green when playing)
        bool isPlaying = m_controller->isPlaying();
        if (isPlaying) {
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.18f, 0.55f, 0.18f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.20f, 0.60f, 0.20f, 1.0f));
        }

        if (ImGui::Button("Play", ImVec2(buttonWidth, 0))) {
            m_controller->play();
        }

        if (isPlaying) {
            ImGui::PopStyleColor(2);
        }

        ImGui::SameLine();

        // Pause button (yellow when paused)
        bool isPaused = m_controller->isPaused();
        if (isPaused) {
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.7f, 0.6f, 0.1f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.8f, 0.7f, 0.2f, 1.0f));
        }

        if (ImGui::Button("Pause", ImVec2(buttonWidth, 0))) {
            m_controller->pause();
        }

        if (isPaused) {
            ImGui::PopStyleColor(2);
        }

        ImGui::SameLine();

        // Stop button (red-ish)
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.6f, 0.2f, 0.2f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.7f, 0.3f, 0.3f, 1.0f));

        if (ImGui::Button("Stop", ImVec2(buttonWidth, 0))) {
            m_controller->stop();
        }

        ImGui::PopStyleColor(2);

        ImGui::SameLine();

        // Reset button
        if (ImGui::Button("Reset", ImVec2(buttonWidth, 0))) {
            m_controller->reset();
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        // Status display
        ImGui::Text("Status:");
        ImGui::SameLine();

        const char* stateText = "STOPPED";
        ImVec4 stateColor = ImVec4(0.7f, 0.7f, 0.7f, 1.0f);

        if (m_controller->isPlaying()) {
            stateText = "PLAYING";
            stateColor = ImVec4(0.2f, 0.8f, 0.2f, 1.0f);
        }
        else if (m_controller->isPaused()) {
            stateText = "PAUSED";
            stateColor = ImVec4(0.9f, 0.8f, 0.2f, 1.0f);
        }

        ImGui::TextColored(stateColor, "%s", stateText);

        ImGui::Spacing();

        // Time display
        ImGui::Text("Time: %.2f s", m_controller->getCurrentTime());

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        // Playback speed control
        ImGui::Text("Playback Speed");
        float speed = m_controller->getPlaybackSpeed();
        if (ImGui::SliderFloat("##Speed", &speed, 0.1f, 2.0f, "%.1fx")) {
            m_controller->setPlaybackSpeed(speed);
        }

        // Quick speed buttons
        if (ImGui::Button("0.5x", ImVec2(60, 0))) {
            m_controller->setPlaybackSpeed(0.5f);
        }
        ImGui::SameLine();
        if (ImGui::Button("1.0x", ImVec2(60, 0))) {
            m_controller->setPlaybackSpeed(1.0f);
        }
        ImGui::SameLine();
        if (ImGui::Button("2.0x", ImVec2(60, 0))) {
            m_controller->setPlaybackSpeed(2.0f);
        }

        ImGui::End();
    }
};

#endif // _PHYSICS_CONTROL_WINDOW_H_