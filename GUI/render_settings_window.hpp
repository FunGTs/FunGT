#ifndef _RENDER_SETTINGS_WINDOW_H_
#define _RENDER_SETTINGS_WINDOW_H_

#include "imgui_window.hpp"
#include "InfoDevice/gpu_device_info.hpp"
#include <memory>

class RenderSettingsWindow : public ImGuiWindow {
private:
    std::shared_ptr<GPUDeviceManager> m_gpuManager;
    bool m_isOpen;

public:
    RenderSettingsWindow(std::shared_ptr<GPUDeviceManager> gpuManager)
        : m_gpuManager(gpuManager)
        , m_isOpen(false)
    {
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
                renderBackendSection("OpenCL Devices", fungt::GPUBackend::OPENCL);
                ImGui::Spacing();
                renderBackendSection("OpenGL Fallback", fungt::GPUBackend::OPENGL);
            }
        }

        ImGui::Spacing();

        if (ImGui::Button("Close", ImVec2(100, 0))) {
            m_isOpen = false;
        }

        ImGui::End();
    }

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

            ImGui::PushID(static_cast<int>(i));

            if (ImGui::RadioButton(device.name.c_str(), device.isActive)) {
                m_gpuManager->setActiveDevice(static_cast<int>(i));
            }

            ImGui::SameLine();
            ImGui::TextDisabled("(%s)", device.getBackendName().c_str());

            ImGui::Indent();
            if (ImGui::TreeNode("Show Info")) {
                ImGui::TextDisabled("ID:      %d", device.id);
                ImGui::TextDisabled("Vendor:  %s", device.vendor.c_str());
                ImGui::TextDisabled("Backend: %s", device.getBackendName().c_str());

                if (device.memory_bytes > 0)
                    ImGui::TextDisabled("Memory:  %s", device.getMemoryString().c_str());

                if (device.compute_units > 0)
                    ImGui::TextDisabled("EU:      %d compute units", device.compute_units);

                ImGui::TextDisabled("Status:  %s", device.isActive ? "Active" : "Inactive");

                ImGui::TreePop();
            }
            ImGui::Unindent();
            ImGui::Spacing();

            ImGui::PopID();
        }

        ImGui::Unindent();
    }
};

#endif // _RENDER_SETTINGS_WINDOW_H_
