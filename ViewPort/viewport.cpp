#include "viewport.hpp"

ViewPort::ViewPort()
:Layer("ViewPort"){

}
void ViewPort::onAttach(){
    std::cout<<"onAttach : ViewPort"<<std::endl; 
    FrameBuffSpec spec{ 1280, 720, 1 };
    m_frameBuffer = FrameBuffer::create(spec);

}
void ViewPort::onDetach()
{
}
void ViewPort::onUpdate(){

    m_frameBuffer->bind();

    // Clear
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Draw Pixar ball scene here...
    // e.g. m_Scene->Render();

    m_frameBuffer->unbind();



}
void ViewPort::onImGuiRender()   {
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0)); // no padding around the image
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.10f, 0.10f, 0.10f, 1.0f)); // Blender dark gray
    ImGui::Begin("Viewport");
   
    ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();


    // --- Debounce resize ---
    static double lastResizeRequest = 0.0;
    static bool pendingResize = false;
    double currentTime = glfwGetTime(); // Or your engine's time source

    // Detect a size change
    if (viewportPanelSize.x != m_viewportSize.x || viewportPanelSize.y != m_viewportSize.y)
    {
        if (viewportPanelSize.x > 0 && viewportPanelSize.y > 0)
        {
            m_viewportSize = viewportPanelSize;
            lastResizeRequest = currentTime;
            pendingResize = true;
        }
    }

    // Apply resize only if stable for >150ms
    if (pendingResize && (currentTime - lastResizeRequest) > 0.15)
    {
        m_frameBuffer->resize(
            static_cast<unsigned int>(m_viewportSize.x),
            static_cast<unsigned int>(m_viewportSize.y)
        );
        pendingResize = false;
    }



    uint32_t texID = m_frameBuffer->GetColorAttachmentRendererID();
    ImGui::Image((void*)(intptr_t)texID,
        ImVec2{ m_viewportSize.x,
               m_viewportSize.y },
        ImVec2{ 0,1 }, ImVec2{ 1,0 });
    ImGui::End();

    ImGui::PopStyleColor();
    ImGui::PopStyleVar();
}