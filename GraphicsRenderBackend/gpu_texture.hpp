#if !defined(_GPU_TEXTURE_HPP_)
#define _GPU_TEXTURE_HPP_

#include <memory>
#include <string>
#include <vector>
#include <cstdint>
#include "../Renders/display_graphics.hpp"
#include "texture_enums.hpp"

class GPUTexture {
public:
    virtual ~GPUTexture() = default;

    virtual void upload(const unsigned char* pixels, int width, int height, TextureColorSpace colorSpace) = 0;
    virtual void uploadFloat(const float* pixels, int width, int height) = 0;
    virtual void uploadCubeMap(const std::vector<unsigned char*>& faces, int width, int height) = 0;
    virtual void allocateEmptyCubemap(int faceSize, bool mipmaps) = 0;
    virtual void bind(unsigned int slot = 0) = 0;
    virtual void unbind() = 0;
    virtual void destroy() = 0;
    virtual uint32_t getID() const = 0;

    static std::unique_ptr<GPUTexture> create(TextureType type = TextureType::Texture2D);
};

#endif // _GPU_TEXTURE_HPP_
