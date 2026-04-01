#if !defined(_LIGHT_EDITOR_WINDOW_H_)
#define _LIGHT_EDITOR_WINDOW_H_

#include "imgui_window.hpp"
#include "SceneManager/scene_manager.hpp"
#include "Lights/scene_light.hpp"
#include <memory>

class LightEditorWindow : public ImGuiWindow {
private:
    std::shared_ptr<SceneManager> m_sceneManager;
    int m_selectedIndex = -1;

    const char* lightTypeName(SceneLightType type) {
        switch (type) {
        case SceneLightType::Point: return "Point";
        case SceneLightType::Sun:   return "Sun";
        case SceneLightType::Spot:  return "Spot";
        case SceneLightType::Area:  return "Area";
        }
        return "Unknown";
    }

public:
    LightEditorWindow(std::shared_ptr<SceneManager> sceneManager)
        : m_sceneManager(sceneManager)
    {
    }

    void onImGuiRender() override {
        ImGui::SetNextWindowSize(ImVec2(320, 500), ImGuiCond_FirstUseEver);
        ImGui::Begin("SceneLight Editor");

        if (!m_sceneManager) {
            ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "No scene manager");
            ImGui::End();
            return;
        }

        auto& lights = m_sceneManager->getLights();

        // ====================================================================
        // ADD LIGHT BUTTONS
        // ====================================================================
        if (ImGui::Button("Add Point")) { SceneLight l; l.type = SceneLightType::Point; l.name = "Point Light";  m_sceneManager->addLight(l); m_selectedIndex = (int)lights.size() - 1; }
        ImGui::SameLine();
        if (ImGui::Button("Add Sun")) { SceneLight l; l.type = SceneLightType::Sun;   l.name = "Sun Light";    m_sceneManager->addLight(l); m_selectedIndex = (int)lights.size() - 1; }
        ImGui::SameLine();
        if (ImGui::Button("Add Spot")) { SceneLight l; l.type = SceneLightType::Spot;  l.name = "Spot Light";   m_sceneManager->addLight(l); m_selectedIndex = (int)lights.size() - 1; }
        ImGui::SameLine();
        if (ImGui::Button("Add Area")) { SceneLight l; l.type = SceneLightType::Area;  l.name = "Area Light";   m_sceneManager->addLight(l); m_selectedIndex = (int)lights.size() - 1; }

        ImGui::Separator();

        // ====================================================================
        // LIGHT LIST
        // ====================================================================
        ImGui::Text("Lights (%d)", (int)lights.size());
        ImGui::BeginChild("LightList", ImVec2(0, 120), true);
        for (int i = 0; i < (int)lights.size(); i++) {
            char label[64];
            snprintf(label, sizeof(label), "[%s] %s", lightTypeName(lights[i].type), lights[i].name.c_str());
            if (ImGui::Selectable(label, m_selectedIndex == i)) {
                m_selectedIndex = i;
            }
        }
        ImGui::EndChild();

        // ====================================================================
        // REMOVE
        // ====================================================================
        if (m_selectedIndex >= 0 && m_selectedIndex < (int)lights.size()) {
            if (ImGui::Button("Remove Selected")) {
                lights.erase(lights.begin() + m_selectedIndex);
                m_selectedIndex = -1;
                ImGui::End();
                return;
            }
        }

        ImGui::Separator();

        // ====================================================================
        // PROPERTIES OF SELECTED LIGHT
        // ====================================================================
        if (m_selectedIndex < 0 || m_selectedIndex >= (int)lights.size()) {
            ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "Select a light to edit");
            ImGui::End();
            return;
        }

        SceneLight& light = lights[m_selectedIndex];

        ImGui::Text("Type: %s", lightTypeName(light.type));
        ImGui::Spacing();

        // Name
        char nameBuf[64];
        strncpy(nameBuf, light.name.c_str(), sizeof(nameBuf));
        if (ImGui::InputText("Name", nameBuf, sizeof(nameBuf))) {
            light.name = nameBuf;
        }

        ImGui::Spacing();
        ImGui::Separator();

        // Common properties
        ImGui::Text("Position");
        ImGui::DragFloat3("##Pos", &light.position.x, 0.1f);

        ImGui::Text("Color");
        ImGui::ColorEdit3("##Color", &light.color.x, ImGuiColorEditFlags_Float);

        ImGui::Text("Power");
        ImGui::DragFloat("##Power", &light.power, 0.1f, 0.0f, 10000.f);

        ImGui::Spacing();
        ImGui::Separator();

        // Type specific properties
        switch (light.type)
        {
        case SceneLightType::Point:
            ImGui::Text("Radius");
            ImGui::DragFloat("##Radius", &light.radius, 0.01f, 0.0f, 100.f);
            break;

        case SceneLightType::Sun:
            ImGui::Text("Direction");
            ImGui::DragFloat3("##Dir", &light.direction.x, 0.01f, -1.f, 1.f);
            break;

        case SceneLightType::Spot:
            ImGui::Text("Direction");
            ImGui::DragFloat3("##SpotDir", &light.direction.x, 0.01f, -1.f, 1.f);
            ImGui::Text("Inner Angle");
            ImGui::DragFloat("##Inner", &light.innerAngle, 0.5f, 0.f, light.outerAngle);
            ImGui::Text("Outer Angle");
            ImGui::DragFloat("##Outer", &light.outerAngle, 0.5f, light.innerAngle, 90.f);
            break;

        case SceneLightType::Area:
            ImGui::Text("Normal");
            ImGui::DragFloat3("##Normal", &light.normal.x, 0.01f, -1.f, 1.f);
            ImGui::Text("Size");
            ImGui::DragFloat2("##Size", &light.size.x, 0.1f, 0.1f, 100.f);
            break;
        }

        ImGui::End();
    }
};

#endif // _LIGHT_EDITOR_WINDOW_H_