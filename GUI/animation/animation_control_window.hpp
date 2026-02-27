#if !defined(_ANIMATION_CONTROL_WINDOW_H_)
#define _ANIMATION_CONTROL_WINDOW_H_

#include "GUI/imgui_window.hpp"
#include "Physics/AnimationCreator/animation_controller.hpp"
#include "Physics/AnimationCreator/animation_exporter.hpp"
#include "SceneManager/scene_manager.hpp"
#include <memory>
#include <filesystem>
#include <thread>
#include <atomic>

class AnimationControlWindow : public ImGuiWindow {
private:
    std::thread m_exportThread;
    std::atomic<int> m_exportProgress;
    std::atomic<int> m_exportTotal;
    std::atomic<bool> m_isExporting;
    std::shared_ptr<fungt::AnimationController> m_animController;
    std::shared_ptr<SceneManager> m_sceneManager;
    Camera* m_camera;
    std::map<std::string, bool> m_bakingEnabled;
    bool m_initialized;

    enum Mode { PHYSICS_RECORDING, ANIMATION_PLAYBACK };
    Mode m_currentMode;

    bool m_showExportPopup;
    int m_exportStartFrame;
    int m_exportEndFrame;
    int m_resolutionIdx;
    int m_samplesIdx;
    char m_outputDir[512];
    int m_backendIdx;

public:
    AnimationControlWindow(std::shared_ptr<fungt::AnimationController> animController,
        std::shared_ptr<SceneManager> sceneManager, Camera* camera)
        : m_animController(animController)
        , m_sceneManager(sceneManager)
        , m_initialized(false)
        , m_currentMode(PHYSICS_RECORDING)
        , m_showExportPopup(false)
        , m_exportStartFrame(0)
        , m_exportEndFrame(0)
        , m_resolutionIdx(1)
        , m_samplesIdx(0)
        , m_backendIdx(1)
        , m_camera(camera)
    {
        m_exportProgress = 0;
        m_exportTotal = 0;
        m_isExporting = false;
        std::string defaultPath = findProjectRoot() + "/output/";
        strncpy(m_outputDir, defaultPath.c_str(), sizeof(m_outputDir) - 1);
        m_outputDir[sizeof(m_outputDir) - 1] = '\0';
    }

    Mode getCurrentMode() const { return m_currentMode; }
    bool isRecordingMode() const { return m_currentMode == PHYSICS_RECORDING; }
    bool isPlaybackMode() const { return m_currentMode == ANIMATION_PLAYBACK; }

