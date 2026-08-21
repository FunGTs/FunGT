#include "render_action_window.hpp"
#include "PBR/Space/space.hpp"
#include "SimpleModel/simple_model.hpp"
#include "SimpleGeometry/simple_geometry.hpp"
#include "Vector/vector3.hpp"

void RenderWindow::onImGuiRender() {
    ImGui::SetNextWindowSize(ImVec2(350, 500), ImGuiCond_FirstUseEver);
    ImGui::Begin("Render");

    if (!m_sceneManager || !m_camera) {
        ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Waiting for scene...");
        ImGui::End();
        return;
    }

    // ACTIVE COMPUTE DEVICE
    ImGui::SeparatorText("Compute Backend");

    if (m_gpuManager) {
        const auto& devices = m_gpuManager->getDevices();
        int activeIdx = m_gpuManager->getActiveDeviceIndex();
        if (activeIdx >= 0 && activeIdx < static_cast<int>(devices.size())) {
            const auto& dev = devices[activeIdx];
            ImGui::Text("Device: %s", dev.name.c_str());
            ImGui::Text("Backend: %s", dev.getBackendName().c_str());
            if (dev.memory_bytes > 0)
                ImGui::TextDisabled("Memory: %s", dev.getMemoryString().c_str());
            else if (dev.compute_units > 0)
                ImGui::TextDisabled("Compute Units: %d EUs", dev.compute_units);
        }
        else {
            ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), "No device selected");
        }
        ImGui::TextDisabled("Change device in Render Settings");
    }

    ImGui::Spacing();
    ImGui::Separator();

    // VIEWPORT PREVIEW
    ImGui::SeparatorText("Viewport Preview");

#ifdef FUNGT_USE_OPENCL
    if (ImGui::Checkbox("Use OpenCL Interop", &m_useOpenCLPreview)) {
        if (m_viewportPathTrace && m_viewport) {
            if (m_useOpenCLPreview) {
                ComputeRender::SetBackend(Compute::Backend::OPENCL);
            } else {
                selectComputeBackend();
            }
            m_viewport->enablePathTracing(true);
            m_viewport->resetAccumulation();
        }
    }
#endif

    if (ImGui::Checkbox("Enable RaySpace Preview", &m_viewportPathTrace)) {
        if (m_viewport) {
            if (m_useOpenCLPreview) {
                ComputeRender::SetBackend(Compute::Backend::OPENCL);
            } else {
                selectComputeBackend();
            }
            m_viewport->enablePathTracing(m_viewportPathTrace);
            if (m_viewportPathTrace) {
                m_viewport->resetAccumulation();
            }
        }
    }

    if (m_viewportPathTrace) {
        ImGui::SliderInt("Preview Samples", &m_previewSamples, 1, 128);
        if (m_viewport) {
            m_viewport->setMaxPreviewSamples(m_previewSamples);
        }

        ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f),
            "Progressive rendering in viewport");
        ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f),
            "(resets when camera moves)");
    }

    // RESOLUTION SETTINGS
    ImGui::SeparatorText("Resolution");

    if (ImGui::Combo("Preset", &m_selectedPreset, m_resolutionPresets, 5)) {
        switch (m_selectedPreset) {
        case 1: m_renderWidth = 1920; m_renderHeight = 1080; break;
        case 2: m_renderWidth = 2560; m_renderHeight = 1440; break;
        case 3: m_renderWidth = 3840; m_renderHeight = 2160; break;
        case 4:
            m_renderWidth = m_viewportWidth;
            m_renderHeight = m_viewportHeight;
            m_useViewportSize = true;
            break;
        default: break;
        }
    }

    ImGui::Spacing();

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

    float aspect = (float)m_renderWidth / m_renderHeight;
    ImGui::Text("Aspect Ratio: %.3f", aspect);

    ImGui::Spacing();
    ImGui::Separator();

    // QUALITY SETTINGS
    ImGui::SeparatorText("Quality");

    ImGui::SliderInt("Samples", &m_samples, 1, 512);
    ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f),
        "Higher = better quality, slower");

    ImGui::Spacing();
    ImGui::Separator();

    // SCENE INFO
    ImGui::SeparatorText("Scene Info");

    const auto& objects = m_sceneManager->getRenderable();
    ImGui::Text("Objects in scene: %zu", objects.size());

    glm::vec3 camPos = m_camera->getPosition();
    ImGui::Text("Camera: (%.1f, %.1f, %.1f)", camPos.x, camPos.y, camPos.z);

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    // RENDER BUTTON
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
    if (m_gpuManager) {
        const auto& devices = m_gpuManager->getDevices();
        int activeIdx = m_gpuManager->getActiveDeviceIndex();
        if (activeIdx >= 0 && activeIdx < static_cast<int>(devices.size())) {
            ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f),
                "Output: %s_output.png", devices[activeIdx].getBackendName().c_str());
        }
    }

    ImGui::End();
}

