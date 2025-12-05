#if !defined(_PROPERTIES_WINDOW_H_)
#define _PROPERTIES_WINDOW_H_

#include "imgui_window.hpp"
#include "Camera/camera.hpp"

class PropertiesWindow : public ImGuiWindow {
private:
    Camera* m_camera;

public:
    PropertiesWindow(Camera* camera) : m_camera(camera) {}

    void onImGuiRender() override {
        ImGui::Begin("Properties");

        if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen)) {
            glm::vec3 pos = m_camera->getPosition();
            glm::vec3 front = m_camera->getFront();

            ImGui::Text("Position:");
            ImGui::Text("  X: %.2f  Y: %.2f  Z: %.2f", pos.x, pos.y, pos.z);

            ImGui::Spacing();

            ImGui::Text("Direction:");
            ImGui::Text("  X: %.2f  Y: %.2f  Z: %.2f", front.x, front.y, front.z);

            ImGui::Spacing();

            if (ImGui::Button("Reset Camera")) {
                // TODO: Reset camera to default position
            }
        }

        if (ImGui::CollapsingHeader("Scene Settings")) {
            static float bgColor[3] = { 0.1f, 0.1f, 0.1f };
            ImGui::ColorEdit3("Background", bgColor);

            ImGui::Spacing();
            ImGui::Text("Grid");
            static bool showGrid = false;
            ImGui::Checkbox("Show Grid", &showGrid);
        }

        if (ImGui::CollapsingHeader("Render")) {
            ImGui::Text("Quick render settings:");
            static int samples = 128;
            ImGui::SliderInt("Samples", &samples, 1, 512);

            static int bounces = 4;
            ImGui::SliderInt("Max Bounces", &bounces, 1, 16);

            ImGui::Spacing();

            if (ImGui::Button("Render Image", ImVec2(-1, 0))) {
                // TODO: Trigger PBR render
                std::cout << "Render triggered!" << std::endl;
            }
        }

        ImGui::End();
    }
};

#endif