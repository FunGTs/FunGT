#if !defined(_RENDER_WINDOW_H_)
#define _RENDER_WINDOW_H_

#include "imgui_window.hpp"
#include "Camera/camera.hpp"
#include "SceneManager/scene_manager.hpp"
#include "PBR/Space/space.hpp"
#include "PBR/PBRCamera/pbr_camera.hpp"
#include "PBR/Render/include/compute_backends.hpp"
#include <memory>
#include <chrono>

/**
 * Render Window - PBR Path Tracing Controls
 *
 * Provides UI for:
 * - Compute backend selection (CUDA/CPU)
 * - Resolution settings
 * - Sample count
 * - Render triggering
 */
class RenderWindow : public ImGuiWindow {
private:
    std::shared_ptr<SceneManager> m_sceneManager;
    Camera* m_camera;

    // Render settings
    int m_selectedBackend = 0;  // 0 = CUDA, 1 = SYCL
    int m_samples = 128;
    int m_renderWidth = 1920;
    int m_renderHeight = 1080;
    bool m_useViewportSize = false;
    bool m_isRendering = false;

    // Resolution presets
    const char* m_resolutionPresets[5] = {
        "Custom",
        "HD (1920x1080)",
        "2K (2560x1440)",
        "4K (3840x2160)",
        "Viewport Size"
    };
    int m_selectedPreset = 1;  // Default to HD

    // Viewport dimensions (set externally)
    int m_viewportWidth = 1920;
    int m_viewportHeight = 1080;

public:
    RenderWindow(std::shared_ptr<SceneManager> sceneManager, Camera* camera)
        : m_sceneManager(sceneManager)
        , m_camera(camera)
    {
    }

    // Set viewport dimensions from outside
    void setViewportSize(int width, int height) {
        m_viewportWidth = width;
        m_viewportHeight = height;
    }

    void onImGuiRender() override {
        ImGui::SetNextWindowSize(ImVec2(350, 500), ImGuiCond_FirstUseEver);
        ImGui::Begin("Render");

        if (!m_sceneManager || !m_camera) {
            ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Waiting for scene...");
            ImGui::End();
            return;
        }

        // ====================================================================
        // COMPUTE BACKEND SELECTION
        // ====================================================================
        ImGui::SeparatorText("Compute Backend");

        const char* backends[] = { "CUDA (NVIDIA)", "SYCL" };
        if (ImGui::Combo("Backend", &m_selectedBackend, backends, 2)) {
            // Backend changed
        }

        ImGui::Spacing();
        ImGui::TextWrapped("CUDA: Fast (requires NVIDIA GPU)");
        ImGui::TextWrapped("SYCL: Works on Intel GPU ");

        ImGui::Spacing();
        ImGui::Separator();

        // ====================================================================
        // RESOLUTION SETTINGS
        // ====================================================================
        ImGui::SeparatorText("Resolution");

        if (ImGui::Combo("Preset", &m_selectedPreset, m_resolutionPresets, 5)) {
            // Apply preset
            switch (m_selectedPreset) {
            case 1: m_renderWidth = 1920; m_renderHeight = 1080; break;  // HD
            case 2: m_renderWidth = 2560; m_renderHeight = 1440; break;  // 2K
            case 3: m_renderWidth = 3840; m_renderHeight = 2160; break;  // 4K
            case 4:
                m_renderWidth = m_viewportWidth;
                m_renderHeight = m_viewportHeight;
                m_useViewportSize = true;
                break;
            default: break;  // Custom
            }
        }

        ImGui::Spacing();

        // Manual resolution input
        if (m_selectedPreset == 0 || m_selectedPreset == 4) {
            ImGui::BeginDisabled(m_useViewportSize);
            ImGui::InputInt("Width", &m_renderWidth);
            ImGui::InputInt("Height", &m_renderHeight);
            ImGui::EndDisabled();

            if (m_useViewportSize) {
                ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f),
                    "Using viewport: %dx%d", m_viewportWidth, m_viewportHeight);
            }
        }
        else {
            ImGui::Text("Resolution: %dx%d", m_renderWidth, m_renderHeight);
        }

        // Aspect ratio info
        float aspect = (float)m_renderWidth / m_renderHeight;
        ImGui::Text("Aspect Ratio: %.3f", aspect);

        ImGui::Spacing();
        ImGui::Separator();

        // ====================================================================
        // QUALITY SETTINGS
        // ====================================================================
        ImGui::SeparatorText("Quality");

        ImGui::SliderInt("Samples", &m_samples, 1, 512);
        ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f),
            "Higher = better quality, slower");

        ImGui::Spacing();
        ImGui::Separator();

        // ====================================================================
        // RENDER INFO
        // ====================================================================
        ImGui::SeparatorText("Scene Info");

        const auto& objects = m_sceneManager->getRenderable();
        ImGui::Text("Objects in scene: %zu", objects.size());

        glm::vec3 camPos = m_camera->getPosition();
        ImGui::Text("Camera: (%.1f, %.1f, %.1f)", camPos.x, camPos.y, camPos.z);

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        // ====================================================================
        // RENDER BUTTON
        // ====================================================================
        ImGui::BeginDisabled(m_isRendering);

        if (ImGui::Button("Render Image", ImVec2(-1, 40))) {
            triggerRender();
        }

        ImGui::EndDisabled();

        if (m_isRendering) {
            ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.0f, 1.0f), "Rendering...");
            ImGui::TextWrapped("Check console for progress");
        }

        ImGui::Spacing();
        ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f),
            "Output: CUDA_output.png or CPU_output.png");

        ImGui::End();
    }

