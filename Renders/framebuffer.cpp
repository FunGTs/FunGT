#include "framebuffer.hpp"
#include "../Platform/OpenGL/OGLframeBuffer.hpp"
std::shared_ptr<FrameBuffer> FrameBuffer::create(const FrameBuffSpec& fbSpec) {

    return std::make_shared<OpenGLFrameBuffer>(fbSpec);
    
}