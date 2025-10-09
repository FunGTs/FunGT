#if !defined(_IMGUI_LAYER_H_)
#define _IMGUI_LAYER_H_
#include "../include/prerequisites.hpp"
#include "../include/imgui_headers.hpp"
#include "../Layer/layer.hpp"
class ImGuiLayer : public Layer{

private:
    GLFWwindow* m_window;
    int m_width, m_height;
public:
    ImGuiLayer()
    :Layer("IMGUI LAYER"){
       
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
        io.FontGlobalScale = 1.5f; // 1.0 = default, 1.5 = 50% bigger
        // Apply style
        //ImGui::StyleColorsDark();
        ImGui::StyleColorsClassic();
        SetTheme();
        ImGui_ImplGlfw_InitForOpenGL(m_window, true);
        ImGui_ImplOpenGL3_Init("#version 460");
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

        // Example menu bar
        // if (ImGui::BeginMenuBar()) {
        //     if (ImGui::BeginMenu("File")) {
        //         if (ImGui::MenuItem("Exit")) {
        //             // Handle exit if needed
        //         }
        //         ImGui::EndMenu();
        //     }
        //     ImGui::EndMenuBar();
        // }

        ImGui::End();
    }


};




#endif // _IMGUI_LAYER_H_
