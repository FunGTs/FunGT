#if !defined(_IMGUI_LAYER_H_)
#define _IMGUI_LAYER_H_
#include "../include/prerequisites.hpp"
#include "../include/imgui_headers.hpp"
#include "../Layer/layer.hpp"
class ImGuiLayer : public Layer{

private:
    GLFWwindow* m_window;
    int m_width, m_height;

    ImGuiLayer()
    :Layer("IMGUI LAYER"){
       
    }
    ~ImGuiLayer() override = default;

    void setNativeWindow(GLFWwindow& window, int _width, int _height) {

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

        ImGui_ImplGlfw_InitForOpenGL(m_window, true);
        ImGui_ImplOpenGL3_Init("#version 460");
    }

    void onDetach() override {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    void begin() {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    void end() {
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
        // Example Dockspace setup
        static bool dockspaceOpen = true;
        static bool opt_fullscreen = true;
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

        ImGui::Begin("DockSpace Demo", &dockspaceOpen, window_flags);
        ImGui::DockSpace(ImGui::GetID("MainDockSpace"));
        ImGui::End();
    }


};




#endif // _IMGUI_LAYER_H_
