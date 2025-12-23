#if !defined(_OGL_FRAME_BUFFER_)
#define _OGL_FRAME_BUFFER_


#include "../../Renders/framebuffer.hpp"


class OpenGLFrameBuffer : public FrameBuffer{
    private:
        unsigned int m_RendererID = 0;
        FrameBuffSpec m_Specification;

        // allow multiple attachments (RGBA8, depth, cubemap faces later…)
        std::vector<unsigned int> m_ColorAttachments;
        unsigned int m_DepthAttachment = 0;
    public:
        OpenGLFrameBuffer(const FrameBuffSpec & ogl_spec);
        ~OpenGLFrameBuffer();

        void bind() override;
        void unbind() override;
        void resize(unsigned int width, unsigned int height) override;

        FrameBuffSpec& getFrameBuffSpec() override {
            return m_Specification;
        }
        const FrameBuffSpec& getFrameBuffSpec() const override{
            return m_Specification;
        }
        unsigned int GetColorAttachmentRendererID(unsigned int index = 0) const override {
            return m_ColorAttachments.at(index);
        }
    private:
        void invalidate();
};


#endif // _OGL_FRAME_BUFFER_
