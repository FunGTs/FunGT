#if !defined(_FRAME_BUFFER_H_)
#define _FRAME_BUFFER_H_
#include<memory>
struct FrameBufferSpecification{

    unsigned int m_width, m_height;
    unsigned int samples = 1;  
}typedef FrameBuffSpec;


class FrameBuffer{

    public:

        virtual FrameBuffSpec& getFrameBuffSpec() = 0; //Sometimes we need to change the spec
        virtual const FrameBuffSpec& getFrameBuffSpec() const = 0;


        static std::shared_ptr<FrameBuffer> create(const FrameBuffSpec& fbSpec); 

};





#endif // _FRAME_BUFFER_H_