    void onImGuiRender() override {
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
        }
        ImGui::SameLine();
        if (ImGui::RadioButton("Animation Playback", m_currentMode == ANIMATION_PLAYBACK)) {
            m_currentMode = ANIMATION_PLAYBACK;
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

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        // ========== EXPORT BUTTON ==========
        if (ImGui::Button("Open Export Window", ImVec2(-1, 40))) {
            m_exportStartFrame = 0;
            m_exportEndFrame = m_animController->getLastRecordedFrame();
            m_exportProgress = 0;
            m_showExportPopup = true;
        }

        ImGui::End();

        renderExportPopup();
    }

private:
    void initializeBakingSettings() {
        if (!m_sceneManager) return;
        const auto& renderables = m_sceneManager->getRenderable();
        for (const auto& obj : renderables) {
            auto model = std::dynamic_pointer_cast<SimpleModel>(obj);
            if (model) {
                std::string id = model->getAnimationID();
                if (!id.empty()) m_bakingEnabled[id] = false;
                continue;
            }
            auto geom = std::dynamic_pointer_cast<SimpleGeometry>(obj);
            if (geom) {
                std::string id = geom->getAnimationID();
                if (!id.empty()) m_bakingEnabled[id] = false;
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

    void renderExportPopup() {
        if (!m_showExportPopup) return;

        ImGui::OpenPopup("Export Animation###ExportModal");
        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
        ImGui::SetNextWindowSize(ImVec2(500, 0), ImGuiCond_Appearing);

        if (ImGui::BeginPopupModal("Export Animation###ExportModal", &m_showExportPopup)) {

            if (m_isExporting) {
                ImGui::TextColored(ImVec4(1, 1, 0, 1), "Creating animation video...");
                ImGui::Spacing();
                float progress = (m_exportTotal > 0) ? (float)m_exportProgress / m_exportTotal : 0.0f;
                ImGui::ProgressBar(progress, ImVec2(-1, 30));
                ImGui::TextColored(ImVec4(0, 1, 0, 1), "Frame %d / %d  (%.0f%%)",
                    (int)m_exportProgress, (int)m_exportTotal, progress * 100.0f);
                ImGui::EndPopup();
                return;
            }

            if (m_exportProgress > 0 && !m_isExporting) {
                m_showExportPopup = false;
                m_exportProgress = 0;
                ImGui::EndPopup();
                return;
            }

            ImGui::TextColored(ImVec4(1, 1, 0, 1), "Render with PBR Path Tracer");
            ImGui::Separator();
            ImGui::Spacing();

            const char* backends[] = { "SYCL (Intel)", "CUDA (NVIDIA)" };
            ImGui::Combo("Backend##Backend", &m_backendIdx, backends, 2);
            ImGui::Spacing();

            ImGui::Text("Frame Range:");
            ImGui::InputInt("Start##Start", &m_exportStartFrame);
            ImGui::InputInt("End##End", &m_exportEndFrame);
            int numFrames = m_exportEndFrame - m_exportStartFrame + 1;
            ImGui::TextColored(ImVec4(0.7, 0.7, 0.7, 1), "Total: %d frames", numFrames);
            ImGui::Spacing();

            ImGui::Text("Quality:");
            const char* resolutions[] = { "720p", "1080p", "4K" };
            ImGui::Combo("Resolution##Res", &m_resolutionIdx, resolutions, 3);
            const char* samples[] = { "64 (Fast)", "128 (Good)", "256 (High)", "512 (Very High)", "1024 (Ultra)" };
            ImGui::Combo("Samples##Samp", &m_samplesIdx, samples, 5);
            ImGui::Spacing();

            ImGui::Text("Output:");
            ImGui::InputText("Directory##Dir", m_outputDir, sizeof(m_outputDir));
            ImGui::TextColored(ImVec4(0.5, 0.5, 0.5, 1), " %sframe_XXXX.png", m_outputDir);
            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();

            float estimatedMinutes = numFrames * 3.0f / 60.0f;
            ImGui::TextColored(ImVec4(1, 1, 0, 1), "Est. time: ~%.1f min", estimatedMinutes);
            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();

            if (ImGui::Button("Export", ImVec2(120, 40))) {
                doExport();
            }
            ImGui::SameLine();
            if (ImGui::Button("Cancel", ImVec2(120, 40))) {
                m_showExportPopup = false;
            }

            ImGui::EndPopup();
        }
    }

    void doExport() {
        int width = 1920, height = 1080;
        if (m_resolutionIdx == 0) { width = 1280; height = 720; }
        else if (m_resolutionIdx == 2) { width = 3840; height = 2160; }

        int spp = 64;
        if (m_samplesIdx == 1) spp = 128;
        else if (m_samplesIdx == 2) spp = 256;
        else if (m_samplesIdx == 3) spp = 512;
        else if (m_samplesIdx == 4) spp = 1024;

        try {
            std::filesystem::create_directories(m_outputDir);
        }
        catch (const std::exception& e) {
            std::cerr << "Failed to create directory: " << e.what() << std::endl;
            return;
        }

        Compute::Backend backend = (m_backendIdx == 0) ? Compute::Backend::SYCL : Compute::Backend::CUDA;
        int startFrame = m_exportStartFrame;
        int endFrame = m_exportEndFrame;
        std::string outDir = std::string(m_outputDir);

        m_exportProgress = 0;
        m_exportTotal = endFrame - startFrame + 1;
        m_isExporting = true;

        m_exportThread = std::thread([this, width, height, spp, backend, startFrame, endFrame, outDir]() {
            fungt::AnimationExporter exporter(m_animController, m_sceneManager, m_camera);
            exporter.setBackend(backend);
            exporter.setResolution(width, height);
            exporter.setSamples(spp);
            exporter.setProgressCallback([this](int current, int total) {
                m_exportProgress = current;
                m_exportTotal = total;
                });
            exporter.exportAnimation(startFrame, endFrame, outDir);
            m_isExporting = false;
            });
        m_exportThread.detach();

        std::cout << "\n=== TO CREATE VIDEO, RUN: ===" << std::endl;
        std::cout << "ffmpeg -framerate 30 -i " << outDir << "frame_%04d.png -c:v libx264 -pix_fmt yuv420p output.mp4" << std::endl;
    }
};

#endif // _ANIMATION_CONTROL_WINDOW_H_