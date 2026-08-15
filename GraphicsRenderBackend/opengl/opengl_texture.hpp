#if !defined(_OPENGL_TEXTURE_HPP_)
#define _OPENGL_TEXTURE_HPP_

#include "../gpu_texture.hpp"
#include "../../include/prerequisites.hpp"

class OpenGLTexture : public GPUTexture {
public:
    OpenGLTexture(TextureType type = TextureType::Texture2D);
    ~OpenGLTexture() override;

    void upload(const unsigned char* pixels, int width, int height, TextureColorSpace colorSpace) override;
    void uploadFloat(const float* pixels, int width, int height) override;
    void uploadCubeMap(const std::vector<unsigned char*>& faces, int width, int height) override;
    void allocateEmptyCubemap(int faceSize, bool mipmaps) override;
    void bind(unsigned int slot = 0) override;
    void unbind() override;
    void destroy() override;
    uint32_t getID() const override;

private:
    uint32_t   m_id   = 0;
    TextureType m_type;
    GLenum     m_glTarget;
};

#endif // _OPENGL_TEXTURE_HPP_
