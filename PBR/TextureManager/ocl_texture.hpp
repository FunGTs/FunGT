#if !defined(_OPENCL_TEXTURE_HPP_)
#define _OPENCL_TEXTURE_HPP_
#include <CL/cl.h>
#include <stdexcept>
#include <cstring>
#include "idevice_texture.hpp"
#define CL_MEM_BINDLESS_IMAGE_INTEL 0x10060
#define CL_IMAGE_BINDLESS_HANDLE_INTEL 0x10061

class OpenCLTexture 
{
    uint64_t m_handleId;
    cl_image_format m_imageFormat{};
    m_image_format.image_channel_order = CL_RGBA;
    m_image_format.image_channel_data_type = CL_UNORM_INT8;

    public:
        OCLTexture();




};




#endif // _OPENCL_TEXTURE_HPP_
