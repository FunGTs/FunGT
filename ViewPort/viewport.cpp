#include "viewport.hpp"

ViewPort::ViewPort()
:Layer("ViewPort"){

}
void ViewPort::onAttach(){

    FrameBuffSpec spec{ 1280, 720, 1 };
    m_frameBuffer = FrameBuffer::create(spec);

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
    ImGui::Begin("Viewport");
    uint32_t texID = m_frameBuffer->GetColorAttachmentRendererID();
    ImGui::Image((void*)(intptr_t)texID,
        ImVec2{ (float)m_frameBuffer->getFrameBuffSpec().m_width,
               (float)m_frameBuffer->getFrameBuffSpec().m_height },
        ImVec2{ 0,1 }, ImVec2{ 1,0 });
    ImGui::End();
}