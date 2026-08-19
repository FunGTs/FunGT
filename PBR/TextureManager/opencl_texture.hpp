#if !defined(_OPENCL_TEXTURE_HPP_)
#define _OPENCL_TEXTURE_HPP_
#include "idevice_texture.hpp"
#include <string>
#include <vector>
#include <iostream>
#include <map>
#include <cstdint>
#include <stdexcept>
#include <CL/cl.h>
#include <CL/cl_ext.h>

#ifndef CL_MEM_BINDLESS_IMAGE_INTEL
#define CL_MEM_BINDLESS_IMAGE_INTEL 0x10060
#endif
#ifndef CL_IMAGE_BINDLESS_HANDLE_INTEL
#define CL_IMAGE_BINDLESS_HANDLE_INTEL 0x10061
#endif

struct OpenCLTextureData {
    cl_mem image;
    uint64_t bindlessHandle;   // only meaningful if uses bindless images
    bool isBindless;
    int width, height;
    std::string path;
};

class OpenCLTexture : public IDeviceTexture {
private:
    std::vector<OpenCLTextureData> m_textures;
    std::vector<uint64_t> m_bindlessHandles;
    std::map<std::string, int> m_pathToIndex;

    cl_context m_context = nullptr;
    bool m_useBindless = false;
    bool m_handlesDirty = false;
    typedef cl_mem(CL_API_CALL* clCreateImageWithPropertiesINTEL_fn)(
        cl_context, const cl_bitfield*, cl_mem_flags,
        const cl_image_format*, const cl_image_desc*, void*, cl_int*);
    clCreateImageWithPropertiesINTEL_fn createBindlessImage = nullptr;

public:
    OpenCLTexture(cl_context context, cl_platform_id platform, bool useBindless = true);
    ~OpenCLTexture() override;

    OpenCLTexture(const OpenCLTexture&) = delete;
    OpenCLTexture& operator=(const OpenCLTexture&) = delete;
    OpenCLTexture(OpenCLTexture&&) = delete;
    OpenCLTexture& operator=(OpenCLTexture&&) = delete;

    int loadTexture(const std::string& path) override;
    int getTextureCount() const override { return static_cast<int>(m_textures.size() ); };
    void cleanup() override;
    std::vector<cl_mem> getTextureObjects() const;
    const std::vector<uint64_t>& getBindlessHandles() const { return m_bindlessHandles; }
    bool isBindlessMode() const { return m_useBindless; }
    bool handlesAreDirty() const { return m_handlesDirty; }
    void markHandlesClean() { m_handlesDirty = false; }
};


#endif // _OPENCL_TEXTURE_HPP_