private:
    void triggerRender() {
        std::cout << "\n========== STARTING PBR RENDER ==========" << std::endl;

        m_isRendering = true;

        try {
            // ================================================================
            // 1. SET COMPUTE BACKEND
            // ================================================================
            if (m_selectedBackend == 0) {
                ComputeRender::SetBackend(Compute::Backend::CUDA);
            }
            else {
                ComputeRender::SetBackend(Compute::Backend::SYCL);
            }
            std::cout << "Backend: " << ComputeRender::GetBackendName() << std::endl;

            // ================================================================
            // 2. SYNC CAMERA FROM VIEWPORT
            // ================================================================
            glm::vec3 pos = m_camera->getPosition();
            glm::vec3 front = m_camera->getFront();
            glm::vec3 up = m_camera->getUp();
            float fov = m_camera->getFOV();

            // Calculate look-at point
            glm::vec3 lookAt = pos + front;

            // Convert to PBR format
            fungt::Vec3 pbrPos(pos.x, pos.y, pos.z);
            fungt::Vec3 pbrLookAt(lookAt.x, lookAt.y, lookAt.z);
            fungt::Vec3 pbrUp(up.x, up.y, up.z);

            // Get resolution (use viewport size if enabled)
            int width = m_useViewportSize ? m_viewportWidth : m_renderWidth;
            int height = m_useViewportSize ? m_viewportHeight : m_renderHeight;
            float aspect = (float)width / height;

            std::cout << "Resolution: " << width << "x" << height << std::endl;
            std::cout << "Samples: " << m_samples << std::endl;
            std::cout << "Camera Position: (" << pos.x << ", " << pos.y << ", " << pos.z << ")" << std::endl;

            // Create PBR camera
            PBRCamera pbrCam(pbrPos, pbrLookAt, pbrUp, fov, aspect);

            // ================================================================
            // 3. SETUP SPACE
            // ================================================================
            Space space(pbrCam);
            space.InitComputeRenderBackend();
            // ================================================================
            // 4. LOAD ALL MODELS FROM SCENE
            // ================================================================
            const auto& objects = m_sceneManager->getRenderable();

            int modelCount = 0;
            for (auto& obj : objects) {
                // Try to cast to SimpleModel
                auto simpleModel = std::dynamic_pointer_cast<SimpleModel>(obj);
                if (simpleModel) {
                    std::cout << "Loading model " << (modelCount + 1) << " to PBR scene..." << std::endl;
                    space.LoadModelToRender(*simpleModel);
                    modelCount++;
                }
            }

            if (modelCount == 0) {
                std::cerr << " No models found in scene!" << std::endl;
                m_isRendering = false;
                return;
            }

            std::cout << "Loaded " << modelCount << " models" << std::endl;

            // ================================================================
            // 5. BUILD BVH
            // ================================================================
            std::cout << "Building BVH..." << std::endl;
            space.BuildBVH();

            // ================================================================
            // 6. RENDER
            // ================================================================
            space.setSamples(m_samples);

            auto renderStart = std::chrono::high_resolution_clock::now();
            std::cout << "Rendering..." << std::endl;

            auto framebuffer = space.Render(width, height);

            auto renderEnd = std::chrono::high_resolution_clock::now();
            auto renderTime = std::chrono::duration_cast<std::chrono::seconds>(renderEnd - renderStart).count();

            // ================================================================
            // 7. SAVE OUTPUT
            // ================================================================
            Space::SaveFrameBufferAsPNG(framebuffer, width, height);

            std::cout << "\n========== RENDER COMPLETE ==========" << std::endl;
            std::cout << "Time: " << renderTime << " seconds" << std::endl;
            std::cout << "Output: " << ComputeRender::GetBackendName() << "_output.png" << std::endl;
            std::cout << "====================================\n" << std::endl;

        }
        catch (const std::exception& e) {
            std::cerr << "Render failed: " << e.what() << std::endl;
        }

        m_isRendering = false;
    }
};

#endif // _RENDER_WINDOW_H_