#include "gui.hpp"


GUI::GUI()
{
}

GUI::~GUI()
{
    this->cleanUp();
}

void GUI::setup(GLFWwindow &window)
{
    m_window = &window;
    if (!m_window) {
        std::cerr << "Error: GLFW window is NULL!" << std::endl;
        return;
    }
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //Docking and viewport:
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    //  // Get DPI scale
     float xscale, yscale;
    glfwGetWindowContentScale(m_window, &xscale, &yscale);
    float scale = std::max(xscale, yscale);
    scale = scale*0.8f;
    
    // // Scale the default font
    io.FontGlobalScale = scale;
    
    // Scale UI elements
    ImGuiStyle& style = ImGui::GetStyle();
    style.ScaleAllSizes(scale);
    

   

    // Setup Dear ImGui style
    //ImGui::StyleColorsDark();
    ImGui::StyleColorsClassic();
   if(io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable){
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }
    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(m_window, true);
    #if defined(__APPLE__)
        ImGui_ImplOpenGL3_Init("#version 150");
    #else
         ImGui_ImplOpenGL3_Init("#version 410");
    #endif
}

void GUI::newFrame()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void GUI::render()
{
    ImGuiIO &io =  ImGui::GetIO();
   // io.DisplaySize = ImVec2();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
       GLFWwindow* backup_curr_ctx = glfwGetCurrentContext();
       ImGui::UpdatePlatformWindows();
       ImGui::RenderPlatformWindowsDefault();
       glfwMakeContextCurrent(backup_curr_ctx);
    }

}

void GUI::cleanUp()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

