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

    // Setup Dear ImGui style
    //ImGui::StyleColorsDark();
    ImGui::StyleColorsClassic();  
    
    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(m_window, true);
    #if defined(__APPLE__)
        ImGui_ImplOpenGL3_Init("#version 150");
    #else
         ImGui_ImplOpenGL3_Init("#version 330");
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
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void GUI::cleanUp()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

