#if !defined(_IMGUI_LAYER_H_)
#define _IMGUI_LAYER_H_
#include "imgui_window.hpp"
#include "../Layer/layer.hpp"
#include <memory>
class ImGuiLayer : public Layer{

private:
    GLFWwindow* m_window;
    int m_width, m_height;
    std::vector<std::unique_ptr<ImGuiWindow>> m_windows;

public:
    ImGuiLayer()
    :Layer("IMGUI LAYER"){
       
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
        ImGuiStyle &style = ImGui::GetStyle();
        ImVec4 *colors = style.Colors;

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
    ~ImGuiLayer() override{
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }
    void setNativeWindow(GLFWwindow& window, int _width, int _height) {
        std::cout<<"setting native window"<<std::endl; 
        m_window = &window;
        m_width = _width;
        m_height = _height;
        if (!m_window) {
            std::cerr << "Error: Native Window ptr is NULL!" << std::endl;
            return;
        }

    } 

    void onAttach() override {
        //  Create ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
        
        // Apply style
        ImGui::StyleColorsDark();
        //ImGui::StyleColorsClassic();
        SetTheme();
        ImGui_ImplGlfw_InitForOpenGL(m_window, true);
        ImGui_ImplOpenGL3_Init("#version 460");
        io.Fonts->Clear();
        // Load your font file (TTF)
        ImFont *myFont = io.Fonts->AddFontFromFileTTF(
            "/home/juanchuletas/Documents/Development/FunGT/GUI/fonts/Nunito/static/Nunito-Regular.ttf", 18.0f // path + size in pixels
        );

        if (myFont == nullptr)
        {
            std::cerr << "Failed to load font!" << std::endl;
        }
        // Build font atlas
        unsigned char *tex_pixels = nullptr;
        int tex_width, tex_height;
        io.Fonts->GetTexDataAsRGBA32(&tex_pixels, &tex_width, &tex_height);
        io.FontGlobalScale = 1.8f; // 1.0 = default, 1.5 = 50% bigger
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
        // ===== LOOP OVER YOUR WINDOWS =====
        for (auto &window : m_windows)
        {
            // Each window manages its own Begin/End
            window->onImGuiRender();
        }
        // ==================================
        // Example menu bar
        if (ImGui::BeginMenuBar()) {
            if (ImGui::BeginMenu("File")) {
                if (ImGui::MenuItem("Open")) {
                    // Handle exit if needed
                }
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }
        if (ImGui::BeginMenuBar()) {
            if (ImGui::BeginMenu("Edit")) {
                if (ImGui::MenuItem("Undo")) {
                    // Handle exit if needed
                }
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }
        if (ImGui::BeginMenuBar()) {
            if (ImGui::BeginMenu("Select GPU")) {
                if (ImGui::MenuItem("NVIDIA GeForce")) {
                    // Handle exit if needed
                }
                if (ImGui::MenuItem("Intel Arc")) {
                    // Handle exit if needed
                }
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }
        ImGui::End();
    }


};




#endif // _IMGUI_LAYER_H_
