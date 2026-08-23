#if !defined(_OPENCL_RENDERER_H_)
#define _OPENCL_RENDERER_H_
#include <GL/glew.h>  // MUST be first!
#include <GLFW/glfw3.h>
#include <GL/glx.h>
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
#include <array>
#include <fstream>
#include <tuple>
#include "Path_Manager/path_manager.hpp"
#include "icompute_renderer.hpp"
#include "PBR/TextureManager/opencl_texture.hpp"

const std::vector<std::string> OpenCLRendererSourceFiles = {
    "PBR/Render/shared/opencl/fgt_opencl_data.h",
    "PBR/Render/ocl_kernels/rng.cl",
    "PBR/Render/ocl_kernels/rayspace_camera.cl",
    "PBR/Render/ocl_kernels/intersection.cl",
    "PBR/Render/ocl_kernels/bvh.cl",
    "PBR/Render/ocl_kernels/surface_hit.cl",
    "PBR/Render/ocl_kernels/texture_sampling.cl",
    "PBR/Render/ocl_kernels/evaluate_cook_torrance.cl",
    "PBR/Render/ocl_kernels/path_sampling.cl",
    "PBR/Render/ocl_kernels/path_tracer.cl",
    "PBR/Render/ocl_kernels/sample_framebuffer.cl"
};

struct OpenCLRaySpaceBuffer {
    cl_mem triangleGeometry = nullptr;
    cl_mem triangleShading = nullptr;
    cl_mem materials = nullptr;
    cl_mem bvhNodes = nullptr;
    cl_mem lights = nullptr;
    cl_mem emissiveTriangles = nullptr;

    std::size_t numTriangles = 0;
    std::size_t numMaterials = 0;
    std::size_t numBVHNodes = 0;
    std::size_t numLights = 0;
    std::size_t numEmissiveTriangles = 0;
};

class OpenCL_Renderer : public IComputeRenderer {

    cl_platform_id m_oclplatform = nullptr;
    cl_device_id m_ocldevice = nullptr;
    cl_context m_oclcontext = nullptr;
    cl_command_queue m_oclqueue = nullptr;
    cl_program m_oclprogram = nullptr;
    cl_kernel m_oclrenderKernel = nullptr;
    cl_kernel m_oclsampleKernel = nullptr;
    cl_kernel m_ocl_ogldisplayKernel = nullptr;
    cl_mem m_texturesObj = nullptr;
    cl_mem m_textureDimensions = nullptr;
    std::size_t m_numTextures = 0;
    std::unique_ptr<OpenCLTexture> m_textureManager;
    OpenCLRaySpaceBuffer m_raySpaceBuffer;
    bool m_sceneUploaded = false;

    //For OpenGL interop / progressive path tracer (PBO approach)
    cl_mem m_oglSharedBuffer = nullptr;
    GLuint m_oglBufferID = 0;
    int m_oglBufferWidth = 0;
    int m_oglBufferHeight = 0;
    cl_mem m_oclAccumulationBuffer = nullptr;
    int m_accumulationWidth = 0;
    int m_accumulationHeight = 0;
    void prepareTextures();
    void setOpenCLTextures(
        const std::vector<uint64_t>& handles,
        const std::vector<std::array<cl_int, 2>>& dimensions);
    void uploadScene(
        const std::vector<Triangle>& triangles,
        const std::vector<BVHNode>& nodes,
        const std::vector<int>& emissiveTriIndices);
    void loadSceneLigths(const std::vector<Light> &lights);
    void releaseRaySpaceBuffer(OpenCLRaySpaceBuffer& buffers) noexcept;

    public:
        OpenCL_Renderer()= default;
        ~OpenCL_Renderer() override;

        void initialize(bool useOpenGLInterop = false);

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

        //Maybe we need another render scene function 
        //that works with OpenGL interop

        void RenderSceneOpenGLInterop(
            int width,
            int height,
            const std::vector<Triangle>& triangles,
            const std::vector<BVHNode>& nodes,
            const std::vector<Light>& lights,
            const std::vector<int>& emissiveTriIndices,
            const PBRCamera& camera,
            int samplesPerPixel,
            int sampleOffset,
            GLuint glBufferID
        );
        void releaseOpenGLInteropResources() noexcept;
        void invalidateScene() { m_sceneUploaded = false; }
    private:
        void buildOCLPrograms();
        void setKernelArg(cl_kernel kernel, cl_uint &argIndex,size_t argSize,const void* argValue);

};


#endif // _OPENCL_RENDERER_H_
