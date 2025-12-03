#include "viewport.hpp"

ViewPort::ViewPort()
    : Layer("ViewPort")
{
}

void ViewPort::onAttach()
{
    std::cout << "onAttach : ViewPort" << std::endl;
    FrameBuffSpec spec{ 1280, 720, 1 };
    m_frameBuffer = FrameBuffer::create(spec);
    m_viewportSize = ImVec2(1280, 720);
}

void ViewPort::onDetach()
{
}

void ViewPort::onUpdate()
{
    // If resize buffer exists, it means we just finished resizing
    if (m_resizeBuffer) {
        // Render ONE frame to the new buffer
        m_resizeBuffer->bind();
        glViewport(0, 0,
            static_cast<GLsizei>(m_viewportSize.x),
            static_cast<GLsizei>(m_viewportSize.y));
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (m_RenderFunc) {
            m_RenderFunc();
        }

        m_resizeBuffer->unbind();

        // ATOMIC SWAP - No blink!
        m_frameBuffer = std::move(m_resizeBuffer);
        m_resizeBuffer = nullptr;

        return;  // Done with swap
    }

    // Normal rendering to current framebuffer
    m_frameBuffer->bind();
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (m_RenderFunc) {
        m_RenderFunc();
    }

    m_frameBuffer->unbind();
}

void ViewPort::onImGuiRender()
{
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.10f, 0.10f, 0.10f, 1.0f));
    ImGui::Begin("Viewport");

    ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();

    static double lastResizeRequest = 0.0;
    static bool pendingResize = false;
    static ImVec2 pendingSize = ImVec2(0, 0);
    double currentTime = glfwGetTime();

    float diffX = std::abs(viewportPanelSize.x - m_viewportSize.x);
    float diffY = std::abs(viewportPanelSize.y - m_viewportSize.y);

    if (diffX > 1.0f || diffY > 1.0f)
    {
        if (viewportPanelSize.x > 32 && viewportPanelSize.y > 32)
        {
            m_viewportSize = viewportPanelSize;
            pendingSize = viewportPanelSize;
            lastResizeRequest = currentTime;
            pendingResize = true;
        }
    }

    bool isResizing = ImGui::IsMouseDragging(ImGuiMouseButton_Left);

    if (pendingResize && !isResizing && (currentTime - lastResizeRequest) > 0.25)
    {
        // Create NEW framebuffer in background (doesn't affect display)
        FrameBuffSpec spec{
            static_cast<unsigned int>(m_viewportSize.x),
            static_cast<unsigned int>(m_viewportSize.y),
            1
        };
        m_resizeBuffer = FrameBuffer::create(spec);

        // Next frame's onUpdate() will render to it and swap
        pendingResize = false;
    }

    // Always display CURRENT framebuffer (no blink during swap)
    uint32_t texID = m_frameBuffer->GetColorAttachmentRendererID();
    ImGui::Image((void*)(intptr_t)texID,
        m_viewportSize,
        ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

    ImGui::End();
    ImGui::PopStyleColor();
    ImGui::PopStyleVar();
}

ImVec2 ViewPort::getViewPortSize()
{
    return m_viewportSize;
}
