#if !defined(_OPENCL_TEXTURE_HPP_)
#define _OPENCL_TEXTURE_HPP_
#include "idevice_texture.hpp"
#include <string>
#include <vector>
#include <map>
#include <CL/cl.h>

#define CL_MEM_BINDLESS_IMAGE_INTEL 0x10060
#define CL_IMAGE_BINDLESS_HANDLE_INTEL 0x10061

struct OpenCLTextureData {
    cl_mem image;
    uint64_t bindlessHandle;   // only meaningful if isBindless
    bool isBindless;
    int width, height;
    std::string path;
};

class OpenCLTexture : public IDeviceTexture {
private:
    std::vector<OpenCLTextureData> textures;
    std::map<std::string, int> pathToIndex;

    cl_context context;
    cl_platform_id platform;
    bool useBindless;  // decided once at construction, e.g. from driver capability check

    typedef cl_mem(CL_API_CALL* clCreateImageWithPropertiesINTEL_fn)(
        cl_context, const cl_bitfield*, cl_mem_flags,
        const cl_image_format*, const cl_image_desc*, void*, cl_int*);
    clCreateImageWithPropertiesINTEL_fn createBindlessImage = nullptr;

public:
    OpenCLTexture(cl_context context, cl_platform_id platform, bool preferBindless);
    ~OpenCLTexture();

    int loadTexture(const std::string& path) override;
    int getTextureCount() const override;
    void cleanup() override;
    std::vector<cl_mem> getTextureObjects();     // analogous to CUDA's getTextureObjects
    bool isBindlessMode() const { return useBindless; }
};

#endif



#endif // _OPENCL_TEXTURE_HPP_
