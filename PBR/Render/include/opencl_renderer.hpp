#if !defined(_OPENCL_RENDERER_H_)
#define _OPENCL_RENDERER_H_

#include <GL/glew.h>  // MUST be first!
#include <GLFW/glfw3.h>
#include <CL/cl.h>
#include <CL/cl_ext.h>
#include <CL/cl_platform.h>
#include <CL/cl_gl.h>
#include <CL/cl_gl_ext.h>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include "icompute_renderer.hpp"
#include "PBR/TextureManager/opencl_texture.hpp"

class OpenCL_Renderer : public IComputeRenderer {

    cl_platform_id m_oclplatform = nullptr;
    cl_device_id m_ocldevice = nullptr;
    cl_context m_oclcontext = nullptr;
    cl_command_queue m_oclqueue = nullptr;
    cl_program m_oclprogram = nullptr;
    cl_kernel m_oclrenderKernel = nullptr;
    cl_mem m_texturesObj = nullptr;
    std::size_t m_numTextures = 0;
    std::unique_ptr<OpenCLTexture> m_textureManager;

    void prepareTextures();

    public:
        OpenCL_Renderer()= default;
        ~OpenCL_Renderer() override;

        void initialize();

        IDeviceTexture& textures() override {
            if (!m_textureManager) {
                throw std::runtime_error(
                    "OpenCL texture manager requested before OpenCL initialization.");
            }
            return *m_textureManager;
        }

        std::vector<fungt::Vec3> RenderScene(
            int width,
            int height,
            const std::vector<Triangle>& triangles,
            const std::vector<BVHNode>& nodes,
            const std::vector<Light>& lights,
            const std::vector<int>& emissiveTriIndices,
            const PBRCamera& camera,
            int samplesPerPixel,
            int sampleOffset
        ) override;

        void setOpenCLTextures(const std::vector<uint64_t>& handles);


};


#endif // _OPENCL_RENDERER_H_
