#if !defined(_CUDA_RENDER_H_)
#define _CUDA_RENDER_H_
// Define FUNGT_USE_CUDA FIRST, before ANY includes
#if defined(__CUDACC__) || defined(FUNGT_USE_CUDA)
#include <cuda_runtime.h>
#include <curand_kernel.h>
#ifdef __CUDACC__
#include <device_launch_parameters.h>
#endif
#else
    // Define dummy type for non CUDA builds
using cudaTextureObject_t = unsigned long long;
#endif

#include <iostream>
#include "icompute_renderer.hpp"
#include "gpu/include/fgt_cpu_device.hpp"
#include "PBR/Render/shared/core_renderer.hpp"
#include "Random/fgt_rng.hpp"
#include "PBR/Intersection/intersection.hpp"
#include "PBR/Ray/ray.hpp"  

#include "PBR/BVH/bvh_node.hpp"
#include "PBR/Light/light.hpp"
#include "PBR/TextureManager/cuda_texture.hpp"

#define CUDA_CHECK(err) do { cudaError_t e = (err); if (e != cudaSuccess) { \
    std::cerr << "CUDA error: " << cudaGetErrorString(e) << " at " << __FILE__ << ":" << __LINE__ << std::endl; exit(1); }} while(0)

class CUDA_Renderer : public IComputeRenderer{
    CUDATexture m_textures;
    cudaTextureObject_t* m_textureObj= nullptr;
    int m_numTextures = 0;

    void prepareTextures() {
        if (m_textures.handlesAreDirty()) {
            setCudaTextureObjects(m_textures.getTextureObjects());
            m_textures.markHandlesClean();
        }
    }

    void setCudaTextureObjects(const std::vector<cudaTextureObject_t>& textureObj) {
        std::cout<<"*** SETTING CUDA TEXTURE OBJECTS*** "<<std::endl;
        if (m_textureObj) {
            cudaFree(m_textureObj);
            m_textureObj = nullptr;
        }
        m_numTextures = textureObj.size();
        std::cout << "*** NUM CUDA TEXTURE OBJECTS*** " << m_numTextures << std::endl;
        if(m_numTextures>0){
            CUDA_CHECK(cudaMalloc(&m_textureObj,
                m_numTextures * sizeof(cudaTextureObject_t)));
            CUDA_CHECK(cudaMemcpy(m_textureObj,
                textureObj.data(),
                m_numTextures * sizeof(cudaTextureObject_t),
                cudaMemcpyHostToDevice));
            std::cout << "  Uploaded " << m_numTextures << " textures to GPU" << std::endl;
        }
    }

    public: 
        CUDA_Renderer() = default;
        IDeviceTexture& textures() override { return m_textures; }

        std::vector<fungt::Vec3> RenderScene(
            int width, 
            int height,
            const std::vector<Triangle>& triangleList,
            const std::vector<BVHNode> &nodes,
            const std::vector<Light> &lightsList,
            const std::vector<int>& emissiveTriIndices,
            const PBRCamera& camera,
            int samplesPerPixel,
            int sampleOffset
        );
        ~CUDA_Renderer(){
            // Cleanup device textures
            if (m_textureObj) {
                cudaFree(m_textureObj);
                m_textureObj = nullptr;
            }
        }



};




#endif // _CUDA_RENDER_H_
