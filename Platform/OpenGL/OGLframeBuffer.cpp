// OpenGLFrameBuffer.cpp — hardened version

#include "OGLframeBuffer.hpp"
#include <iostream>

OpenGLFrameBuffer::OpenGLFrameBuffer(const FrameBuffSpec& ogl_spec)
    : m_Specification{ ogl_spec }
{
    std::cout << "OpenGLFrameBuffer constructor" << std::endl;
    if (m_Specification.colorAttachmentCount == 0)
        m_Specification.colorAttachmentCount = 1;

    m_ColorAttachments.resize(m_Specification.colorAttachmentCount, 0u);
    m_RendererID = 0;
    m_DepthAttachment = 0;
    invalidate();
}

OpenGLFrameBuffer::~OpenGLFrameBuffer()
{
    if (m_RendererID) {
        glDeleteFramebuffers(1, &m_RendererID);
        m_RendererID = 0;
    }
    if (!m_ColorAttachments.empty()) {
        glDeleteTextures((GLsizei)m_ColorAttachments.size(), m_ColorAttachments.data());
        std::fill(m_ColorAttachments.begin(), m_ColorAttachments.end(), 0u);
    }
    if (m_DepthAttachment) {
        glDeleteTextures(1, &m_DepthAttachment);
        m_DepthAttachment = 0;
    }
}

static const char* GetFramebufferStatusString(GLenum status) {
    switch (status) {
    case GL_FRAMEBUFFER_COMPLETE: return "GL_FRAMEBUFFER_COMPLETE";
    case GL_FRAMEBUFFER_UNDEFINED: return "GL_FRAMEBUFFER_UNDEFINED";
    case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT: return "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT";
    case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT: return "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT";
    case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER: return "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER";
    case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER: return "GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER";
    case GL_FRAMEBUFFER_UNSUPPORTED: return "GL_FRAMEBUFFER_UNSUPPORTED";
    case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE: return "GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE";
    case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS: return "GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS";
    default: return "UNKNOWN_FRAMEBUFFER_STATUS";
    }
}

void OpenGLFrameBuffer::invalidate()
{
    // Delete old attachments safely
    if (m_RendererID) {
        // Delete the framebuffer object
        glDeleteFramebuffers(1, &m_RendererID);
        m_RendererID = 0;
    }
    if (!m_ColorAttachments.empty()) {
        // Delete any existing textures (safe even if IDs are 0)
        glDeleteTextures((GLsizei)m_ColorAttachments.size(), m_ColorAttachments.data());
        std::fill(m_ColorAttachments.begin(), m_ColorAttachments.end(), 0u);
    }
    if (m_DepthAttachment) {
        glDeleteTextures(1, &m_DepthAttachment);
        m_DepthAttachment = 0;
    }

    // Create new framebuffer
    glGenFramebuffers(1, &m_RendererID);
    glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);

    // Create color attachments
    for (size_t i = 0; i < m_ColorAttachments.size(); ++i) {
        GLuint tex = 0;
        glGenTextures(1, &tex);
        m_ColorAttachments[i] = tex;
        glBindTexture(GL_TEXTURE_2D, tex);

        // Allocate texture storage (undefined contents) — we'll clear it immediately below
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8,
            (GLsizei)m_Specification.m_width, (GLsizei)m_Specification.m_height, 0,
            GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

        // Important texture params
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        // Attach to framebuffer
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + (GLenum)i,
            GL_TEXTURE_2D, m_ColorAttachments[i], 0);
    }

    // Tell GL which color attachments we'll draw into (even if only one)
    if (!m_ColorAttachments.empty()) {
        std::vector<GLenum> drawBuffers;
        drawBuffers.reserve(m_ColorAttachments.size());
        for (size_t i = 0; i < m_ColorAttachments.size(); ++i)
            drawBuffers.push_back(GL_COLOR_ATTACHMENT0 + (GLenum)i);
        glDrawBuffers((GLsizei)drawBuffers.size(), drawBuffers.data());
    }
    else {
        // No color attachments — explicit fallback
        GLenum db = GL_NONE;
        glDrawBuffers(1, &db);
    }

    // Create depth-stencil texture
    glGenTextures(1, &m_DepthAttachment);
    glBindTexture(GL_TEXTURE_2D, m_DepthAttachment);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8,
        (GLsizei)m_Specification.m_width, (GLsizei)m_Specification.m_height, 0,
        GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_DepthAttachment, 0);

    // Check status
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Framebuffer incomplete: " << GetFramebufferStatusString(status) << std::endl;
        // helpful debug info: list attachments
        for (size_t i = 0; i < m_ColorAttachments.size(); ++i)
            std::cerr << "  color[" << i << "] = " << m_ColorAttachments[i] << std::endl;
        std::cerr << "  depth = " << m_DepthAttachment << std::endl;
    }
    else {
        // clear the newly-created framebuffer to avoid uninitialized garbage
        GLint prevFBO = 0;
        glGetIntegerv(GL_FRAMEBUFFER_BINDING, &prevFBO);
        glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
        glViewport(0, 0, (GLsizei)m_Specification.m_width, (GLsizei)m_Specification.m_height);
        glClearColor(0.08f, 0.08f, 0.08f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        // restore previous binding (if any)
        glBindFramebuffer(GL_FRAMEBUFFER, (GLuint)prevFBO);
    }

    // Unbind texture and framebuffer
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void OpenGLFrameBuffer::bind()
{
    if (!m_RendererID) return;

    glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);

    // Always set viewport to FBO size when binding
    glViewport(0, 0, (GLsizei)m_Specification.m_width, (GLsizei)m_Specification.m_height);

    // Standard state for 3D rendering
    //glEnable(GL_DEPTH_TEST);
    //glEnable(GL_CULL_FACE);
    //glCullFace(GL_BACK);
}

void OpenGLFrameBuffer::unbind()
{
    // Bind default framebuffer (0) and restore window viewport
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Restore viewport to window size
    GLFWwindow* ctx = glfwGetCurrentContext();
    if (ctx) {
        int displayW, displayH;
        glfwGetFramebufferSize(ctx, &displayW, &displayH);
        glViewport(0, 0, displayW, displayH);
    }
}

void OpenGLFrameBuffer::resize(unsigned int width, unsigned int height)
{
    if (width == 0 || height == 0) return;

    if (m_Specification.m_width == width && m_Specification.m_height == height)
        return;

    m_Specification.m_width = width;
    m_Specification.m_height = height;

    // Recreate attachments with new size
    invalidate();
}
