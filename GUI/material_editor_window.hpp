#if !defined(_MATERIAL_EDITOR_WINDOW_H_)
#define _MATERIAL_EDITOR_WINDOW_H_

#include "imgui_window.hpp"
#include "../SceneManager/scene_manager.hpp"
#include "../Meshing/mesh_renderable.hpp"
#include <memory>

class MaterialEditorWindow : public ImGuiWindow {
private:
    std::shared_ptr<SceneManager> m_sceneManager;
    int m_selectedMeshRenderableIndex;
    int m_selectedMeshGeometryIndex;
    int m_selectedMaterialIndex;

public:
    MaterialEditorWindow(std::shared_ptr<SceneManager> sceneManager)
        : m_sceneManager(sceneManager)
        , m_selectedMeshRenderableIndex(0)
        , m_selectedMeshGeometryIndex(0)
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
            const char* selectedMeshName =
                meshRenderables[m_selectedMeshRenderableIndex]
                    ->getMeshGeometry()
                    ->m_name
                    .c_str();
            if (ImGui::BeginCombo("##MeshRenderableSelect",
                selectedMeshName)) {
                for (int i = 0; i < meshRenderables.size(); ++i) {
                    const bool isSelected = m_selectedMeshRenderableIndex == i;
                    if (ImGui::Selectable(meshRenderables[i]->getMeshGeometry()->m_name.c_str(), isSelected)) {
                        m_selectedMeshRenderableIndex = i;
                        m_selectedMeshGeometryIndex = 0;
                        m_selectedMaterialIndex = 0;
                    }
                    if (isSelected) {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }

            const auto& geometries =
                meshRenderables[m_selectedMeshRenderableIndex]->getGeomMeshes();

            if (m_selectedMeshGeometryIndex >= geometries.size()) {
                m_selectedMeshGeometryIndex = 0;
            }

            if (geometries.size() > 1) {
                const std::string currentGeometryName =
                    geometries[m_selectedMeshGeometryIndex]->m_name.empty()
                        ? "Mesh " + std::to_string(m_selectedMeshGeometryIndex)
                        : geometries[m_selectedMeshGeometryIndex]->m_name;

                ImGui::Text("Mesh");
                ImGui::SameLine();
                if (ImGui::BeginCombo("##MeshGeometrySelect",
                                      currentGeometryName.c_str())) {
                    for (int i = 0; i < geometries.size(); ++i) {
                        const bool isSelected = m_selectedMeshGeometryIndex == i;
                        const std::string geometryName = geometries[i]->m_name.empty()
                            ? "Mesh " + std::to_string(i)
                            : geometries[i]->m_name;
                        if (ImGui::Selectable(geometryName.c_str(), isSelected)) {
                            m_selectedMeshGeometryIndex = i;
                            m_selectedMaterialIndex = 0;
                        }
                        if (isSelected) {
                            ImGui::SetItemDefaultFocus();
                        }
                    }
                    ImGui::EndCombo();
                }
            }

            std::shared_ptr<MeshGeometry> geometry =
                geometries[m_selectedMeshGeometryIndex];
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

        if (meshRenderables.empty()) {
            ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f),
                               "No mesh objects in scene");
        }

        ImGui::End();
    }
};

#endif // _MATERIAL_EDITOR_WINDOW_H_
