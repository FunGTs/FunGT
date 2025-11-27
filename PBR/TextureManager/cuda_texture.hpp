#if !defined(_CUDA_TEXTURE_HPP_)
#define _CUDA_TEXTURE_HPP_
#include "idevice_texture.hpp"
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <cuda_runtime.h>


// CUDA-specific texture data
struct CUDATextureData {
    cudaTextureObject_t texObj;
    cudaArray_t cuArray;
    int width, height;
    std::string path;
};

class CUDATexture : public IDeviceTexture {
private:
    std::vector<CUDATextureData> textures;
    std::map<std::string, int> pathToIndex;  // Cache: path -> index

public:
    CUDATexture();
    ~CUDATexture();

    int loadTexture(const std::string& path) override;
    int getTextureCount() const override;
    void cleanup() override;
    std::vector<cudaTextureObject_t> getTextureObjects();
};


#endif // _CUDA_TEXTURE_HPP_