void RenderWindow::triggerRender() {
    std::cout << "\n========== STARTING PBR RENDER ==========" << std::endl;

    m_isRendering = true;

    try {
        selectComputeBackend();
        std::cout << "Backend: " << ComputeRender::GetBackendName() << std::endl;

        // SYNC CAMERA FROM VIEWPORT
        glm::vec3 pos = m_camera->getPosition();
        glm::vec3 front = m_camera->getFront();
        glm::vec3 up = m_camera->getUp();
        float fov = m_camera->getFOV();

        glm::vec3 lookAt = pos + front;

        fungt::Vec3 pbrPos(pos.x, pos.y, pos.z);
        fungt::Vec3 pbrLookAt(lookAt.x, lookAt.y, lookAt.z);
        fungt::Vec3 pbrUp(up.x, up.y, up.z);

        int width = m_useViewportSize ? m_viewportWidth : m_renderWidth;
        int height = m_useViewportSize ? m_viewportHeight : m_renderHeight;
        float aspect = (float)width / height;

        std::cout << "Resolution: " << width << "x" << height << std::endl;
        std::cout << "Samples: " << m_samples << std::endl;
        std::cout << "Camera Position: (" << pos.x << ", " << pos.y << ", " << pos.z << ")" << std::endl;

        PBRCamera pbrCam(pbrPos, pbrLookAt, pbrUp, fov, aspect);

        // SETUP SPACE
        Space space(pbrCam);
        space.InitComputeRenderBackend();
        space.loadLightsFromScene(m_sceneManager->getLights());

        // LOAD ALL MODELS FROM SCENE
        const auto& objects = m_sceneManager->getRenderable();

        int modelCount = 0;
        int geometryCount = 0;
        for (auto& obj : objects) {
            auto simpleModel = std::dynamic_pointer_cast<SimpleModel>(obj);
            if (simpleModel) {
                std::cout << "Loading model " << (modelCount + 1) << " to PBR scene..." << std::endl;
                space.LoadModelToRender(*simpleModel);
                modelCount++;
                continue;
            }
            auto simpleGeometry = std::dynamic_pointer_cast<SimpleGeometry>(obj);
            if (simpleGeometry) {
                std::cout << "Loading geometry " << (geometryCount + 1) << " to PBR scene..." << std::endl;
                space.LoadGeometryToRender(*simpleGeometry);
                geometryCount++;
            }
        }

        if (modelCount == 0 && geometryCount == 0) {
            std::cerr << "No models or geometries found in scene!" << std::endl;
            m_isRendering = false;
            return;
        }

        std::cout << "Loaded " << modelCount << " models" << std::endl;

        // BUILD BVH
        std::cout << "Building BVH..." << std::endl;
        space.BuildBVH();

        // RENDER
        space.setSamples(m_samples);

        auto renderStart = std::chrono::high_resolution_clock::now();
        std::cout << "Rendering..." << std::endl;

        auto framebuffer = space.Render(width, height);

        auto renderEnd = std::chrono::high_resolution_clock::now();
        auto renderTime = std::chrono::duration_cast<std::chrono::seconds>(renderEnd - renderStart).count();

        // SAVE OUTPUT
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

void RenderWindow::selectComputeBackend()
{
    const auto& devices = m_gpuManager->getDevices();
    const int activeIdx = m_gpuManager->getActiveDeviceIndex();
    if (activeIdx < 0 || activeIdx >= static_cast<int>(devices.size())) {
        ComputeRender::SetBackend(Compute::Backend::CPU);
        return;
    }

    const auto& device = devices[activeIdx];
    if (device.backend == fungt::GPUBackend::CUDA) {
        ComputeRender::SetBackend(Compute::Backend::CUDA);
    } else if (device.backend == fungt::GPUBackend::SYCL) {
        const bool isNvidia =
            device.vendor.find("NVIDIA") != std::string::npos ||
            device.vendor.find("nvidia") != std::string::npos;
        ComputeRender::SetBackend(
            isNvidia ? Compute::Backend::SYCL_CUDA : Compute::Backend::SYCL);
    } else if (device.backend == fungt::GPUBackend::OPENCL) {
        ComputeRender::SetBackend(Compute::Backend::OPENCL);
    } else {
        ComputeRender::SetBackend(Compute::Backend::CPU);
    }
}
