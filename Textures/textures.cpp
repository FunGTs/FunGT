#include "textures.hpp"
#include "../vendor/stb_image/stb_image.h"
#include <iostream>

Texture::Texture()
    : m_gpu(GPUTexture::create(TextureType::Texture2D)) {}

Texture::Texture(TextureType type)
    : m_gpu(GPUTexture::create(type)) {}

Texture::Texture(const std::string& path)
    : txt_Path(path), m_gpu(GPUTexture::create(TextureType::Texture2D))
{
    genTexture(path);
}

Texture::Texture(const std::string& path, TextureType type)
    : txt_Path(path), m_gpu(GPUTexture::create(type))
{
    genTexture(path);
}

Texture::~Texture() {
    std::cout << "Texture Destructor" << std::endl;
}

void Texture::genTexture(const std::string& path) {
    txt_Path = path;
    stbi_set_flip_vertically_on_load(1);
    unsigned char* pixels = stbi_load(txt_Path.c_str(), &txt_width, &txt_height, &txt_BBP, 4);

    if (pixels) {
        std::cout << "\nOk to load: " << std::endl;
        printf("%s\n", txt_Path.c_str());
        m_gpu->upload(pixels, txt_width, txt_height);
        stbi_image_free(pixels);
    } else {
        std::cout << "\nError: Failed to load texture" << std::endl;
        std::cout << stbi_failure_reason() << std::endl;
    }
}

void Texture::genTextureCubeMap(const std::vector<std::string>& faces) {
    if (!m_gpu)
        m_gpu = GPUTexture::create(TextureType::CubeMap);

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

    m_gpu->uploadCubeMap(pixelFaces, w, h);

    for (auto* p : pixelFaces)
        if (p) stbi_image_free(p);
}

void Texture::active(unsigned int slot) {
    m_gpu->bind(slot);
}

void Texture::bind() {
    m_gpu->bind();
}

void Texture::unBind() {
    m_gpu->unbind();
}

void Texture::Delete() {
    m_gpu->destroy();
}

uint32_t Texture::getID() const {
    return m_gpu ? m_gpu->getID() : 0;
}
