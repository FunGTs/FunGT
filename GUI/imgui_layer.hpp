#if !defined(_IMGUI_LAYER_H_)
#define _IMGUI_LAYER_H_
#include "imgui_window.hpp"
#include "Layer/layer.hpp"
#include "InfoDevice/gpu_device_info.hpp"
#include "Path_Manager/path_manager.hpp"
#include "render_settings_window.hpp"
#include "SceneManager/scene_manager.hpp"
#include "SimpleModel/simple_model.hpp"
#include "IconsFontAwesome5.h"
#include <memory>
#include <filesystem>
#include <algorithm>

class ImGuiLayer : public Layer {

private:
    GLFWwindow* m_window;
    int m_width, m_height;
    std::vector<std::unique_ptr<ImGuiWindow>> m_windows;
    // GPU and render settings management
    std::shared_ptr<GPUDeviceManager> m_gpuManager;
    RenderSettingsWindow* m_renderSettingsWindow;  // Just a pointer

    std::shared_ptr<SceneManager> m_sceneManager;
    bool m_showFileBrowser = false;
    std::string m_currentPath;
    std::string m_selectedFile;
    std::vector<std::filesystem::directory_entry> m_dirEntries;
    std::string m_loadError;
    const std::vector<std::string> m_modelExtensions = {".obj", ".fbx", ".gltf", ".glb", ".dae"};

public:
    ImGuiLayer()
        : Layer("IMGUI LAYER")
        , m_gpuManager(std::make_shared<GPUDeviceManager>())
    {
    }

    std::shared_ptr<GPUDeviceManager> getGPUManager() const { return m_gpuManager; }

    void setSceneManager(std::shared_ptr<SceneManager> sm) {
        m_sceneManager = sm;
        m_currentPath = findProjectRoot();
    }

    void addWindow(std::unique_ptr<ImGuiWindow> window)
    {
        m_windows.push_back(std::move(window));
    }
    void SetTheme()
    {
        ImGuiStyle& style = ImGui::GetStyle();
        ImVec4* colors = style.Colors;

        // Base dark tones (similar to Blender's UI)
        colors[ImGuiCol_WindowBg] = ImVec4(0.15f, 0.15f, 0.16f, 1.0f);
        colors[ImGuiCol_ChildBg] = ImVec4(0.13f, 0.13f, 0.14f, 1.0f);
        colors[ImGuiCol_PopupBg] = ImVec4(0.17f, 0.17f, 0.18f, 1.0f);
        colors[ImGuiCol_Border] = ImVec4(0.25f, 0.25f, 0.27f, 1.0f);
        colors[ImGuiCol_BorderShadow] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);

        // Headers / Active items
        colors[ImGuiCol_Header] = ImVec4(0.27f, 0.27f, 0.30f, 1.0f);
        colors[ImGuiCol_HeaderHovered] = ImVec4(0.32f, 0.32f, 0.36f, 1.0f);
        colors[ImGuiCol_HeaderActive] = ImVec4(0.38f, 0.38f, 0.42f, 1.0f);

        // Buttons
        colors[ImGuiCol_Button] = ImVec4(0.25f, 0.25f, 0.28f, 1.0f);
        colors[ImGuiCol_ButtonHovered] = ImVec4(0.35f, 0.35f, 0.39f, 1.0f);
        colors[ImGuiCol_ButtonActive] = ImVec4(0.42f, 0.42f, 0.47f, 1.0f);

        // Frame background
        colors[ImGuiCol_FrameBg] = ImVec4(0.20f, 0.20f, 0.22f, 1.0f);
        colors[ImGuiCol_FrameBgHovered] = ImVec4(0.25f, 0.25f, 0.28f, 1.0f);
        colors[ImGuiCol_FrameBgActive] = ImVec4(0.30f, 0.30f, 0.33f, 1.0f);

