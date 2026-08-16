#include "textures.hpp"
#include "../vendor/stb_image/stb_image.h"
#include <iostream>

TextureGPU* (*Texture::s_gpuCreate)(TextureType) = nullptr;
void (*Texture::s_gpuFree)(TextureGPU*) = nullptr;
void (*Texture::s_gpuUpload)(TextureGPU&, const unsigned char*, int, int, TextureColorSpace) = nullptr;
void (*Texture::s_gpuUploadFloat)(TextureGPU&, const float*, int, int) = nullptr;
void (*Texture::s_gpuUploadCubeMap)(TextureGPU&, const std::vector<unsigned char*>&, int, int) = nullptr;
void (*Texture::s_gpuAllocateEmptyCubemap)(TextureGPU&, int, bool) = nullptr;
void (*Texture::s_gpuBind)(TextureGPU&, unsigned int) = nullptr;
void (*Texture::s_gpuUnbind)(TextureGPU&) = nullptr;
void (*Texture::s_gpuDestroy)(TextureGPU&) = nullptr;
uint32_t (*Texture::s_gpuGetID)(const TextureGPU&) = nullptr;

Texture::Texture() {
    if (s_gpuCreate) {
        m_gpuCache = s_gpuCreate(TextureType::Texture2D);
    }
}

Texture::Texture(TextureType type) {
    if (s_gpuCreate) {
        m_gpuCache = s_gpuCreate(type);
    }
}

Texture::Texture(const std::string& path)
    : txt_Path(path)
{
    if (s_gpuCreate) {
        m_gpuCache = s_gpuCreate(TextureType::Texture2D);
    }
    genTexture(path);
}

Texture::Texture(const std::string& path, TextureType type)
    : txt_Path(path)
{
    if (s_gpuCreate) {
        m_gpuCache = s_gpuCreate(type);
    }
    genTexture(path);
}

Texture::~Texture() {
    if (s_gpuFree && m_gpuCache) {
        s_gpuFree(m_gpuCache);
    }
    std::cout << "Texture Destructor" << std::endl;
}

Texture::Texture(Texture&& other) noexcept
    : name(std::move(other.name)),
      m_type(std::move(other.m_type)),
      txt_Path(std::move(other.txt_Path)),
      txt_width(other.txt_width),
      txt_height(other.txt_height),
      txt_BBP(other.txt_BBP),
      m_gpuCache(other.m_gpuCache)
{
    other.m_gpuCache = nullptr;
}

Texture& Texture::operator=(Texture&& other) noexcept {
    if (this != &other) {
        if (s_gpuFree && m_gpuCache) {
            s_gpuFree(m_gpuCache);
        }
        name = std::move(other.name);
        m_type = std::move(other.m_type);
        txt_Path = std::move(other.txt_Path);
        txt_width = other.txt_width;
        txt_height = other.txt_height;
        txt_BBP = other.txt_BBP;
        m_gpuCache = other.m_gpuCache;
        other.m_gpuCache = nullptr;
    }
    return *this;
}

void Texture::genTexture(const std::string& path, TextureColorSpace colorSpace) {
    txt_Path = path;
    stbi_set_flip_vertically_on_load(1);
    unsigned char* pixels = stbi_load(txt_Path.c_str(), &txt_width, &txt_height, &txt_BBP, 4);

    if (pixels) {
        std::cout << "\nOk to load: " << std::endl;
        printf("%s\n", txt_Path.c_str());
        if (s_gpuUpload && m_gpuCache) {
            s_gpuUpload(*m_gpuCache, pixels, txt_width, txt_height, colorSpace);
        }
        stbi_image_free(pixels);
    } else {
        std::cout << "\nError: Failed to load texture" << std::endl;
        std::cout << stbi_failure_reason() << std::endl;
    }
}

void Texture::genTextureHDR(const std::string& path) {
    txt_Path = path;
    stbi_set_flip_vertically_on_load(1);
    float* pixels = stbi_loadf(txt_Path.c_str(), &txt_width, &txt_height, &txt_BBP, 3);

    if (pixels) {
        std::cout << "\nOk to load HDR: " << std::endl;
        printf("%s\n", txt_Path.c_str());
        if (s_gpuUploadFloat && m_gpuCache) {
            s_gpuUploadFloat(*m_gpuCache, pixels, txt_width, txt_height);
        }
        stbi_image_free(pixels);
    } else {
        std::cout << "\nError: Failed to load HDR texture" << std::endl;
        std::cout << stbi_failure_reason() << std::endl;
    }
}

void Texture::genTextureCubeMap(const std::vector<std::string>& faces) {
    if (!m_gpuCache && s_gpuCreate) {
        m_gpuCache = s_gpuCreate(TextureType::CubeMap);
    }

    std::vector<unsigned char*> pixelFaces;
    int w = 0, h = 0, bbp = 0;

    for (const auto& face : faces) {
        stbi_set_flip_vertically_on_load(false);
        unsigned char* pixels = stbi_load(face.c_str(), &w, &h, &bbp, 4);
        if (pixels) {
            std::cout << "\nOk to load: " << std::endl;
            printf("%s\n", face.c_str());
        } else {
            std::cout << "\nError: Failed to load texture: " << face << std::endl;
            std::cout << stbi_failure_reason() << std::endl;
        }
        pixelFaces.push_back(pixels);
    }

    if (s_gpuUploadCubeMap && m_gpuCache) {
        s_gpuUploadCubeMap(*m_gpuCache, pixelFaces, w, h);
    }

    for (auto* p : pixelFaces)
        if (p) stbi_image_free(p);
}

void Texture::allocateEmptyCubemap(int faceSize, bool mipmaps) {
    if (!m_gpuCache && s_gpuCreate) {
        m_gpuCache = s_gpuCreate(TextureType::CubeMap);
    }
    if (s_gpuAllocateEmptyCubemap && m_gpuCache) {
        s_gpuAllocateEmptyCubemap(*m_gpuCache, faceSize, mipmaps);
    }
}

void Texture::active(unsigned int slot) {
    if (s_gpuBind && m_gpuCache) {
        s_gpuBind(*m_gpuCache, slot);
    }
}

void Texture::bind() {
    if (s_gpuBind && m_gpuCache) {
        s_gpuBind(*m_gpuCache, 0);
    }
}

void Texture::unBind() {
    if (s_gpuUnbind && m_gpuCache) {
        s_gpuUnbind(*m_gpuCache);
    }
}

void Texture::Delete() {
    if (s_gpuDestroy && m_gpuCache) {
        s_gpuDestroy(*m_gpuCache);
    }
}

uint32_t Texture::getID() const {
    if (s_gpuGetID && m_gpuCache) {
        return s_gpuGetID(*m_gpuCache);
    }
    return 0;
}
