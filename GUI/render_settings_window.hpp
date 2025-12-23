#ifndef _RENDER_SETTINGS_WINDOW_H_
#define _RENDER_SETTINGS_WINDOW_H_

#include "imgui_window.hpp"
#include "InfoDevice/gpu_device_info.hpp"
#include <memory>

class RenderSettingsWindow : public ImGuiWindow {
private:
    std::shared_ptr<GPUDeviceManager> m_gpuManager;
    bool m_isOpen;

    // Render settings
    int m_samples;
    int m_maxBounces;
    int m_resolution[2];
    bool m_enableDenoising;

public:
    RenderSettingsWindow(std::shared_ptr<GPUDeviceManager> gpuManager)
        : m_gpuManager(gpuManager)
        , m_isOpen(false)
        , m_samples(128)
        , m_maxBounces(4)
        , m_enableDenoising(true)
    {
        m_resolution[0] = 1920;
        m_resolution[1] = 1080;
    }

    void open() { m_isOpen = true; }
    void close() { m_isOpen = false; }
    bool isOpen() const { return m_isOpen; }

    void onImGuiRender() override {
        if (!m_isOpen) return;

        ImGui::SetNextWindowSize(ImVec2(600, 500), ImGuiCond_FirstUseEver);

        if (!ImGui::Begin("Render Settings", &m_isOpen, ImGuiWindowFlags_NoCollapse)) {
            ImGui::End();
            return;
        }

        // === GPU DEVICES SECTION ===
        if (ImGui::CollapsingHeader("Render Devices", ImGuiTreeNodeFlags_DefaultOpen)) {
            const auto& devices = m_gpuManager->getDevices();

            if (devices.empty()) {
                ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), "No GPU devices detected!");
                ImGui::Text("Make sure GPU backends are properly configured.");
            }
            else {
                // Group by backend
                renderBackendSection("CUDA Devices", fungt::GPUBackend::CUDA);
                ImGui::Spacing();
                renderBackendSection("SYCL Devices", fungt::GPUBackend::SYCL);
                ImGui::Spacing();
                renderBackendSection("OpenGL Fallback", fungt::GPUBackend::OPENGL);
            }
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        // === RENDER QUALITY SECTION ===
        if (ImGui::CollapsingHeader("Render Quality", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::Text("Path Tracing Settings");
            ImGui::Spacing();

            ImGui::SliderInt("Samples Per Pixel", &m_samples, 1, 4096);
            if (ImGui::IsItemHovered()) {
                ImGui::SetTooltip("Higher samples = better quality but slower render");
            }

            ImGui::SliderInt("Max Bounces", &m_maxBounces, 1, 32);
            if (ImGui::IsItemHovered()) {
                ImGui::SetTooltip("Maximum light bounces (higher = more realistic)");
            }

            ImGui::Spacing();
            ImGui::Checkbox("Enable Denoising", &m_enableDenoising);
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        // === RESOLUTION SECTION ===
        if (ImGui::CollapsingHeader("Output Resolution")) {
            ImGui::InputInt2("Resolution", m_resolution);

            if (ImGui::Button("720p")) {
                m_resolution[0] = 1280;
                m_resolution[1] = 720;
            }
            ImGui::SameLine();
            if (ImGui::Button("1080p")) {
                m_resolution[0] = 1920;
                m_resolution[1] = 1080;
            }
            ImGui::SameLine();
            if (ImGui::Button("4K")) {
                m_resolution[0] = 3840;
                m_resolution[1] = 2160;
            }
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        // === ACTION BUTTONS ===
        ImGui::Spacing();
        if (ImGui::Button("Apply Settings", ImVec2(150, 0))) {
            std::cout << "Render settings applied!" << std::endl;
            // TODO: Apply settings to your PBR renderer
        }
        ImGui::SameLine();
        if (ImGui::Button("Close", ImVec2(100, 0))) {
            m_isOpen = false;
        }

        ImGui::End();
    }

    // Getters for render settings
    int getSamples() const { return m_samples; }
    int getMaxBounces() const { return m_maxBounces; }
    bool isdenoisingEnabled() const { return m_enableDenoising; }

private:
    void renderBackendSection(const char* title, fungt::GPUBackend backend) {
        const auto& devices = m_gpuManager->getDevices();
        bool hasDevices = false;

        // Check if we have devices for this backend
        for (const auto& device : devices) {
            if (device.backend == backend) {
                hasDevices = true;
                break;
            }
        }

        if (!hasDevices) return;

        ImGui::Text("%s:", title);
        ImGui::Indent();

        for (size_t i = 0; i < devices.size(); ++i) {
            const auto& device = devices[i];

            if (device.backend != backend) continue;

            bool isActive = device.isActive;

            // Push unique ID for each radio button
            ImGui::PushID(static_cast<int>(i));

            // Create radio button for device selection
            if (ImGui::RadioButton(device.name.c_str(), isActive)) {
                m_gpuManager->setActiveDevice(static_cast<int>(i));
            }

            ImGui::PopID();  // Pop the unique ID

            // Show additional info based on backend
            if (device.backend == fungt::GPUBackend::CUDA && device.memory_bytes > 0) {
                ImGui::SameLine();
                ImGui::TextDisabled("(%s)", device.getMemoryString().c_str());
            }
            else if (device.backend == fungt::GPUBackend::SYCL && device.compute_units > 0) {
                ImGui::SameLine();
                ImGui::TextDisabled("(%d EUs)", device.compute_units);
            }
        }

        ImGui::Unindent();
    }
};

#endif // _RENDER_SETTINGS_WINDOW_H_