#if !defined(_SCENE_HIERARCHY_WINDOW_H_)
#define _SCENE_HIERARCHY_WINDOW_H_

#include "imgui_window.hpp"
#include "SceneManager/scene_manager.hpp"
#include <memory>

class SceneHierarchyWindow : public ImGuiWindow {
private:
    std::shared_ptr<SceneManager> m_sceneManager;

public:
    SceneHierarchyWindow(std::shared_ptr<SceneManager> sceneManager)
        : m_sceneManager(sceneManager) {
    }

    void onImGuiRender() override {
        ImGui::Begin("Scene");

        ImGui::Text("Objects:");
        ImGui::Separator();

        // List all renderable objects
        const auto& renderables = m_sceneManager->getRenderable();

        if (renderables.empty()) {
            ImGui::TextDisabled("(No objects in scene)");
        }
        else {
            for (size_t i = 0; i < renderables.size(); ++i) {
                // Simple list item (can be made selectable later)
                ImGui::Selectable(("Object " + std::to_string(i+1)).c_str());
            }
        }

        ImGui::Separator();

        if (ImGui::Button("Add Model", ImVec2(-1, 0))) {
            // TODO: Open file dialog
            ImGui::OpenPopup("Load Model");
        }

        // Simple popup for now
        if (ImGui::BeginPopup("Load Model")) {
            ImGui::Text("File browser not implemented yet");
            ImGui::Text("Use main.cpp to add models for now");
            if (ImGui::Button("Close")) {
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }

        ImGui::End();
    }
};

#endif