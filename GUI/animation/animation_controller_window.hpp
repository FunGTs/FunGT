#if !defined(_ANIMATION_CONTROL_WINDOW_H_)
#define _ANIMATION_CONTROL_WINDOW_H_

#include "GUI/imgui_window.hpp"
#include "Physics/Animation/AnimationController.hpp"
#include "SceneManager/scene_manager.hpp"
#include <memory>

class AnimationControlWindow : public ImGuiWindow {
private:
    std::shared_ptr<fungt::AnimationController> m_animController;
    std::shared_ptr<SceneManager> m_sceneManager;

    // Baking settings per object
    std::map<std::string, bool> m_bakingEnabled;
    bool m_initialized;

public:
    AnimationControlWindow(std::shared_ptr<fungt::AnimationController> animController,
        std::shared_ptr<SceneManager> sceneManager)
        : ImGuiWindow("Animation Control")
        , m_animController(animController)
        , m_sceneManager(sceneManager)
        , m_initialized(false)
    {
    }

    void render() override {
        ImGui::Begin("Animation Control", &m_isOpen);

        if (!m_animController) {
            ImGui::Text("No AnimationController!");
            ImGui::End();
            return;
        }

        // Initialize baking settings once
        if (!m_initialized) {
            initializeBakingSettings();
            m_initialized = true;
        }

        // ========== KEYFRAME RECORDING ==========

        ImGui::SeparatorText("Keyframe Recording");

        // Manual keyframe button
        if (ImGui::Button("Record Keyframe (K)", ImVec2(-1, 30))) {
            recordManualKeyframes();
        }

        ImGui::Spacing();

        // Physics baking settings
        if (ImGui::CollapsingHeader("Physics Auto-Record", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::Indent();

            for (auto& [objID, enabled] : m_bakingEnabled) {
                if (ImGui::Checkbox(objID.c_str(), &enabled)) {
                    m_animController->setObjectBakingEnabled(objID, enabled);
                }

                // Show keyframe count
                ImGui::SameLine(200);
                int count = m_animController->getRecorder()->getKeyframeCount(objID);
                if (enabled) {
                    ImGui::TextColored(ImVec4(0, 1, 0, 1), "%d keyframes", count);
                }
                else {
                    ImGui::TextColored(ImVec4(0.5, 0.5, 0.5, 1), "%d keyframes", count);
                }
            }

            ImGui::Unindent();
        }

        ImGui::Spacing();

        // ========== KEYFRAME MANAGEMENT ==========

        if (ImGui::CollapsingHeader("Manage Keyframes")) {
            ImGui::Indent();

            for (auto& [objID, enabled] : m_bakingEnabled) {
                int count = m_animController->getRecorder()->getKeyframeCount(objID);

                ImGui::Text("%s:", objID.c_str());
                ImGui::SameLine(150);
                ImGui::Text("%d keyframes", count);

                if (count > 0) {
                    ImGui::SameLine();
                    std::string clearLabel = "Clear##" + objID;
                    if (ImGui::SmallButton(clearLabel.c_str())) {
                        m_animController->clearTrack(objID);
                    }
                }
            }

            ImGui::Unindent();
        }

        ImGui::Spacing();

        // ========== ANIMATION SETTINGS ==========

        if (ImGui::CollapsingHeader("Animation Settings")) {
            ImGui::Indent();

            int maxFrame = m_animController->getMaxFrame();
            if (ImGui::InputInt("Max Frame", &maxFrame)) {
                m_animController->setMaxFrame(maxFrame);
            }

            float speed = m_animController->getPlaybackSpeed();
            if (ImGui::SliderFloat("Playback Speed", &speed, 0.1f, 2.0f)) {
                m_animController->setPlaybackSpeed(speed);
            }

            ImGui::Unindent();
        }

        ImGui::End();
    }

private:
    void initializeBakingSettings() {
        if (!m_sceneManager) return;

        const auto& renderables = m_sceneManager->getRenderable();

        for (const auto& obj : renderables) {
            // Check SimpleModel
            auto model = std::dynamic_pointer_cast<SimpleModel>(obj);
            if (model) {
                std::string id = model->getAnimationID();
                if (!id.empty()) {
                    m_bakingEnabled[id] = false;  // Default: manual
                }
                continue;
            }

            // Check SimpleGeometry
            auto geom = std::dynamic_pointer_cast<SimpleGeometry>(obj);
            if (geom) {
                std::string id = geom->getAnimationID();
                if (!id.empty()) {
                    m_bakingEnabled[id] = false;
                }
            }
        }
    }

    void recordManualKeyframes() {
        // Record all objects that are NOT auto-baking
        for (const auto& [objID, bakingEnabled] : m_bakingEnabled) {
            if (!bakingEnabled) {  // Only manual objects
                m_animController->recordKeyframe(objID);
            }
        }
    }
};

#endif // _ANIMATION_CONTROL_WINDOW_H_