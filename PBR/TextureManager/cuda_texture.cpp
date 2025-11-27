#include "cuda_texture.hpp"
#include "stb_image.h"
#include <iostream>

CUDATexture::CUDATexture() {
    std::cout << "CUDATexture initialized" << std::endl;
}

CUDATexture::~CUDATexture() {
    cleanup();
}

int CUDATexture::loadTexture(const std::string& path) {
    // Check cache
    auto it = pathToIndex.find(path);
    if (it != pathToIndex.end()) {
        std::cout << "  [CUDA] Texture cached: " << path << " (index " << it->second << ")" << std::endl;
        return it->second;
    }

    // Load image with stb_image
    int width, height, channels;
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, 4); // Force RGBA

    if (!data) {
        std::cerr << "  [CUDA] Failed to load: " << path << std::endl;
        std::cerr << "  [CUDA] Error: " << stbi_failure_reason() << std::endl;
        return -1;
    }

    std::cout << "  [CUDA] Loaded: " << path << " (" << width << "x" << height
        << ", " << channels << " channels)" << std::endl;

    CUDATextureData cudaTex;
    cudaTex.width = width;
    cudaTex.height = height;
    cudaTex.path = path;

    // Create CUDA array
    cudaChannelFormatDesc channelDesc = cudaCreateChannelDesc<uchar4>();
    cudaError_t err = cudaMallocArray(&cudaTex.cuArray, &channelDesc, width, height);
    if (err != cudaSuccess) {
        std::cerr << "  [CUDA] cudaMallocArray failed: " << cudaGetErrorString(err) << std::endl;
        stbi_image_free(data);
        return -1;
    }

    // Copy data to CUDA array
    err = cudaMemcpy2DToArray(cudaTex.cuArray, 0, 0, data,
        width * 4, width * 4, height,
        cudaMemcpyHostToDevice);
    if (err != cudaSuccess) {
        std::cerr << "  [CUDA] cudaMemcpy2DToArray failed: " << cudaGetErrorString(err) << std::endl;
        cudaFreeArray(cudaTex.cuArray);
        stbi_image_free(data);
        return -1;
    }

    // Create resource descriptor
    cudaResourceDesc resDesc = {};
    resDesc.resType = cudaResourceTypeArray;
    resDesc.res.array.array = cudaTex.cuArray;

    // Create texture descriptor
    cudaTextureDesc texDesc = {};
    texDesc.addressMode[0] = cudaAddressModeWrap;
    texDesc.addressMode[1] = cudaAddressModeWrap;
    texDesc.filterMode = cudaFilterModeLinear;
    texDesc.readMode = cudaReadModeNormalizedFloat;  // Returns [0,1]
    texDesc.normalizedCoords = 1;

    // Create texture object
    err = cudaCreateTextureObject(&cudaTex.texObj, &resDesc, &texDesc, nullptr);
    if (err != cudaSuccess) {
        std::cerr << "  [CUDA] cudaCreateTextureObject failed: " << cudaGetErrorString(err) << std::endl;
        cudaFreeArray(cudaTex.cuArray);
        stbi_image_free(data);
        return -1;
    }

    stbi_image_free(data);

    // Store and cache
    int idx = textures.size();
    textures.push_back(cudaTex);
    pathToIndex[path] = idx;
    std::cout << "  textures size : " << textures.size() << std::endl;
    std::cout << "  [CUDA] Texture index: " << idx << std::endl;
    return idx;
}

std::vector<cudaTextureObject_t> CUDATexture::getTextureObjects() {
    std::vector<cudaTextureObject_t> objs;
    
    objs.reserve(textures.size());
    for (const auto& tex : textures) {
        objs.push_back(tex.texObj);
    }
    return objs;
}

int CUDATexture::getTextureCount() const {
    return static_cast<int>(textures.size());
}

void CUDATexture::cleanup() {
    std::cout << "  [CUDA] Cleaning up " << textures.size() << " textures..." << std::endl;
    for (auto& tex : textures) {
        if (tex.texObj) {
            cudaDestroyTextureObject(tex.texObj);
        }
        if (tex.cuArray) {
            cudaFreeArray(tex.cuArray);
        }
    }
    textures.clear();
    pathToIndex.clear();
}