        // Title bar
        colors[ImGuiCol_TitleBg] = ImVec4(0.12f, 0.12f, 0.13f, 1.0f);
        colors[ImGuiCol_TitleBgActive] = ImVec4(0.18f, 0.18f, 0.20f, 1.0f);
        colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.10f, 0.10f, 0.10f, 1.0f);

        // Tabs
        colors[ImGuiCol_Tab] = ImVec4(0.17f, 0.17f, 0.18f, 1.0f);
        colors[ImGuiCol_TabHovered] = ImVec4(0.28f, 0.28f, 0.32f, 1.0f);
        colors[ImGuiCol_TabActive] = ImVec4(0.24f, 0.24f, 0.27f, 1.0f);
        colors[ImGuiCol_TabUnfocused] = ImVec4(0.15f, 0.15f, 0.16f, 1.0f);
        colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.18f, 0.18f, 0.20f, 1.0f);

        // Resize grip
        colors[ImGuiCol_ResizeGrip] = ImVec4(0.28f, 0.28f, 0.30f, 0.25f);
        colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.33f, 0.33f, 0.36f, 0.4f);
        colors[ImGuiCol_ResizeGripActive] = ImVec4(0.40f, 0.40f, 0.45f, 0.6f);

        // Scrollbar
        colors[ImGuiCol_ScrollbarBg] = ImVec4(0.10f, 0.10f, 0.11f, 1.0f);
        colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.26f, 0.26f, 0.28f, 1.0f);
        colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.32f, 0.32f, 0.36f, 1.0f);
        colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.39f, 0.39f, 0.43f, 1.0f);

        // Text
        colors[ImGuiCol_Text] = ImVec4(0.85f, 0.85f, 0.87f, 1.0f);
        colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.52f, 1.0f);

        // Style adjustments
        style.FrameRounding = 4.0f;
        style.GrabRounding = 2.0f;
        style.TabRounding = 3.0f;
        style.ScrollbarRounding = 3.0f;
    }
    void SetGreenTheme()
    {
        ImGuiStyle& style = ImGui::GetStyle();
        ImVec4* colors = style.Colors;

        // Base dark tones
        colors[ImGuiCol_WindowBg] = ImVec4(0.10f, 0.12f, 0.10f, 1.0f);
        colors[ImGuiCol_ChildBg] = ImVec4(0.09f, 0.10f, 0.09f, 1.0f);
        colors[ImGuiCol_PopupBg] = ImVec4(0.12f, 0.14f, 0.12f, 1.0f);
        colors[ImGuiCol_Border] = ImVec4(0.15f, 0.20f, 0.15f, 1.0f);
        colors[ImGuiCol_BorderShadow] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);

        // Headers / Active items
        colors[ImGuiCol_Header] = ImVec4(0.12f, 0.35f, 0.12f, 1.0f);
        colors[ImGuiCol_HeaderHovered] = ImVec4(0.15f, 0.45f, 0.15f, 1.0f);
        colors[ImGuiCol_HeaderActive] = ImVec4(0.18f, 0.55f, 0.18f, 1.0f);

        // Buttons
        colors[ImGuiCol_Button] = ImVec4(0.10f, 0.25f, 0.10f, 1.0f);
        colors[ImGuiCol_ButtonHovered] = ImVec4(0.15f, 0.35f, 0.15f, 1.0f);
        colors[ImGuiCol_ButtonActive] = ImVec4(0.18f, 0.45f, 0.18f, 1.0f);

        // Frame background
        colors[ImGuiCol_FrameBg] = ImVec4(0.12f, 0.12f, 0.12f, 1.0f);
        colors[ImGuiCol_FrameBgHovered] = ImVec4(0.15f, 0.25f, 0.15f, 1.0f);
        colors[ImGuiCol_FrameBgActive] = ImVec4(0.18f, 0.35f, 0.18f, 1.0f);

        // Title bar
        colors[ImGuiCol_TitleBg] = ImVec4(0.08f, 0.10f, 0.08f, 1.0f);
        colors[ImGuiCol_TitleBgActive] = ImVec4(0.12f, 0.25f, 0.12f, 1.0f);
        colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.06f, 0.06f, 0.06f, 1.0f);

        // Tabs
        colors[ImGuiCol_Tab] = ImVec4(0.10f, 0.12f, 0.10f, 1.0f);
        colors[ImGuiCol_TabHovered] = ImVec4(0.15f, 0.30f, 0.15f, 1.0f);
        colors[ImGuiCol_TabActive] = ImVec4(0.12f, 0.25f, 0.12f, 1.0f);
        colors[ImGuiCol_TabUnfocused] = ImVec4(0.08f, 0.10f, 0.08f, 1.0f);
        colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.10f, 0.20f, 0.10f, 1.0f);

        // Resize grip
        colors[ImGuiCol_ResizeGrip] = ImVec4(0.12f, 0.20f, 0.12f, 0.25f);
        colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.15f, 0.30f, 0.15f, 0.4f);
        colors[ImGuiCol_ResizeGripActive] = ImVec4(0.18f, 0.35f, 0.18f, 0.6f);

        // Scrollbar
        colors[ImGuiCol_ScrollbarBg] = ImVec4(0.05f, 0.05f, 0.05f, 1.0f);
        colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.12f, 0.25f, 0.12f, 1.0f);
        colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.15f, 0.35f, 0.15f, 1.0f);
        colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.18f, 0.45f, 0.18f, 1.0f);

        // Sliders
        colors[ImGuiCol_SliderGrab] = ImVec4(0.12f, 0.30f, 0.12f, 1.0f);
        colors[ImGuiCol_SliderGrabActive] = ImVec4(0.18f, 0.40f, 0.18f, 1.0f);

        // Checkboxes / Radio buttons
        colors[ImGuiCol_CheckMark] = ImVec4(0.18f, 0.40f, 0.18f, 1.0f);

        // Text
        colors[ImGuiCol_Text] = ImVec4(0.85f, 0.85f, 0.85f, 1.0f);
        colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.55f, 0.50f, 1.0f);

        // Style adjustments
        style.FrameRounding = 4.0f;
        style.GrabRounding = 3.0f;
        style.TabRounding = 3.0f;
        style.ScrollbarRounding = 3.0f;
        style.WindowRounding = 5.0f;
    }
    ~ImGuiLayer() override {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }
    void setNativeWindow(GLFWwindow& window, int _width, int _height) {
        std::cout << "setting native window" << std::endl;
        m_window = &window;
        m_width = _width;
        m_height = _height;
        if (!m_window) {
            std::cerr << "Error: Native Window ptr is NULL!" << std::endl;
            return;
        }

    }

    void onAttach() override {
        // Create ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

        // Apply style
        ImGui::StyleColorsDark();
        SetTheme();
        ImGui_ImplGlfw_InitForOpenGL(m_window, true);
        ImGui_ImplOpenGL3_Init("#version 460");
        io.Fonts->Clear();

        // Load your font file (TTF)
	// 	
	std::string fontPath = getAssetPath("GUI/fonts/Nunito/static/Nunito-Regular.ttf");
        ImFont* myFont = io.Fonts->AddFontFromFileTTF(
            fontPath.c_str(), 18.0f
        );

        if (myFont == nullptr)
        {
            std::cerr << "Failed to load font!" << std::endl;
        }

        // Merge FontAwesome icons into the base font.
        static const ImWchar iconRange[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
        ImFontConfig iconConfig;
        iconConfig.MergeMode = true;
        iconConfig.PixelSnapH = true;
        std::string iconPath = getAssetPath("GUI/fonts/awesome/fa-solid-900.ttf");
        ImFont* iconFont = io.Fonts->AddFontFromFileTTF(
            iconPath.c_str(), 16.0f, &iconConfig, iconRange
        );

        if (iconFont == nullptr)
        {
            std::cerr << "Failed to load icon font!" << std::endl;
        }

        // Build font atlas
        unsigned char* tex_pixels = nullptr;
        int tex_width, tex_height;
        io.Fonts->GetTexDataAsRGBA32(&tex_pixels, &tex_width, &tex_height);
        io.FontGlobalScale = 1.8f;

        // Initialize GPU device manager (created in constructor, initialize here)
        m_gpuManager->initialize();

        // Create render settings window and add to windows list
        auto renderSettings = std::make_unique<RenderSettingsWindow>(m_gpuManager);
        m_renderSettingsWindow = renderSettings.get();  // Keep pointer for menu access
        addWindow(std::move(renderSettings));
    }

    void onDetach() override {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    void begin() override {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    void end() override {
        ImGuiIO& io = ImGui::GetIO();
        io.DisplaySize = ImVec2(
            (float)m_width,
            (float)m_height
        );

        // Rendering
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Handle multiple viewports (ImGui docking)
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }
    }

    void onImGuiRender() override {
        static bool dockspaceOpen = true;
        static bool opt_fullscreen = true;
        static bool opt_padding = false;
        static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
        if (opt_fullscreen) {
            const ImGuiViewport* viewport = ImGui::GetMainViewport();
            ImGui::SetNextWindowPos(viewport->WorkPos);
            ImGui::SetNextWindowSize(viewport->WorkSize);
            ImGui::SetNextWindowViewport(viewport->ID);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
            window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
                ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
            window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
        }
        else {
            dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
        }

        // Important: Remove padding for fullscreen mode
        if (!opt_padding)
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

        ImGui::Begin("DockSpace Demo", &dockspaceOpen, window_flags);

        if (!opt_padding)
            ImGui::PopStyleVar();

        if (opt_fullscreen)
            ImGui::PopStyleVar(2);

        // Dockspace
        ImGuiIO& io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
            ImGuiID dockspace_id = ImGui::GetID("MainDockSpace");
            ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
        }

        // Loop over your windows
        for (auto& window : m_windows)
        {
            window->onImGuiRender();
        }

        // Menu Bar
        if (ImGui::BeginMenuBar()) {
            if (ImGui::BeginMenu("File")) {
                if (ImGui::MenuItem("Open", "Ctrl+O")) {
                    m_showFileBrowser = true;
                    m_loadError.clear();
                    m_selectedFile.clear();
                    refreshDirectory();
                }
                if (ImGui::MenuItem("Save", "Ctrl+S")) {
                    // TODO: Handle file save
                }
                ImGui::Separator();
                if (ImGui::MenuItem("Exit", "Alt+F4")) {
                    // TODO: Handle exit
                }
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Edit")) {
                if (ImGui::MenuItem("Undo", "Ctrl+Z")) {
                    // TODO: Handle undo
                }
                if (ImGui::MenuItem("Redo", "Ctrl+Y")) {
                    // TODO: Handle redo
                }
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Render")) {
                if (ImGui::MenuItem("Render Settings")) {
                    m_renderSettingsWindow->open();
                }

                ImGui::Separator();

                // Show active GPU using new API
                const auto& devices = m_gpuManager->getDevices();
                const fungt::GPUDeviceInfo* activeDevice = nullptr;

                for (const auto& device : devices) {
                    if (device.isActive) {
                        activeDevice = &device;
                        break;
                    }
                }

                if (activeDevice) {
                    ImGui::Text("Active Device:");
                    ImGui::TextDisabled("%s (%s)",
                        activeDevice->name.c_str(),
                        activeDevice->getBackendName().c_str());
                }
                else {
                    ImGui::TextDisabled("No GPU selected");
                }

                ImGui::EndMenu();
            }

            ImGui::EndMenuBar();
        }

        ImGui::End();

        if (m_showFileBrowser) {
            renderFileBrowser();
        }
    }

private:
    void refreshDirectory() {
        m_dirEntries.clear();
        try {
            if (!std::filesystem::exists(m_currentPath) ||
                !std::filesystem::is_directory(m_currentPath)) {
                m_currentPath = findProjectRoot();
            }
            for (const auto& entry : std::filesystem::directory_iterator(m_currentPath)) {
                try {
                    if (entry.is_directory()) {
                        m_dirEntries.push_back(entry);
                    } else if (entry.is_regular_file()) {
                        std::string ext = entry.path().extension().string();
                        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
                        for (const auto& validExt : m_modelExtensions) {
                            if (ext == validExt) {
                                m_dirEntries.push_back(entry);
                                break;
                            }
                        }
                    }
                } catch (...) {}
            }
            std::sort(m_dirEntries.begin(), m_dirEntries.end(),
                [](const auto& a, const auto& b) {
                    if (a.is_directory() != b.is_directory()) return a.is_directory();
                    return a.path().filename() < b.path().filename();
                });
        } catch (...) {
            m_currentPath = findProjectRoot();
        }
    }

    void renderFileBrowser() {
        ImGui::SetNextWindowSize(ImVec2(700, 500), ImGuiCond_FirstUseEver);
        if (!ImGui::Begin("Open Model", &m_showFileBrowser)) {
            ImGui::End();
            return;
        }

        ImGui::Text("Path: %s", m_currentPath.c_str());

        if (ImGui::Button(ICON_FA_ARROW_LEFT)) {
            std::filesystem::path parent = std::filesystem::path(m_currentPath).parent_path();
            if (!parent.empty()) {
                m_currentPath = parent.string();
                m_selectedFile.clear();
                refreshDirectory();
            }
        }
        ImGui::SameLine();
        if (ImGui::Button(ICON_FA_SYNC_ALT)) {
            refreshDirectory();
        }

        ImGui::Separator();

        ImGui::BeginChild("FileList", ImVec2(0, -80), true);
        for (size_t i = 0; i < m_dirEntries.size(); ++i) {
            const auto& entry = m_dirEntries[i];
            bool isDir = entry.is_directory();
            std::string name = (isDir ? ICON_FA_FOLDER "  " : ICON_FA_FILE "  ")
                + entry.path().filename().string();

            bool isSelected = (m_selectedFile == entry.path().string());
            if (ImGui::Selectable(name.c_str(), isSelected, ImGuiSelectableFlags_AllowDoubleClick)) {
                if (isDir) {
                    if (ImGui::IsMouseDoubleClicked(0)) {
                        m_currentPath = entry.path().string();
                        m_selectedFile.clear();
                        refreshDirectory();
                        break;
                    }
                } else {
                    m_selectedFile = entry.path().string();
                    if (ImGui::IsMouseDoubleClicked(0)) {
                        loadModel();
                        break;
                    }
                }
            }
        }
        ImGui::EndChild();

        if (!m_loadError.empty()) {
            ImGui::TextColored(ImVec4(1.0f, 0.3f, 0.3f, 1.0f), "%s", m_loadError.c_str());
        }

        ImGui::Text("Selected: %s", m_selectedFile.empty() ? "(none)" :
            std::filesystem::path(m_selectedFile).filename().string().c_str());

        if (ImGui::Button("Open", ImVec2(120, 0))) {
            loadModel();
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0))) {
            m_showFileBrowser = false;
        }

        ImGui::End();
    }

    void loadModel() {
        if (m_selectedFile.empty()) {
            m_loadError = "No file selected";
            return;
        }
        if (!m_sceneManager) {
            m_loadError = "SceneManager not set";
            return;
        }

        try {
            ModelPaths paths;
            paths.path = m_selectedFile;

            auto model = SimpleModel::create();
            model->load(paths);
            m_sceneManager->addRenderableObj(model);

            m_showFileBrowser = false;
            m_selectedFile.clear();
        }
        catch (const std::exception& e) {
            m_loadError = std::string("Failed: ") + e.what();
        }
    }
};

#endif // _IMGUI_LAYER_H_
