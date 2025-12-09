#if !defined(_LIGHT_EDITOR_WINDOW_H_)
#define _LIGHT_EDITOR_WINDOW_H_

#include "imgui_window.hpp"
#include "../SceneManager/scene_manager.hpp"
#include <memory>

/**
 * Light Editor Window - Edit scene lighting in real-time
 * Works with lights stored in SceneManager
 */
class LightEditorWindow : public ImGuiWindow {
private:
    std::shared_ptr<SceneManager> m_sceneManager;

public:
    LightEditorWindow(std::shared_ptr<SceneManager> sceneManager)
        : m_sceneManager(sceneManager)
    {
    }

    void onImGuiRender() override {
        // Set initial size if window hasn't been created yet
        ImGui::SetNextWindowSize(ImVec2(300, 400), ImGuiCond_FirstUseEver);

        ImGui::Begin("Light Editor");

        if (!m_sceneManager) {
            ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "No scene manager");
            ImGui::End();
            return;
        }

        ImGui::Text("Scene Light");
        ImGui::Separator();
        ImGui::Spacing();

        // ====================================================================
        // LIGHT POSITION
        // ====================================================================
        ImGui::Text("Position");
        auto& lightPos = m_sceneManager->getLightPosition();
        if (ImGui::DragFloat3("##LightPos", &lightPos.x, 0.1f, -20.0f, 20.0f)) {
            // Light position changed - will update on next render automatically!
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        // ====================================================================
        // AMBIENT COLOR
        // ====================================================================
        ImGui::Text("Ambient");
        auto& ambient = m_sceneManager->getLightAmbient();
        ImGui::ColorEdit3("##Ambient", &ambient.x, ImGuiColorEditFlags_Float);

        ImGui::Spacing();

        // ====================================================================
        // DIFFUSE COLOR
        // ====================================================================
        ImGui::Text("Diffuse (Main Color)");
        auto& diffuse = m_sceneManager->getLightDiffuse();
        ImGui::ColorEdit3("##Diffuse", &diffuse.x, ImGuiColorEditFlags_Float);

        ImGui::Spacing();

        // ====================================================================
        // SPECULAR COLOR
        // ====================================================================
        ImGui::Text("Specular (Highlights)");
        auto& specular = m_sceneManager->getLightSpecular();
        ImGui::ColorEdit3("##Specular", &specular.x, ImGuiColorEditFlags_Float);

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        // ====================================================================
        // PRESETS
        // ====================================================================
        ImGui::Text("Presets:");

        if (ImGui::Button("Daylight", ImVec2(-1, 0))) {
            lightPos = glm::vec3(5.0f, 10.0f, 5.0f);
            ambient = glm::vec3(0.3f, 0.3f, 0.3f);
            diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
            specular = glm::vec3(1.0f, 1.0f, 1.0f);
        }

        if (ImGui::Button("Sunset", ImVec2(-1, 0))) {
            lightPos = glm::vec3(5.0f, 3.0f, 5.0f);
            ambient = glm::vec3(0.2f, 0.15f, 0.1f);
            diffuse = glm::vec3(1.0f, 0.6f, 0.3f);
            specular = glm::vec3(1.0f, 0.8f, 0.6f);
        }

        if (ImGui::Button("Night", ImVec2(-1, 0))) {
            lightPos = glm::vec3(0.0f, 5.0f, 5.0f);
            ambient = glm::vec3(0.05f, 0.05f, 0.1f);
            diffuse = glm::vec3(0.3f, 0.3f, 0.5f);
            specular = glm::vec3(0.5f, 0.5f, 0.8f);
        }

        if (ImGui::Button("Studio (3-Point)", ImVec2(-1, 0))) {
            lightPos = glm::vec3(5.0f, 5.0f, 5.0f);
            ambient = glm::vec3(0.2f, 0.2f, 0.2f);
            diffuse = glm::vec3(0.8f, 0.8f, 0.8f);
            specular = glm::vec3(1.0f, 1.0f, 1.0f);
        }

        // ====================================================================
        // INFO
        // ====================================================================
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "Tip:");
        ImGui::TextWrapped("Changes apply immediately to all objects in the scene");

        ImGui::End();
    }
};

#endif // _LIGHT_EDITOR_WINDOW_H_