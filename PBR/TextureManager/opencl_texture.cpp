#include "opencl_texture.hpp"
#include "stb_image.h"

OpenCLTexture::OpenCLTexture(cl_context context, cl_platform_id platform, bool useBindless)
: m_context{context}, m_useBindless{useBindless}
{
    if (m_useBindless) {
        createBindlessImage = reinterpret_cast<clCreateImageWithPropertiesINTEL_fn>(
            clGetExtensionFunctionAddressForPlatform(platform, "clCreateImageWithPropertiesINTEL"));
        if (!createBindlessImage) {
            throw std::runtime_error(
                "OpenCLTexture: bindless image API is not available on the selected platform.");
        }
    }
    std::cout << "OpenCLTexture initialized (" << (m_useBindless ? "bindless" : "bound") << ")" << std::endl;
}

OpenCLTexture::~OpenCLTexture()
{
    cleanup();
}

int OpenCLTexture::loadTexture(const std::string& path)
{
    auto it = m_pathToIndex.find(path);
    if (it != m_pathToIndex.end()) {
        std::cout << "  [OpenCL] Texture cached: " << path << " (index " << it->second << ")" << std::endl;
        return it->second;
    }

    int width, height, channels;
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, 4);
    if (!data) {
        std::cerr << "  [OpenCL] Failed to load: " << path << std::endl;
        std::cerr << "  [OpenCL] Error: " << stbi_failure_reason() << std::endl;
        return -1;
    }

    std::cout << "  [OpenCL] Loaded: " << path << " (" << width << "x" << height
        << ", " << channels << " channels)" << std::endl;

    OpenCLTextureData ocltex{};
    ocltex.width = width;
    ocltex.height = height;
    ocltex.path = path;
    ocltex.isBindless = m_useBindless;

    cl_image_format format{};
    format.image_channel_order = CL_sRGBA;
    format.image_channel_data_type = CL_UNORM_INT8;

    cl_image_desc desc{};
    desc.image_type = CL_MEM_OBJECT_IMAGE2D;
    desc.image_width = static_cast<size_t>(width);
    desc.image_height = static_cast<size_t>(height);

    cl_int err = CL_SUCCESS;

    if (m_useBindless) {
        const cl_bitfield props[] = {
            CL_MEM_BINDLESS_IMAGE_INTEL, 1, 0
        };
        ocltex.image = createBindlessImage(m_context, props, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
            &format, &desc, data, &err);
    }
    else {
        ocltex.image = clCreateImage(m_context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
            &format, &desc, data, &err);
    }

    stbi_image_free(data);

    if (err != CL_SUCCESS || !ocltex.image) {
        std::cerr << "  [OpenCL] Failed to create image: " << path << " (err " << err << ")" << std::endl;
        return -1;
    }

    if (m_useBindless) {
        err = clGetImageInfo(ocltex.image, CL_IMAGE_BINDLESS_HANDLE_INTEL,
            sizeof(ocltex.bindlessHandle), &ocltex.bindlessHandle, nullptr);
        if (err != CL_SUCCESS) {
            std::cerr << "  [OpenCL] Failed to get bindless image handle: "
                      << path << " (err " << err << ")" << std::endl;
            clReleaseMemObject(ocltex.image);
            return -1;
        }
    }

    int idx = static_cast<int>(m_textures.size());
    m_textures.push_back(ocltex);
    if (m_useBindless) {
        m_bindlessHandles.push_back(ocltex.bindlessHandle);
        m_handlesDirty = true;
    }
    m_pathToIndex[path] = idx;
    std::cout << "  textures size : " << m_textures.size() << std::endl;
    std::cout << "  [OpenCL] Texture index: " << idx << std::endl;
    return idx;
}

std::vector<cl_mem> OpenCLTexture::getTextureObjects() const {
    std::vector<cl_mem> objs;
    objs.reserve(m_textures.size());
    for (const auto& tex : m_textures) {
        objs.push_back(tex.image);
    }
    return objs;
}

void OpenCLTexture::cleanup()
{
    for (auto& texture : m_textures) {
        if (texture.image) {
            clReleaseMemObject(texture.image);
            texture.image = nullptr;
        }
        texture.bindlessHandle = 0;
    }

    m_textures.clear();
    m_bindlessHandles.clear();
    m_pathToIndex.clear();
    m_handlesDirty = true;
}
