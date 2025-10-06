#include "OGLframeBuffer.hpp"


OpenGLFrameBuffer::OpenGLFrameBuffer(const FrameBuffSpec& ogl_spec)
: m_Specification{ ogl_spec } {

    this->invalidate();

}
OpenGLFrameBuffer::~OpenGLFrameBuffer(){
   
    glDeleteFramebuffers(1, &m_RendererID);
    glDeleteTextures((GLsizei)m_ColorAttachments.size(), m_ColorAttachments.data());
    glDeleteTextures(1, &m_DepthAttachment);
}

void OpenGLFrameBuffer::invalidate()
{
    if (m_RendererID) {
        glDeleteFramebuffers(1, &m_RendererID);
        glDeleteTextures((GLsizei)m_ColorAttachments.size(), m_ColorAttachments.data());
        glDeleteTextures(1, &m_DepthAttachment);
    }

    glGenFramebuffers(1, &m_RendererID);
    glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);

    // ---- color attachment (default: 1) ----
    m_ColorAttachments.resize(m_Specification.colorAttachmentCount);
    glGenTextures(1, &m_ColorAttachments[0]);
    glBindTexture(GL_TEXTURE_2D, m_ColorAttachments[0]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8,
        m_Specification.m_width, m_Specification.m_height, 0,
        GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
        GL_TEXTURE_2D, m_ColorAttachments[0], 0);

    // ---- depth attachment ----
    glGenTextures(1, &m_DepthAttachment);
    glBindTexture(GL_TEXTURE_2D, m_DepthAttachment);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8,
        m_Specification.m_width, m_Specification.m_height, 0,
        GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
        GL_TEXTURE_2D, m_DepthAttachment, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cerr << "Framebuffer incomplete!" << std::endl;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
void OpenGLFrameBuffer::bind() {
    glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
    glViewport(0, 0, m_Specification.m_width, m_Specification.m_height);
}

void OpenGLFrameBuffer::unbind() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
