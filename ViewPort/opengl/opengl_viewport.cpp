#include "opengl_viewport.hpp"

OpenGLViewPort::OpenGLViewPort()
{
}

void OpenGLViewPort::onAttach()
{
    std::cout << "onAttach : OpenGLViewPort" << std::endl;
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

    glGenBuffers(1, &m_pathTracePBO);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, m_pathTracePBO);
    glBufferData(GL_PIXEL_UNPACK_BUFFER, 1280 * 720 * 4 * sizeof(float), nullptr, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
}

void OpenGLViewPort::onDetach()
{
    if (m_pathTraceTexture != 0) {
        glDeleteTextures(1, &m_pathTraceTexture);
        m_pathTraceTexture = 0;
    }
    if (m_pathTracePBO != 0) {
        glDeleteBuffers(1, &m_pathTracePBO);
        m_pathTracePBO = 0;
    }
}

void OpenGLViewPort::onUpdate()
{
    if (m_resizeBuffer) {
        m_resizeBuffer->bind();
        glViewport(0, 0,
            static_cast<GLsizei>(m_viewportSize.x),
            static_cast<GLsizei>(m_viewportSize.y));
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (m_RenderFunc)
            m_RenderFunc();

        m_resizeBuffer->unbind();

        m_frameBuffer = std::move(m_resizeBuffer);
        m_resizeBuffer = nullptr;
        return;
    }

    m_frameBuffer->bind();
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (m_RenderFunc)
        m_RenderFunc();

    m_frameBuffer->unbind();

    if (m_pathTraceMode && m_ActivePathTraceFunc && m_currentSample < m_maxPreviewSamples) {
        int width  = static_cast<int>(m_viewportSize.x);
        int height = static_cast<int>(m_viewportSize.y);

        m_ActivePathTraceFunc(width, height, m_currentSample);
        m_currentSample++;

        if (m_currentSample <= 5 || m_currentSample == m_maxPreviewSamples) {
            std::cout << "Path trace sample " << m_currentSample
                      << "/" << m_maxPreviewSamples << std::endl;
        }
    }
}

void OpenGLViewPort::onImGuiRender()
{
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.10f, 0.10f, 0.10f, 1.0f));
    ImGui::Begin("Viewport");

    ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();

    static double lastResizeRequest = 0.0;
    static bool   pendingResize     = false;
    static ImVec2 pendingSize       = ImVec2(0, 0);
    double currentTime = glfwGetTime();

    float diffX = std::abs(viewportPanelSize.x - m_viewportSize.x);
    float diffY = std::abs(viewportPanelSize.y - m_viewportSize.y);

    if (diffX > 1.0f || diffY > 1.0f) {
        if (viewportPanelSize.x > 32 && viewportPanelSize.y > 32) {
            const float pendingDiffX = std::abs(viewportPanelSize.x - pendingSize.x);
            const float pendingDiffY = std::abs(viewportPanelSize.y - pendingSize.y);
            if (!pendingResize || pendingDiffX > 1.0f || pendingDiffY > 1.0f) {
                pendingSize       = viewportPanelSize;
                lastResizeRequest = currentTime;
                pendingResize     = true;
            }
        }
    }

    bool isResizing = ImGui::IsMouseDragging(ImGuiMouseButton_Left);

    if (pendingResize && !isResizing && (currentTime - lastResizeRequest) > 0.25) {
        // Release the OpenCL wrapper before changing the GL texture storage.
        if (m_PathTraceReleaseInteropFunc) {
            m_PathTraceReleaseInteropFunc();
        }

        FrameBuffSpec spec{
            static_cast<unsigned int>(pendingSize.x),
            static_cast<unsigned int>(pendingSize.y),
            1
        };
        m_resizeBuffer = FrameBuffer::create(spec);
        glBindTexture(GL_TEXTURE_2D, m_pathTraceTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F,
            static_cast<int>(pendingSize.x),
            static_cast<int>(pendingSize.y),
            0, GL_RGBA, GL_FLOAT, nullptr);
        glBindTexture(GL_TEXTURE_2D, 0);

        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, m_pathTracePBO);
        glBufferData(GL_PIXEL_UNPACK_BUFFER,
            static_cast<size_t>(pendingSize.x) * static_cast<size_t>(pendingSize.y) * 4 * sizeof(float),
            nullptr, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

        m_viewportSize = pendingSize;
        m_currentSample = 0;
        pendingResize   = false;
    }

    uint32_t texID;
    if (m_pathTraceMode && m_pathTraceTexture != 0)
        texID = m_pathTraceTexture;
    else
        texID = m_frameBuffer->GetColorAttachmentRendererID();

    const ImVec2 imagePosition = ImGui::GetCursorScreenPos();
    ImGui::Image((void*)(intptr_t)texID,
        viewportPanelSize,
        ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

    if (m_pathTraceMode) {
        const ImU32 textColor = ImGui::ColorConvertFloat4ToU32(
            ImVec4(1.0f, 0.8f, 0.0f, 1.0f));
        auto* drawList = ImGui::GetWindowDrawList();
        drawList->AddText(
            ImVec2(imagePosition.x + 10.0f, imagePosition.y + 10.0f),
            textColor,
            ("RaySpace: " + std::to_string(m_currentSample) + "/" +
                std::to_string(m_maxPreviewSamples) + " samples").c_str());
        drawList->AddText(
            ImVec2(imagePosition.x + 10.0f, imagePosition.y + 30.0f),
            textColor,
            ("Using " + ComputeRender::GetBackendName()).c_str());
    }

    ImGui::End();
    ImGui::PopStyleColor();
    ImGui::PopStyleVar();
}

void OpenGLViewPort::copyPBOToTexture(int width, int height)
{
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, m_pathTracePBO);
    glBindTexture(GL_TEXTURE_2D, m_pathTraceTexture);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height,
        GL_RGBA, GL_FLOAT, nullptr);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
}
