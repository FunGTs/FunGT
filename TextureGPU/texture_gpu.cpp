#include "texture_gpu.hpp"
#include "../Textures/textures.hpp"

TextureGPU* BuildTextureGPU(TextureType type) {
    auto* cache = new TextureGPU();
    cache->m_gpu = GPUTexture::create(type);
    return cache;
}

void FreeTextureGPU(TextureGPU* cache) {
    delete cache;
}

void UploadTextureGPU(TextureGPU& cache, const unsigned char* pixels, int w, int h, TextureColorSpace cs) {
    cache.m_gpu->upload(pixels, w, h, cs);
}

void UploadFloatTextureGPU(TextureGPU& cache, const float* pixels, int w, int h) {
    cache.m_gpu->uploadFloat(pixels, w, h);
}

void UploadCubeMapTextureGPU(TextureGPU& cache, const std::vector<unsigned char*>& faces, int w, int h) {
    cache.m_gpu->uploadCubeMap(faces, w, h);
}

void AllocateEmptyCubemapTextureGPU(TextureGPU& cache, int faceSize, bool mipmaps) {
    cache.m_gpu->allocateEmptyCubemap(faceSize, mipmaps);
}

void BindTextureGPU(TextureGPU& cache, unsigned int slot) {
    cache.m_gpu->bind(slot);
}

void UnbindTextureGPU(TextureGPU& cache) {
    cache.m_gpu->unbind();
}

void DestroyTextureGPU(TextureGPU& cache) {
    cache.m_gpu->destroy();
}

uint32_t GetIDTextureGPU(const TextureGPU& cache) {
    return cache.m_gpu ? cache.m_gpu->getID() : 0;
}

void RegisterTextureGPUCallbacks() {
    Texture::s_gpuCreate = &BuildTextureGPU;
    Texture::s_gpuFree = &FreeTextureGPU;
    Texture::s_gpuUpload = &UploadTextureGPU;
    Texture::s_gpuUploadFloat = &UploadFloatTextureGPU;
    Texture::s_gpuUploadCubeMap = &UploadCubeMapTextureGPU;
    Texture::s_gpuAllocateEmptyCubemap = &AllocateEmptyCubemapTextureGPU;
    Texture::s_gpuBind = &BindTextureGPU;
    Texture::s_gpuUnbind = &UnbindTextureGPU;
    Texture::s_gpuDestroy = &DestroyTextureGPU;
    Texture::s_gpuGetID = &GetIDTextureGPU;
}
