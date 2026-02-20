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

    glGenTextures(1, &m_pathTraceTexture);
    glBindTexture(GL_TEXTURE_2D, m_pathTraceTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F,
        1280, 720, 0,
        GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void ViewPort::onDetach()
{
    if (m_pathTraceTexture != 0) {
        glDeleteTextures(1, &m_pathTraceTexture);
        m_pathTraceTexture = 0;
    }
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

    if (m_pathTraceMode && m_PathTraceFunc && m_currentSample < m_maxPreviewSamples) {
        int width = static_cast<int>(m_viewportSize.x);
        int height = static_cast<int>(m_viewportSize.y);

        m_PathTraceFunc(width, height, m_currentSample);

        m_currentSample++;
        std::cout << "Path trace sample " << m_currentSample
            << "/" << m_maxPreviewSamples << std::endl;
        // Debug output
        if (m_currentSample <= 5 || m_currentSample == m_maxPreviewSamples) {
            std::cout << "Path trace sample " << m_currentSample
                << "/" << m_maxPreviewSamples << std::endl;
        }
    }
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
        glBindTexture(GL_TEXTURE_2D, m_pathTraceTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F,
            static_cast<int>(m_viewportSize.x),
            static_cast<int>(m_viewportSize.y),
            0, GL_RGBA, GL_FLOAT, nullptr);
        glBindTexture(GL_TEXTURE_2D, 0);
        m_currentSample = 0;

        // Next frame's onUpdate() will render to it and swap
        pendingResize = false;
    }

    // Always display CURRENT framebuffer (no blink during swap)
    uint32_t texID; // = m_frameBuffer->GetColorAttachmentRendererID();
    if (m_pathTraceMode && m_pathTraceTexture != 0) {
        texID = m_pathTraceTexture;  // Show path traced result
    }
    else {
        texID = m_frameBuffer->GetColorAttachmentRendererID();  // Show raster
    }

    if (m_pathTraceMode) {
        ImGui::SetCursorPos(ImVec2(10, 30));
        ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.0f, 1.0f),
            "RaySpace: %d/%d samples",
            m_currentSample, m_maxPreviewSamples);
        ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.0f, 1.0f),
            " Using SYCL on iGPU");
    }

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
