#if !defined(_SYCL_TEXTURE_HPP_)
#define _SYCL_TEXTURE_HPP_

#include "idevice_texture.hpp"
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <sycl/sycl.hpp>
#include <sycl/ext/oneapi/bindless_images.hpp>

namespace syclexp = sycl::ext::oneapi::experimental;

struct SYCLTextureData {
    syclexp::sampled_image_handle imgHandle;
    syclexp::image_mem imgMem;
    int width, height;
    std::string path;
};

class SYCLTexture : public IDeviceTexture {
private:
    std::vector<SYCLTextureData> textures;
    std::map<std::string, int> pathToIndex;
    sycl::queue* m_queue;

public:
    SYCLTexture(sycl::queue& queue);
    ~SYCLTexture();

    int loadTexture(const std::string& path) override;
    int getTextureCount() const override{};
    void cleanup() override;

    // MATCHING CUDA PATTERN - return host-side handles!
    std::vector<syclexp::sampled_image_handle> getImageHandles() {
        std::vector<syclexp::sampled_image_handle> handles;
        for (const auto& tex : textures) {
            handles.push_back(tex.imgHandle);
        }
        std::cout<< "HANDLES SIZE : " <<handles.size()<<std::endl;
        return handles;
    }
};

#endif // _SYCL_TEXTURE_HPP_