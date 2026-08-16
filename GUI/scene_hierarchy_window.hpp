#if !defined(_SCENE_HIERARCHY_WINDOW_H_)
#define _SCENE_HIERARCHY_WINDOW_H_

#include "imgui_window.hpp"
#include "SceneManager/scene_manager.hpp"
#include <memory>

class SceneHierarchyWindow : public ImGuiWindow {
private:
    std::shared_ptr<SceneManager> m_sceneManager;
    Camera* m_camera;
    int m_selectedIndex = -1;

public:
    SceneHierarchyWindow(std::shared_ptr<SceneManager> sceneManager, Camera* camera)
        : m_sceneManager(sceneManager), m_camera(camera) {
    }

    void onImGuiRender() override {
        ImGui::Begin("Scene");

        ImGui::Text("Objects:");
        ImGui::Separator();

        const auto renderables = m_sceneManager->getRenderable();
        std::vector<std::shared_ptr<Renderable>> visibleRenderables;
        visibleRenderables.reserve(renderables.size());

        for (const auto& renderable : renderables) {
            if (renderable->showInSceneHierarchy()) {
                visibleRenderables.push_back(renderable);
            }
        }

        if (visibleRenderables.empty()) {
            ImGui::TextDisabled("(No objects in scene)");
            m_selectedIndex = -1;
        }
        else {
            for (size_t i = 0; i < visibleRenderables.size(); ++i) {
                const bool selected = m_selectedIndex == static_cast<int>(i);
                const std::string label = "Object " + std::to_string(i + 1);
                if (ImGui::Selectable(label.c_str(), selected)) {
                    m_selectedIndex = static_cast<int>(i);
                }
            }
        }

        ImGui::Separator();

        if (m_selectedIndex >= 0 &&
            m_selectedIndex < static_cast<int>(visibleRenderables.size())) {
            auto model = std::dynamic_pointer_cast<SimpleModel>(
                visibleRenderables[static_cast<size_t>(m_selectedIndex)]);

            if (model) {
                glm::vec3 position = model->getPosition();
                glm::vec3 rotation = model->getRotation();
                glm::vec3 scale = model->getScale();

                ImGui::Text("Transform");
                if (ImGui::DragFloat3("Position", &position.x, 0.1f)) {
                    model->position(position.x, position.y, position.z);
                }
                if (ImGui::Button("Reset Position", ImVec2(-1, 0))) {
                    model->position();
                }

                if (ImGui::SliderFloat3("Rotation", &rotation.x,
                                        -180.0f, 180.0f, "%.1f deg")) {
                    model->rotation(rotation.x, rotation.y, rotation.z);
                }
                if (ImGui::Button("Reset Rotation", ImVec2(-1, 0))) {
                    model->rotation();
                }

                if (ImGui::DragFloat3("Scale", &scale.x, 0.01f, 0.001f)) {
                    model->scale(scale.x, scale.y, scale.z);
                }
                if (ImGui::Button("Reset Scale", ImVec2(-1, 0))) {
                    model->scale();
                }

                if (ImGui::Button("Frame Selected", ImVec2(-1, 0))) {
                    glm::vec3 boundsMin;
                    glm::vec3 boundsMax;
                    const ImVec2 displaySize = ImGui::GetIO().DisplaySize;
                    const float aspectRatio = displaySize.y > 0.0f
                        ? displaySize.x / displaySize.y
                        : 1.0f;

                    if (model->getWorldBounds(boundsMin, boundsMax)) {
                        m_camera->frameBounds(boundsMin, boundsMax, aspectRatio);
                    }
                }

                if (ImGui::Button("Light Selected", ImVec2(-1, 0))) {
                    glm::vec3 boundsMin;
                    glm::vec3 boundsMax;
                    auto& lights = m_sceneManager->getLights();

                    if (!lights.empty() &&
                        model->getWorldBounds(boundsMin, boundsMax)) {
                        const glm::vec3 center =
                            (boundsMin + boundsMax) * 0.5f;
                        const float radius = glm::max(
                            glm::length(boundsMax - boundsMin) * 0.5f,
                            0.001f);
                        lights.front().position = center +
                            glm::vec3(-0.8f, 1.2f, 0.8f) * radius;
                    }
                }

                if (ImGui::Button("Reset All", ImVec2(-1, 0))) {
                    model->position();
                    model->rotation();
                    model->scale();
                }

                ImGui::Separator();
            }
            else {
                ImGui::TextDisabled("Selected object has no editable model transform");
                ImGui::Separator();
            }
        }

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
