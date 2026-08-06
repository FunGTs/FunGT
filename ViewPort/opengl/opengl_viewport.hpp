#if !defined(_OPENGL_VIEWPORT_H_)
#define _OPENGL_VIEWPORT_H_

#include "../viewport.hpp"
#include "../../Renders/framebuffer.hpp"
#include "../../include/prerequisites.hpp"
#include <memory>

class OpenGLViewPort : public ViewPort {
private:
    std::shared_ptr<FrameBuffer> m_frameBuffer;
    std::shared_ptr<FrameBuffer> m_resizeBuffer;
    GLuint m_pathTraceTexture = 0;

public:
    OpenGLViewPort();
    ~OpenGLViewPort() = default;

    void onAttach() override;
    void onDetach() override;
    void onUpdate() override;
    void onImGuiRender() override;

    uint32_t getPathTraceTexture() const override { return m_pathTraceTexture; }
};

#endif // _OPENGL_VIEWPORT_H_
