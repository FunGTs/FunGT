#if !defined(_MATERIAL_EDITOR_WINDOW_H_)
#define _MATERIAL_EDITOR_WINDOW_H_

#include "imgui_window.hpp"
#include "../SceneManager/scene_manager.hpp"
#include "../SimpleModel/simple_model.hpp"
#include "../Meshing/mesh_renderable.hpp"
#include <memory>

class MaterialEditorWindow : public ImGuiWindow {
private:
    std::shared_ptr<SceneManager> m_sceneManager;
    int m_selectedObjectIndex;
    int m_selectedMeshIndex;
    int m_selectedMeshRenderableIndex;
    int m_selectedMaterialIndex;

public:
    MaterialEditorWindow(std::shared_ptr<SceneManager> sceneManager)
        : m_sceneManager(sceneManager)
        , m_selectedObjectIndex(0)
        , m_selectedMeshIndex(0)
        , m_selectedMeshRenderableIndex(0)
        , m_selectedMaterialIndex(0)
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

        std::vector<MeshRenderable*> meshRenderables;
        for (const auto& object : allObjects) {
            auto* meshRenderable = dynamic_cast<MeshRenderable*>(object.get());
            if (meshRenderable) {
                meshRenderables.push_back(meshRenderable);
            }
        }

        if (!meshRenderables.empty()) {
            if (m_selectedMeshRenderableIndex >= meshRenderables.size()) {
                m_selectedMeshRenderableIndex = 0;
            }

            ImGui::Text("New Mesh Objects");
            ImGui::SameLine();
            if (ImGui::BeginCombo("##MeshRenderableSelect",
                                  ("Mesh " + std::to_string(m_selectedMeshRenderableIndex)).c_str())) {
                for (int i = 0; i < meshRenderables.size(); ++i) {
                    const bool isSelected = m_selectedMeshRenderableIndex == i;
                    if (ImGui::Selectable(("Mesh " + std::to_string(i)).c_str(), isSelected)) {
                        m_selectedMeshRenderableIndex = i;
                        m_selectedMaterialIndex = 0;
                    }
                    if (isSelected) {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }

            std::shared_ptr<MeshGeometry> geometry =
                meshRenderables[m_selectedMeshRenderableIndex]->getMeshGeometry();
            std::vector<Material>& materials = geometry->getMaterials();

            if (m_selectedMaterialIndex >= materials.size()) {
                m_selectedMaterialIndex = 0;
            }

            ImGui::Text("Material Slot");
            ImGui::SameLine();
            if (ImGui::BeginCombo("##MeshMaterialSelect",
                                  std::to_string(m_selectedMaterialIndex).c_str())) {
                for (int i = 0; i < materials.size(); ++i) {
                    const bool isSelected = m_selectedMaterialIndex == i;
                    std::string label = materials[i].m_name.empty()
                        ? "Material " + std::to_string(i)
                        : materials[i].m_name;
                    if (ImGui::Selectable(label.c_str(), isSelected)) {
                        m_selectedMaterialIndex = i;
                    }
                    if (isSelected) {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }

            if (ImGui::Button("Add Material")) {
                materials.push_back(Material::createDefaultMaterial());
                m_selectedMaterialIndex = static_cast<int>(materials.size()) - 1;
            }

            Material& material = materials[m_selectedMaterialIndex];

            ImGui::Text("Material:");
            ImGui::SameLine();
            ImGui::TextColored(ImVec4(0.4f, 0.7f, 1.0f, 1.0f), "%s", material.m_name.c_str());

            ImGui::Text("Base Color");
            ImGui::ColorEdit3("##MeshBaseColor", &material.m_baseColor.x, ImGuiColorEditFlags_Float);

            ImGui::Text("Metallic");
            ImGui::SliderFloat("##MeshMetallic", &material.m_metallic, 0.0f, 1.0f, "%.3f");

            ImGui::Text("Roughness");
            ImGui::SliderFloat("##MeshRoughness", &material.m_roughness, 0.05f, 1.0f, "%.3f");

            ImGui::Text("Reflectance");
            ImGui::SliderFloat("##MeshReflectance", &material.m_reflectance, 0.0f, 1.0f, "%.3f");

            ImGui::Text("Emission");
            ImGui::SliderFloat("##MeshEmission", &material.m_emission, 0.0f, 50.0f, "%.1f");

            if (ImGui::Button("Reset Mesh Material", ImVec2(-1, 0))) {
                material = Material::createDefaultMaterial();
            }

            ImGui::Text("Faces: %zu", geometry->faceCount());
            ImGui::Text("Material Slots: %zu", materials.size());
            ImGui::Separator();
        }

        // Filter to only SimpleModel objects (skip InfiniteGrid, etc.)
        std::vector<SimpleModel*> models;
        for (const auto& obj : allObjects) {
            auto* model = dynamic_cast<SimpleModel*>(obj.get());
            if (model) {
                models.push_back(model);
            }
        }

        if (models.empty()) {
            if (meshRenderables.empty()) {
                ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "No mesh objects in scene");
            }
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
        SimpleModel* selectedModel = models[m_selectedObjectIndex];

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
            // Get current mesh name for combo preview
            std::string currentMeshName = meshes[m_selectedMeshIndex]->m_name.empty()
                ? "Mesh " + std::to_string(m_selectedMeshIndex)
                : meshes[m_selectedMeshIndex]->m_name;
            if (ImGui::BeginCombo("##MeshSelect", currentMeshName.c_str())) {
                for (int i = 0; i < meshes.size(); i++) {
                    bool isSelected = (m_selectedMeshIndex == i);
                    // Use mesh name if available, fallback to index
                    std::string meshLabel = meshes[i]->m_name.empty()
                        ? "Mesh " + std::to_string(i)
                        : meshes[i]->m_name;
                    if (ImGui::Selectable(meshLabel.c_str(), isSelected)) {
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

        ImGui::Text("Base Color");
        ImGui::ColorEdit3("##BaseColor", &mat.m_baseColor.x, ImGuiColorEditFlags_Float);

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        ImGui::Text("Metallic");
        ImGui::SliderFloat("##Metallic", &mat.m_metallic, 0.0f, 1.0f, "%.3f");

        ImGui::Text("Roughness");
        ImGui::SliderFloat("##Roughness", &mat.m_roughness, 0.05f, 1.0f, "%.3f");

        ImGui::Text("Reflectance");
        ImGui::SliderFloat("##Reflectance", &mat.m_reflectance, 0.0f, 1.0f, "%.3f");

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();
        // Emission
        ImGui::Text("Emission");
        ImGui::SliderFloat("##Emission", &mat.m_emission, 0.0f, 50.0f, "%.1f");

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
