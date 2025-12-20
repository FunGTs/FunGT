#ifndef SYCL_RENDERER_HPP
#define SYCL_RENDERER_HPP
#include <GL/glew.h>  // MUST be first!
#include <GLFW/glfw3.h>
#include <vector>
#include <funlib/funlib.hpp>
#include <sycl/ext/oneapi/bindless_images.hpp>
#include "icompute_renderer.hpp"
#include "Triangle/triangle.hpp"
#include "Vector/vector3.hpp"
#include "PBR/PBRCamera/pbr_camera.hpp"
#include "PBR/BVH/bvh_node.hpp"
#include "PBR/Light/light.hpp"
#include "Random/fgt_rng.hpp"
#include "PBR/Render/include/sycl_renderer.hpp"
#include "PBR/Render/brdf/cook_torrance.hpp"
#include "PBR/HitData/hit_data.hpp"
#include "PBR/Render/shared/core_renderer.hpp"
namespace syclexp = sycl::ext::oneapi::experimental;
class SYCL_Renderer : public IComputeRenderer {
private:
    sycl::ext::oneapi::experimental::sampled_image_handle* m_textureHandles = nullptr;
    int m_numTextures = 0;
    sycl::queue m_queue;

public:

    SYCL_Renderer(){
       
    }
    std::vector<fungt::Vec3> RenderScene(
        int width,
        int height,
        const std::vector<Triangle>& triangleList,
        const std::vector<BVHNode>& nodes,
        const std::vector<Light>& lightsList,
        const PBRCamera& camera,
        int samplesPerPixel
    ) override;
    void createQueue();
    sycl::queue &getQueue();
    // MATCHING CUDA PATTERN!
    void setSyclTextureHandles(
        const std::vector<syclexp::sampled_image_handle>& handles
    ) {
        std::cout << "*** SETTING SYCL TEXTURE HANDLES ***" << std::endl;

        // Free old handles
        if (m_textureHandles) {
            sycl::free(m_textureHandles, m_queue);
            m_textureHandles = nullptr;
        }

        m_numTextures = handles.size();
        std::cout << "*** NUM SYCL TEXTURE HANDLES *** " << m_numTextures << std::endl;

        if (m_numTextures > 0) {
            // Allocate GPU memory
            m_textureHandles = sycl::malloc_device<syclexp::sampled_image_handle>(
                m_numTextures, m_queue
            );

            // Copy to GPU
            m_queue.memcpy(
                m_textureHandles,
                handles.data(),
                m_numTextures * sizeof(syclexp::sampled_image_handle)
            ).wait();

            std::cout << "  Uploaded " << m_numTextures << " texture handles to GPU" << std::endl;
        }
    }
    ~SYCL_Renderer() {
        if (m_textureHandles) {
            sycl::free(m_textureHandles, m_queue);
            m_textureHandles = nullptr;
        }
    }
};

#endif // SYCL_RENDERER_HPP