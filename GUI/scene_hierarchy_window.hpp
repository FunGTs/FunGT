#if !defined(_SCENE_HIERARCHY_WINDOW_H_)
#define _SCENE_HIERARCHY_WINDOW_H_

#include "imgui_window.hpp"
#include "CubeMap/cube_map.hpp"
#include "Path_Manager/path_manager.hpp"
#include "SceneManager/scene_manager.hpp"
#include "SimpleModel/simple_model.hpp"
#include <algorithm>
#include <filesystem>
#include <memory>

class SceneHierarchyWindow : public ImGuiWindow {
private:
    std::shared_ptr<SceneManager> m_sceneManager;
    std::shared_ptr<CubeMap> m_environment;
    Camera* m_camera;
    int m_selectedIndex = -1;
    bool m_showEnvironmentBrowser = false;
    std::string m_currentEnvironmentPath;
    std::string m_selectedEnvironmentFile;
    std::string m_environmentLoadError;
    std::vector<std::filesystem::directory_entry> m_environmentEntries;

public:
    SceneHierarchyWindow(std::shared_ptr<SceneManager> sceneManager, Camera* camera)
        : m_sceneManager(sceneManager), m_camera(camera),
          m_currentEnvironmentPath(findProjectRoot()) {
    }

    void onImGuiRender() override {
        ImGui::Begin("Scene");

        if (ImGui::CollapsingHeader("Environment", ImGuiTreeNodeFlags_DefaultOpen)) {
            if (ImGui::Button("Add Environment", ImVec2(-1.0f, 0.0f))) {
                m_showEnvironmentBrowser = true;
                m_selectedEnvironmentFile.clear();
                m_environmentLoadError.clear();
                refreshEnvironmentDirectory();
            }
            ImGui::Separator();
        }

        ImGui::Text("Objects:");
        ImGui::Separator();

        const auto& renderables = m_sceneManager->getRenderable();
        std::vector<Renderable*> visibleRenderables;
        visibleRenderables.reserve(renderables.size());

        for (const auto& renderable : renderables) {
            if (renderable->showInSceneHierarchy()) {
                visibleRenderables.push_back(renderable.get());
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
            auto* model = dynamic_cast<SimpleModel*>(
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

        ImGui::TextDisabled("Use File -> Open to add models");

        ImGui::End();

        if (m_showEnvironmentBrowser) {
            renderEnvironmentBrowser();
        }
    }

private:
    void refreshEnvironmentDirectory() {
        m_environmentEntries.clear();

        try {
            if (!std::filesystem::exists(m_currentEnvironmentPath) ||
                !std::filesystem::is_directory(m_currentEnvironmentPath)) {
                m_currentEnvironmentPath = findProjectRoot();
            }

            for (const auto& entry :
                std::filesystem::directory_iterator(m_currentEnvironmentPath)) {
                if (entry.is_directory()) {
                    m_environmentEntries.push_back(entry);
                    continue;
                }

                if (!entry.is_regular_file()) {
                    continue;
                }

                std::string extension = entry.path().extension().string();
                std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
                if (extension == ".hdr") {
                    m_environmentEntries.push_back(entry);
                }
            }

            std::sort(m_environmentEntries.begin(), m_environmentEntries.end(),
                [](const auto& a, const auto& b) {
                    if (a.is_directory() != b.is_directory()) {
                        return a.is_directory();
                    }
                    return a.path().filename() < b.path().filename();
                });
        }
        catch (const std::exception& error) {
            m_environmentLoadError = error.what();
        }
    }

    void renderEnvironmentBrowser() {
        ImGui::SetNextWindowSize(ImVec2(700, 500), ImGuiCond_FirstUseEver);
        if (!ImGui::Begin("Open Environment", &m_showEnvironmentBrowser)) {
            ImGui::End();
            return;
        }

        ImGui::Text("Path: %s", m_currentEnvironmentPath.c_str());

        if (ImGui::Button("Back")) {
            const std::filesystem::path parent =
                std::filesystem::path(m_currentEnvironmentPath).parent_path();
            if (!parent.empty()) {
                m_currentEnvironmentPath = parent.string();
                m_selectedEnvironmentFile.clear();
                refreshEnvironmentDirectory();
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("Refresh")) {
            refreshEnvironmentDirectory();
        }

        ImGui::Separator();
        ImGui::BeginChild("EnvironmentFileList", ImVec2(0, -80), true);

        for (const auto& entry : m_environmentEntries) {
            const bool directory = entry.is_directory();
            const std::string name = entry.path().filename().string();
            const bool selected = m_selectedEnvironmentFile == entry.path().string();

            if (ImGui::Selectable(name.c_str(), selected,
                ImGuiSelectableFlags_AllowDoubleClick)) {
                if (directory && ImGui::IsMouseDoubleClicked(0)) {
                    m_currentEnvironmentPath = entry.path().string();
                    m_selectedEnvironmentFile.clear();
                    refreshEnvironmentDirectory();
                    break;
                }

                if (!directory) {
                    m_selectedEnvironmentFile = entry.path().string();
                    if (ImGui::IsMouseDoubleClicked(0)) {
                        loadEnvironment();
                        break;
                    }
                }
            }
        }

        ImGui::EndChild();

        if (!m_environmentLoadError.empty()) {
            ImGui::TextColored(ImVec4(1.0f, 0.3f, 0.3f, 1.0f), "%s",
                m_environmentLoadError.c_str());
        }

        ImGui::Text("Selected: %s", m_selectedEnvironmentFile.empty()
            ? "(none)"
            : std::filesystem::path(m_selectedEnvironmentFile).filename().string().c_str());

        if (ImGui::Button("Open", ImVec2(120, 0))) {
            loadEnvironment();
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0))) {
            m_showEnvironmentBrowser = false;
        }

        ImGui::End();
    }

    void loadEnvironment() {
        if (m_selectedEnvironmentFile.empty()) {
            m_environmentLoadError = "No HDR file selected";
            return;
        }

        try {
            if (!m_environment) {
                m_environment = CubeMap::create();
                m_environment->setShaders(
                    getAssetPath("resources/equirect_skybox.vs"),
                    getAssetPath("resources/equirect_skybox.fs"));
                m_sceneManager->addRenderableObj(m_environment);
            }

            m_environment->buildHDR(m_selectedEnvironmentFile);
            m_sceneManager->loadEnvironment(m_selectedEnvironmentFile);

            m_showEnvironmentBrowser = false;
            m_selectedEnvironmentFile.clear();
        }
        catch (const std::exception& error) {
            m_environmentLoadError = std::string("Failed: ") + error.what();
        }
    }
};

#endif
