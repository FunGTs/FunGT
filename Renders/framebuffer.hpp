#if !defined(_FRAME_BUFFER_H_)
#define _FRAME_BUFFER_H_
#include<memory>
#include<vector>
#include "../include/prerequisites.hpp"
struct FrameBufferSpecification{

    unsigned int m_width, m_height;
    unsigned int samples = 1;
    uint32_t colorAttachmentCount = 1; // how many color attachments  
}typedef FrameBuffSpec;


class FrameBuffer{

    public:

        virtual FrameBuffSpec& getFrameBuffSpec() = 0; //Sometimes we need to change the spec
        virtual const FrameBuffSpec& getFrameBuffSpec() const = 0;
        virtual void bind() = 0;
        virtual void unbind() = 0; 
        virtual void resize(unsigned int width, unsigned int height) = 0;
        virtual unsigned int GetColorAttachmentRendererID(unsigned int index = 0) const = 0;
        static std::shared_ptr<FrameBuffer> create(const FrameBuffSpec& fbSpec);

};





#endif // _FRAME_BUFFER_H_
