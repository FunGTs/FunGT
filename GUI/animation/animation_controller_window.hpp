#if !defined(_ANIMATION_CONTROL_WINDOW_H_)
#define _ANIMATION_CONTROL_WINDOW_H_

#include "GUI/imgui_window.hpp"
#include "Physics/AnimationCreator/animation_controller.hpp"
#include "SceneManager/scene_manager.hpp"
#include <memory>

class AnimationControlWindow : public ImGuiWindow {
private:
    std::shared_ptr<fungt::AnimationController> m_animController;
    std::shared_ptr<SceneManager> m_sceneManager;

    std::map<std::string, bool> m_bakingEnabled;
    bool m_initialized;

    // MODE CONTROL
    enum Mode { PHYSICS_RECORDING, ANIMATION_PLAYBACK };
    Mode m_currentMode;

public:
    AnimationControlWindow(std::shared_ptr<fungt::AnimationController> animController,
        std::shared_ptr<SceneManager> sceneManager)
        : m_animController(animController)
        , m_sceneManager(sceneManager)
        , m_initialized(false)
        , m_currentMode(PHYSICS_RECORDING)
    {
    }

    Mode getCurrentMode() const { return m_currentMode; }
    bool isRecordingMode() const { return m_currentMode == PHYSICS_RECORDING; }
    bool isPlaybackMode() const { return m_currentMode == ANIMATION_PLAYBACK; }

    void onImGuiRender() override {  // ← YOUR BASE CLASS METHOD!
        ImGui::Begin("Animation Control");

        if (!m_animController) {
            ImGui::Text("No AnimationController!");
            ImGui::End();
            return;
        }

        if (!m_initialized) {
            initializeBakingSettings();
            m_initialized = true;
        }

        // ========== MODE SELECTOR ==========
        ImGui::SeparatorText("Mode");

        if (ImGui::RadioButton("Physics Recording", m_currentMode == PHYSICS_RECORDING)) {
            m_currentMode = PHYSICS_RECORDING;
            std::cout << "Switched to PHYSICS RECORDING mode" << std::endl;
        }
        ImGui::SameLine();
        if (ImGui::RadioButton("Animation Playback", m_currentMode == ANIMATION_PLAYBACK)) {
            m_currentMode = ANIMATION_PLAYBACK;
            std::cout << "Switched to ANIMATION PLAYBACK mode" << std::endl;
        }

        if (m_currentMode == PHYSICS_RECORDING) {
            ImGui::TextColored(ImVec4(1, 0.5, 0, 1), "Mode: Recording physics motion");
        }
        else {
            ImGui::TextColored(ImVec4(0, 1, 0, 1), "Mode: Playing back keyframes");
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        // ========== KEYFRAME RECORDING ==========

        ImGui::SeparatorText("Keyframe Recording");

        if (ImGui::Button("Record Keyframe (K)", ImVec2(-1, 30))) {
            recordManualKeyframes();
        }

        ImGui::Spacing();

        if (ImGui::CollapsingHeader("Physics Auto-Record", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::Indent();

            for (auto& [objID, enabled] : m_bakingEnabled) {
                if (ImGui::Checkbox(objID.c_str(), &enabled)) {
                    m_animController->setObjectBakingEnabled(objID, enabled);
                }

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

            ImGui::Spacing();

            if (ImGui::Button("Clear All Keyframes", ImVec2(-1, 0))) {
                m_animController->getRecorder()->clearAll();
                std::cout << "Cleared all keyframes" << std::endl;
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
            auto model = std::dynamic_pointer_cast<SimpleModel>(obj);
            if (model) {
                std::string id = model->getAnimationID();
                if (!id.empty()) {
                    m_bakingEnabled[id] = false;
                }
                continue;
            }

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
        for (const auto& [objID, bakingEnabled] : m_bakingEnabled) {
            if (!bakingEnabled) {
                m_animController->recordKeyframe(objID);
            }
        }
    }
};

#endif // _ANIMATION_CONTROL_WINDOW_H_