#if !defined(_TEXTURES_H_)
#define _TEXTURES_H_
#include <vector>
#include <string>
#include <cstdint>
#include "../GraphicsRenderBackend/texture_enums.hpp"

class TextureGPU;

class Texture {
public:
    std::string name;
    std::string m_type;

private:
    std::string  txt_Path;
    int          txt_width = 0, txt_height = 0, txt_BBP = 0;
    TextureGPU*  m_gpuCache = nullptr;

public:
    Texture();
    Texture(TextureType type);
    Texture(const std::string& path);
    Texture(const std::string& path, TextureType type);
    ~Texture();

    Texture(Texture&& other) noexcept;
    Texture& operator=(Texture&& other) noexcept;
    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;

    void genTexture(const std::string& path,
                    TextureColorSpace colorSpace = TextureColorSpace::SRGB);
    void genTextureHDR(const std::string& path);
    void genTextureCubeMap(const std::vector<std::string>& faces);
    void allocateEmptyCubemap(int faceSize, bool mipmaps);
    void active(unsigned int slot = 0);
    void bind();
    void unBind();
    void Delete();
    uint32_t getID() const;

    inline int getWidth()  const { return txt_width; }
    inline int getHeight() const { return txt_height; }
    inline void setPath(std::string path)              { txt_Path = path; }
    inline std::string getPath()         const         { return txt_Path; }
    inline void setTypeName(std::string textureType)   { m_type = textureType; }
    inline std::string getTypeName()     const         { return m_type; }

    static TextureGPU* (*s_gpuCreate)(TextureType type);
    static void (*s_gpuFree)(TextureGPU*);
    static void (*s_gpuUpload)(TextureGPU&, const unsigned char*, int, int, TextureColorSpace);
    static void (*s_gpuUploadFloat)(TextureGPU&, const float*, int, int);
    static void (*s_gpuUploadCubeMap)(TextureGPU&, const std::vector<unsigned char*>&, int, int);
    static void (*s_gpuAllocateEmptyCubemap)(TextureGPU&, int, bool);
    static void (*s_gpuBind)(TextureGPU&, unsigned int);
    static void (*s_gpuUnbind)(TextureGPU&);
    static void (*s_gpuDestroy)(TextureGPU&);
    static uint32_t (*s_gpuGetID)(const TextureGPU&);
};

#endif // _TEXTURES_H_