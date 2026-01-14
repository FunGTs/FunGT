#if !defined(_MATERIAL_EDITOR_WINDOW_H_)
#define _MATERIAL_EDITOR_WINDOW_H_

#include "imgui_window.hpp"
#include "../SceneManager/scene_manager.hpp"
#include "../SimpleModel/simple_model.hpp"
#include <memory>

class MaterialEditorWindow : public ImGuiWindow {
private:
    std::shared_ptr<SceneManager> m_sceneManager;
    int m_selectedObjectIndex;
    int m_selectedMeshIndex;

public:
    MaterialEditorWindow(std::shared_ptr<SceneManager> sceneManager)
        : m_sceneManager(sceneManager)
        , m_selectedObjectIndex(0)
        , m_selectedMeshIndex(0)
    {
    }

    void onImGuiRender() override {
        ImGui::Begin("Material Editor");

        if (!m_sceneManager) {
            ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "No scene manager");
            ImGui::End();
            return;
        }

        const auto& allObjects = m_sceneManager->getRenderable();

        // Filter to only SimpleModel objects (skip InfiniteGrid, etc.)
        std::vector<std::shared_ptr<SimpleModel>> models;
        for (auto& obj : allObjects) {
            auto model = std::dynamic_pointer_cast<SimpleModel>(obj);
            if (model) {
                models.push_back(model);
            }
        }

        if (models.empty()) {
            ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "No models in scene");
            ImGui::Text("Load a model to edit materials");
            ImGui::End();
            return;
        }

        // Clamp selection index
        if (m_selectedObjectIndex >= models.size()) {
            m_selectedObjectIndex = 0;
        }

        // Object selection dropdown (only show if multiple models)
        if (models.size() > 1) {
            ImGui::Text("Model:");
            ImGui::SameLine();
            if (ImGui::BeginCombo("##ModelSelect",
                ("Model " + std::to_string(m_selectedObjectIndex)).c_str())) {
                for (int i = 0; i < models.size(); i++) {
                    bool isSelected = (m_selectedObjectIndex == i);
                    if (ImGui::Selectable(("Model " + std::to_string(i)).c_str(), isSelected)) {
                        m_selectedObjectIndex = i;
                        m_selectedMeshIndex = 0; // Reset mesh selection
                    }
                    if (isSelected) {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }
            ImGui::Separator();
        }

        // Get the selected model
        auto& selectedModel = models[m_selectedObjectIndex];

        const auto& meshes = selectedModel->getMeshes();
        
        if (meshes.empty()) {
            ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), "Model has no meshes");
            ImGui::End();
            return;
        }

        // Mesh selection (if model has multiple meshes)
        if (meshes.size() > 1) {
            ImGui::Text("Mesh:");
            ImGui::SameLine();
            if (ImGui::BeginCombo("##MeshSelect",
                ("Mesh " + std::to_string(m_selectedMeshIndex)).c_str())) {
                for (int i = 0; i < meshes.size(); i++) {
                    bool isSelected = (m_selectedMeshIndex == i);
                    if (ImGui::Selectable(("Mesh " + std::to_string(i)).c_str(), isSelected)) {
                        m_selectedMeshIndex = i;
                    }
                    if (isSelected) {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }
            ImGui::Separator();
        }

        // Get the selected mesh
        Mesh* mesh = meshes[m_selectedMeshIndex].get();

        if (mesh->m_material.empty()) {
            ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), " No materials in this mesh");
            ImGui::End();
            return;
        }

        // Edit the first material
        Material& mat = mesh->m_material[0];

        // Material name
        ImGui::Text("Material:");
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(0.4f, 0.7f, 1.0f, 1.0f), "%s", mat.m_name.c_str());

        ImGui::Separator();
        ImGui::Spacing();

        // Ambient color
        ImGui::Text("Ambient");
        ImGui::ColorEdit3("##Ambient", &mat.m_ambientLight.x, ImGuiColorEditFlags_Float);

        ImGui::Spacing();

        // Diffuse color
        ImGui::Text("Diffuse");
        ImGui::ColorEdit3("##Diffuse", &mat.m_diffLigth.x, ImGuiColorEditFlags_Float);

        ImGui::Spacing();

        // Specular color
        ImGui::Text("Specular");
        ImGui::ColorEdit3("##Specular", &mat.m_specLight.x, ImGuiColorEditFlags_Float);

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        // Shininess slider
        ImGui::Text("Shininess");
        ImGui::SliderFloat("##Shininess", &mat.m_shininess, 1.0f, 128.0f, "%.1f");

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        // Reset button
        if (ImGui::Button("Reset to Default", ImVec2(-1, 0))) {
            mat = Material::createDefaultMaterial();
        }

        // Info section
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "Info:");
        ImGui::Text("Vertices: %zu", mesh->m_vertex.size());
        ImGui::Text("Indices: %zu", mesh->m_index.size());
        ImGui::Text("Textures: %zu", mesh->m_texture.size());
        ImGui::Text("Materials: %zu", mesh->m_material.size());

        ImGui::End();
    }
};

#endif // _MATERIAL_EDITOR_WINDOW_H_