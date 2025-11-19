#if !defined(_CUDA_TEXTURE_HPP_)
#define _CUDA_TEXTURE_HPP_
#include "texture_manager.hpp"
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

class CUDATexture : public ITextureManager<cudaTextureObject_t>,
                    public IDeviceTexture {
private:
    std::vector<CUDATextureData> textures;
    std::map<std::string, int> pathToIndex;  // Cache: path -> index

public:
    CUDATexture();
    ~CUDATexture();

    int loadTexture(const std::string& path) override;
    int getTextureCount() const override;
    void cleanup() override;

    // CUDA-specific: Get raw texture data (for debugging)
    std::vector<cudaTextureObject_t> getTextureObjects() override;
};


#endif // _CUDA_TEXTURE_HPP_
