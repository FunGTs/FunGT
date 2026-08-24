#include "opencl_renderer.hpp"
#include "PBR/Render/shared/opencl/fgt_opencl_data_translator.hpp"
#include <algorithm>
#include <iterator>

void OpenCL_Renderer::initialize(bool useOpenGLInterop)
{
    if (m_oclcontext && m_oclqueue && m_textureManager) {
        return;
    }
    cl_uint platformCount = 0;
    cl_int err = clGetPlatformIDs(0, nullptr, &platformCount);
    if (err != CL_SUCCESS || platformCount == 0) {
        throw std::runtime_error(
            "OpenCL initialization failed: no OpenCL platforms available, error " +
            std::to_string(err));
    }

    std::vector<cl_platform_id> platforms(platformCount);
    err = clGetPlatformIDs(platformCount, platforms.data(), nullptr);
    if (err != CL_SUCCESS) {
        throw std::runtime_error(
            "OpenCL initialization failed while enumerating platforms, error " +
            std::to_string(err));
    }

    std::array<cl_context_properties, 7> contextProperties{};
    const cl_context_properties* properties = nullptr;

    if (useOpenGLInterop) {
        const auto glContext = glXGetCurrentContext();
        const auto glDisplay = glXGetCurrentDisplay();
        if (!glContext || !glDisplay) {
            throw std::runtime_error(
                "OpenGL context is not current in this thread.");
        }

        using GetGLContextInfo = cl_int (CL_API_CALL *)(
            const cl_context_properties*,
            cl_gl_context_info,
            std::size_t,
            void*,
            std::size_t*);

        for (cl_platform_id platform : platforms) {
            if (!clGetExtensionFunctionAddressForPlatform(
                    platform, "clCreateImageWithPropertiesINTEL")) {
                continue;
            }

            contextProperties = {
                CL_GL_CONTEXT_KHR,
                reinterpret_cast<cl_context_properties>(glContext),
                CL_GLX_DISPLAY_KHR,
                reinterpret_cast<cl_context_properties>(glDisplay),
                CL_CONTEXT_PLATFORM,
                reinterpret_cast<cl_context_properties>(platform),
                0
            };

            const auto getGLContextInfo = reinterpret_cast<GetGLContextInfo>(
                clGetExtensionFunctionAddressForPlatform(
                    platform, "clGetGLContextInfoKHR"));
            if (!getGLContextInfo) {
                continue;
            }

            cl_device_id glDevice = nullptr;
            err = getGLContextInfo(
                contextProperties.data(),
                CL_CURRENT_DEVICE_FOR_GL_CONTEXT_KHR,
                sizeof(glDevice),
                &glDevice,
                nullptr);
            if (err == CL_SUCCESS && glDevice) {
                m_oclplatform = platform;
                m_ocldevice = glDevice;
                properties = contextProperties.data();
                break;
            }
        }
    } else {
        for (cl_platform_id platform : platforms) {
            if (!clGetExtensionFunctionAddressForPlatform(
                    platform, "clCreateImageWithPropertiesINTEL")) {
                continue;
            }

            cl_device_id device = nullptr;
            err = clGetDeviceIDs(
                platform, CL_DEVICE_TYPE_GPU, 1, &device, nullptr);
            if (err == CL_SUCCESS && device) {
                m_oclplatform = platform;
                m_ocldevice = device;
                break;
            }
        }
    }

    if (!m_oclplatform || !m_ocldevice) {
        throw std::runtime_error(
            "OpenCL initialization failed: no GPU platform exposes the bindless image API.");
    }

    char platformName[256] = {};
    char deviceName[256] = {};
    clGetPlatformInfo(
        m_oclplatform, CL_PLATFORM_NAME,
        sizeof(platformName), platformName, nullptr);
    clGetDeviceInfo(
        m_ocldevice, CL_DEVICE_NAME,
        sizeof(deviceName), deviceName, nullptr);
    std::cout << "OpenCL platform: " << platformName << std::endl;
    std::cout << "OpenCL device:   " << deviceName << std::endl;
    if (useOpenGLInterop) {
        char extensions[2048] = {};
        clGetDeviceInfo(m_ocldevice, CL_DEVICE_EXTENSIONS, sizeof(extensions), extensions, nullptr);
        if (std::string(extensions).find("cl_khr_gl_sharing") == std::string::npos) {
            throw std::runtime_error("OpenCL device does not support OpenGL interoperability.");
        }
    }
    m_oclcontext = clCreateContext(
        properties, 1, &m_ocldevice, nullptr, nullptr, &err);
    if (err != CL_SUCCESS || !m_oclcontext) {
        m_oclcontext = nullptr;
        throw std::runtime_error(
            "OpenCL initialization failed while creating the context, error " +
            std::to_string(err));
    }

    // This is an in order queuee
    // Commands run in the order we add them.
    m_oclqueue = clCreateCommandQueueWithProperties(
        m_oclcontext, m_ocldevice, nullptr, &err);
    if (err != CL_SUCCESS || !m_oclqueue) {
        m_oclqueue = nullptr;
        clReleaseContext(m_oclcontext);
        m_oclcontext = nullptr;
        throw std::runtime_error(
            "OpenCL initialization failed while creating the command queue, error " +
            std::to_string(err));
    }

    m_textureManager = std::make_unique<OpenCLTexture>(
        m_oclcontext, m_oclplatform, true);

    // Defer kernel build until textures are loaded for bindless to work
    std::cout << "OpenCL context and command queue initialized (kernel build deferred)." << std::endl;

}

OpenCL_Renderer::~OpenCL_Renderer()
{
    if (m_oclqueue) {
        clFinish(m_oclqueue);
    }

    m_textureManager.reset();

    if (m_textureDimensions) {
        clReleaseMemObject(m_textureDimensions);
        m_textureDimensions = nullptr;
    }
    if (m_texturesObj) {
        clReleaseMemObject(m_texturesObj);
        m_texturesObj = nullptr;
    }
    m_numTextures = 0;

    releaseRaySpaceBuffer(m_raySpaceBuffer);
    m_sceneUploaded = false;

    if (m_oclrenderKernel) {
        clReleaseKernel(m_oclrenderKernel);
        m_oclrenderKernel = nullptr;
    }
    if (m_oclsampleKernel) {
        clReleaseKernel(m_oclsampleKernel);
        m_oclsampleKernel = nullptr;
    }
    if (m_ocl_ogldisplayKernel) {
        clReleaseKernel(m_ocl_ogldisplayKernel);
        m_ocl_ogldisplayKernel = nullptr;
    }

    if (m_oclprogram) {
        clReleaseProgram(m_oclprogram);
        m_oclprogram = nullptr;
    }

    if (m_oclAccumulationBuffer) {
        clReleaseMemObject(m_oclAccumulationBuffer);
        m_oclAccumulationBuffer = nullptr;
        m_accumulationWidth = 0;
        m_accumulationHeight = 0;
    }
    if (m_oglSharedBuffer) {
        clReleaseMemObject(m_oglSharedBuffer);
        m_oglSharedBuffer = nullptr;
        m_oglBufferID = 0;
        m_oglBufferWidth = 0;
        m_oglBufferHeight = 0;
    }

    if (m_oclqueue) {
        clReleaseCommandQueue(m_oclqueue);
        m_oclqueue = nullptr;
    }
    if (m_oclcontext) {
        clReleaseContext(m_oclcontext);
        m_oclcontext = nullptr;
    }

    m_ocldevice = nullptr;
    m_oclplatform = nullptr;
}

void OpenCL_Renderer::releaseOpenGLInteropResources() noexcept
{
    if (m_oclqueue) {
        clFinish(m_oclqueue);
    }

    if (m_oglSharedBuffer) {
        clReleaseMemObject(m_oglSharedBuffer);
        m_oglSharedBuffer = nullptr;
    }
    m_oglBufferID = 0;
    m_oglBufferWidth = 0;
    m_oglBufferHeight = 0;

    if (m_oclAccumulationBuffer) {
        clReleaseMemObject(m_oclAccumulationBuffer);
        m_oclAccumulationBuffer = nullptr;
    }
    m_accumulationWidth = 0;
    m_accumulationHeight = 0;
}

std::vector<fungt::Vec3> OpenCL_Renderer::RenderScene(int width, 
    int height, 
    const std::vector<Triangle>& triangles, 
    const std::vector<BVHNode>& nodes, 
    const std::vector<Light>& lights, 
    const std::vector<int>& emissiveTriIndices, 
    const PBRCamera& camera, 
    int samplesPerPixel, 
    int sampleOffset)
{
    if (width <= 0 || height <= 0) {
        throw std::invalid_argument(
            "OpenCL RenderScene requires positive image dimensions.");
    }
    if (samplesPerPixel <= 0) {
        throw std::invalid_argument(
            "OpenCL RenderScene requires at least one sample per pixel.");
    }
    if (!m_oclcontext || !m_oclqueue) {
        throw std::runtime_error(
            "OpenCL RenderScene called before renderer initialization.");
    }

    prepareTextures();

    if (!m_oclrenderKernel) {
        throw std::runtime_error(
            "OpenCL RenderScene: kernel not built (no textures loaded?).");
    }
    if (!m_sceneUploaded) {
        uploadScene(triangles, nodes, emissiveTriIndices);
        loadSceneLigths(lights);
        
    }

    const fgt_rayspace_camera cameraData =
        fgt::opencl::translate_rayspace_camera(camera);
    const std::size_t imageSize =
        static_cast<std::size_t>(width) * static_cast<std::size_t>(height);

    cl_int err = CL_SUCCESS;
    cl_mem framebufferBuffer = clCreateBuffer(
        m_oclcontext,
        CL_MEM_READ_WRITE,
        imageSize * sizeof(fgt_vec4),
        nullptr,
        &err);
    if (err != CL_SUCCESS || !framebufferBuffer) {
        throw std::runtime_error(
            "OpenCL RenderScene failed to create framebuffer, error " +
            std::to_string(err));
    }

    cl_mem cameraBuffer = clCreateBuffer(
        m_oclcontext,
        CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
        sizeof(fgt_rayspace_camera),
        const_cast<fgt_rayspace_camera*>(&cameraData),
        &err);
    if (err != CL_SUCCESS || !cameraBuffer) {
        clReleaseMemObject(framebufferBuffer);
        throw std::runtime_error(
            "OpenCL RenderScene failed to create camera buffer, error " +
            std::to_string(err));
    }

    const fgt_vec4 zeroPixel{0.0f, 0.0f, 0.0f, 0.0f};
    // This clear runs before all sample kernels.
    err = clEnqueueFillBuffer(
        m_oclqueue,
        framebufferBuffer,
        &zeroPixel,
        sizeof(zeroPixel),
        0,
        imageSize * sizeof(fgt_vec4),
        0,
        nullptr,
        nullptr);
    if (err != CL_SUCCESS) {
        clReleaseMemObject(cameraBuffer);
        clReleaseMemObject(framebufferBuffer);
        throw std::runtime_error(
            "OpenCL RenderScene failed to clear framebuffer, error " +
            std::to_string(err));
    }

    const cl_int numTriangles = static_cast<cl_int>(m_raySpaceBuffer.numTriangles);
    const cl_int numMaterials = static_cast<cl_int>(m_raySpaceBuffer.numMaterials);
    const cl_int numBVHNodes =  static_cast<cl_int>(m_raySpaceBuffer.numBVHNodes);
    const cl_int numLights = static_cast<cl_int>(m_raySpaceBuffer.numLights);
    const cl_int numEmissiveTriangles = static_cast<cl_int>(m_raySpaceBuffer.numEmissiveTriangles);
    const cl_int numTextures = static_cast<cl_int>(m_numTextures);

    cl_uint argument = 0;
    try {
        setKernelArg(m_oclrenderKernel,argument, sizeof(cl_mem), &framebufferBuffer);
        setKernelArg(m_oclrenderKernel,argument, sizeof(cl_mem), &m_raySpaceBuffer.triangleGeometry);
        setKernelArg(m_oclrenderKernel,argument, sizeof(cl_mem), &m_raySpaceBuffer.triangleShading);
        setKernelArg(m_oclrenderKernel,argument, sizeof(cl_mem), &m_raySpaceBuffer.materials);
        setKernelArg(m_oclrenderKernel,argument, sizeof(cl_mem), &m_raySpaceBuffer.bvhNodes);
        setKernelArg(m_oclrenderKernel,argument, sizeof(cl_mem), &m_raySpaceBuffer.lights);
        setKernelArg(m_oclrenderKernel,argument, sizeof(cl_mem), &m_raySpaceBuffer.emissiveTriangles);
        setKernelArg(m_oclrenderKernel,argument, sizeof(cl_mem), &m_texturesObj);
        setKernelArg(m_oclrenderKernel,argument, sizeof(cl_mem), &m_textureDimensions);
        setKernelArg(m_oclrenderKernel,argument, sizeof(cl_int), &numTriangles);
        setKernelArg(m_oclrenderKernel,argument, sizeof(cl_int), &numMaterials);
        setKernelArg(m_oclrenderKernel,argument, sizeof(cl_int), &numBVHNodes);
        setKernelArg(m_oclrenderKernel,argument, sizeof(cl_int), &numLights);
        setKernelArg(m_oclrenderKernel,argument, sizeof(cl_int), &numEmissiveTriangles);
        setKernelArg(m_oclrenderKernel,argument, sizeof(cl_int), &numTextures);
        setKernelArg(m_oclrenderKernel,argument, sizeof(cl_mem), &cameraBuffer);
        setKernelArg(m_oclrenderKernel,argument, sizeof(cl_int), &width);
        setKernelArg(m_oclrenderKernel,argument, sizeof(cl_int), &height);

        const cl_int initialSampleIndex = sampleOffset;
        setKernelArg(m_oclrenderKernel,argument,sizeof(cl_int), &initialSampleIndex);

        const std::size_t localSize[2] = {16, 16};
        const std::size_t globalSize[2] = {
            (static_cast<std::size_t>(width) + localSize[0] - 1) /
                localSize[0] * localSize[0],
            (static_cast<std::size_t>(height) + localSize[1] - 1) /
                localSize[1] * localSize[1]
        };

        constexpr cl_uint sampleIndexArgument = 18;
        for (int sample = 0; sample < samplesPerPixel; ++sample) {
            const cl_int sampleIndex = sampleOffset + sample;
            err = clSetKernelArg(
                m_oclrenderKernel,
                sampleIndexArgument,
                sizeof(sampleIndex),
                &sampleIndex);
            if (err != CL_SUCCESS) {
                throw std::runtime_error(
                    "OpenCL RenderScene failed to set sample index, error " +
                    std::to_string(err));
            }

            err = clEnqueueNDRangeKernel(
                m_oclqueue,
                m_oclrenderKernel,
                2,
                nullptr,
                globalSize,
                localSize,
                0,
                nullptr,
                nullptr);
            if (err != CL_SUCCESS) {
                throw std::runtime_error(
                    "OpenCL RenderScene failed to enqueue path_tracer sample " +
                    std::to_string(sampleIndex) + ", error " +
                    std::to_string(err));
            }
        } // When this ends framebuffer is filled with samplesPerPixel samples per pixel.

        // Now add args for the second kernel.
        argument = 0;

        setKernelArg(m_oclsampleKernel, argument,sizeof(cl_mem), &framebufferBuffer);
        setKernelArg(m_oclsampleKernel, argument, sizeof(cl_int), &samplesPerPixel);

        // Divide each pixel by samplesPerPixel.
        const std::size_t sampleGlobalSize[1] = {imageSize};

        err = clEnqueueNDRangeKernel(
            m_oclqueue,
            m_oclsampleKernel,
            1,
            nullptr,
            sampleGlobalSize,
            nullptr,
            0,
            nullptr,
            nullptr);
        if (err != CL_SUCCESS) {
            throw std::runtime_error(
                "OpenCL RenderScene failed to enqueue sample_framebuffer kernel, error " +
                std::to_string(err));
        }
        // Get the framebuffer back to host memory
        std::vector<fgt_vec4> finalframebuffer(imageSize);
        // CL_TRUE waits for this read and all earlier kernels.
        err = clEnqueueReadBuffer(
            m_oclqueue,
            framebufferBuffer,
            CL_TRUE,
            0,
            imageSize * sizeof(fgt_vec4),
            finalframebuffer.data(),
            0,
            nullptr,
            nullptr);
        if (err != CL_SUCCESS) {
            throw std::runtime_error(
                "OpenCL RenderScene failed to read framebuffer, error " +
                std::to_string(err));
        }

        std::vector<fungt::Vec3> framebuffer(imageSize);
        for (std::size_t index = 0; index < imageSize; ++index) {
            framebuffer[index] = fungt::Vec3(
                finalframebuffer[index].x,
                finalframebuffer[index].y,
                finalframebuffer[index].z);
        }

        clReleaseMemObject(cameraBuffer);
        clReleaseMemObject(framebufferBuffer);

        return framebuffer;
    }
    catch (...) {
        clReleaseMemObject(cameraBuffer);
        clReleaseMemObject(framebufferBuffer);
        throw;
    }
}

void OpenCL_Renderer::uploadScene(
    const std::vector<Triangle>& triangles,
    const std::vector<BVHNode>& nodes,
    const std::vector<int>& emissiveTriIndices)
{
    if (!m_oclcontext) {
        throw std::runtime_error(
            "uploadScene: OpenCL context is not initialized.");
    }

    const fgt::opencl::fgt_opencl_scene_data sceneData =
        fgt::opencl::translate_scene_data(triangles, nodes);

    std::cout << "[uploadScene] Materials texture indices: ";
    for (size_t i = 0; i < sceneData.materials.size(); ++i) {
        std::cout << "mat" << i << "=" << sceneData.materials[i].base_color_texture_index << " ";
    }
    std::cout << std::endl;

    std::vector<fgt_int32> emissiveIndices;
    emissiveIndices.reserve(emissiveTriIndices.size());
    for (int index : emissiveTriIndices) {
        emissiveIndices.push_back(static_cast<fgt_int32>(index));
    }

    OpenCLRaySpaceBuffer newBuffers;
    newBuffers.numTriangles = sceneData.triangle_geometry.size();
    newBuffers.numMaterials = sceneData.materials.size();
    newBuffers.numBVHNodes = sceneData.bvh_nodes.size();
    newBuffers.numEmissiveTriangles = emissiveIndices.size();

    cl_int err = CL_SUCCESS;

    if (!sceneData.triangle_geometry.empty()) {
        newBuffers.triangleGeometry = clCreateBuffer(
            m_oclcontext,
            CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
            sceneData.triangle_geometry.size() * sizeof(fgt_triangle_geom),
            const_cast<fgt_triangle_geom*>(sceneData.triangle_geometry.data()),
            &err);
        if (err != CL_SUCCESS || !newBuffers.triangleGeometry) {
            releaseRaySpaceBuffer(newBuffers);
            throw std::runtime_error(
                "uploadScene: failed to create triangle geometry buffer, error " +
                std::to_string(err));
        }
    }

    if (!sceneData.triangle_shading.empty()) {
        newBuffers.triangleShading = clCreateBuffer(
            m_oclcontext,
            CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
            sceneData.triangle_shading.size() * sizeof(fgt_triangle_shading),
            const_cast<fgt_triangle_shading*>(sceneData.triangle_shading.data()),
            &err);
        if (err != CL_SUCCESS || !newBuffers.triangleShading) {
            releaseRaySpaceBuffer(newBuffers);
            throw std::runtime_error(
                "uploadScene: failed to create triangle shading buffer, error " +
                std::to_string(err));
        }
    }

    if (!sceneData.materials.empty()) {
        newBuffers.materials = clCreateBuffer(
            m_oclcontext,
            CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
            sceneData.materials.size() * sizeof(fgt_material_data),
            const_cast<fgt_material_data*>(sceneData.materials.data()),
            &err);
        if (err != CL_SUCCESS || !newBuffers.materials) {
            releaseRaySpaceBuffer(newBuffers);
            throw std::runtime_error(
                "uploadScene: failed to create material buffer, error " +
                std::to_string(err));
        }
    }

    if (!sceneData.bvh_nodes.empty()) {
        newBuffers.bvhNodes = clCreateBuffer(
            m_oclcontext,
            CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
            sceneData.bvh_nodes.size() * sizeof(fgt_bvh_node),
            const_cast<fgt_bvh_node*>(sceneData.bvh_nodes.data()),
            &err);
        if (err != CL_SUCCESS || !newBuffers.bvhNodes) {
            releaseRaySpaceBuffer(newBuffers);
            throw std::runtime_error(
                "uploadScene: failed to create BVH buffer, error " +
                std::to_string(err));
        }
    }

    if (!emissiveIndices.empty()) {
        newBuffers.emissiveTriangles = clCreateBuffer(
            m_oclcontext,
            CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
            emissiveIndices.size() * sizeof(fgt_int32),
            emissiveIndices.data(),
            &err);
        if (err != CL_SUCCESS || !newBuffers.emissiveTriangles) {
            releaseRaySpaceBuffer(newBuffers);
            throw std::runtime_error(
                "uploadScene: failed to create emissive triangle buffer, error " +
                std::to_string(err));
        }
    }

    releaseRaySpaceBuffer(m_raySpaceBuffer);
    m_raySpaceBuffer = newBuffers;
    m_sceneUploaded = true;

    std::cout << "OpenCL RaySpace scene uploaded: "
              << m_raySpaceBuffer.numTriangles << " triangles, "
              << m_raySpaceBuffer.numMaterials << " materials, "
        << m_raySpaceBuffer.numBVHNodes << " BVH nodes, " << std::endl;
}

void OpenCL_Renderer::loadSceneLigths(const std::vector<Light> &lights)
{
    cl_int err = CL_SUCCESS;
    std::vector<fgt_light> scene_lights = fgt::opencl::load_scene_lights(lights);

    if (m_raySpaceBuffer.lights) {
        clReleaseMemObject(m_raySpaceBuffer.lights);
        m_raySpaceBuffer.lights = nullptr;
    }

    m_raySpaceBuffer.numLights = scene_lights.size();
    if (!scene_lights.empty()) {
        m_raySpaceBuffer.lights = clCreateBuffer(
            m_oclcontext,
            CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
            scene_lights.size() * sizeof(fgt_light),
            const_cast<fgt_light*>(scene_lights.data()),
            &err);
        if (err != CL_SUCCESS || !m_raySpaceBuffer.lights) {
            throw std::runtime_error(
                "loadSceneLigths: failed to create light buffer, error " +
                std::to_string(err));
        }
    }   
    std::cout << " Lights Loaded to the Scene : " << m_raySpaceBuffer.numLights<< std::endl;
    if (!scene_lights.empty()) {
        std::cout << " [DEBUG] Color intensity : " << scene_lights[0].intensity.x << " , " << scene_lights[0].intensity.y << " ," << scene_lights[0].intensity.z << std::endl;
    }
}

void OpenCL_Renderer::updateSceneShading(
    const std::vector<Triangle>& triangles)
{
    if (!m_oclcontext || !m_sceneUploaded) {
        throw std::runtime_error(
            "updateSceneShading: OpenCL scene is not uploaded.");
    }
    if (triangles.size() != m_raySpaceBuffer.numTriangles) {
        throw std::invalid_argument(
            "updateSceneShading: triangle count changed.");
    }

    const fgt::opencl::fgt_opencl_shading_data shadingData =
        fgt::opencl::translate_shading_data(triangles);

    cl_mem newTriangleShading = nullptr;
    cl_mem newMaterials = nullptr;
    cl_mem newEmissiveTriangles = nullptr;
    cl_int err = CL_SUCCESS;

    if (!shadingData.triangle_shading.empty()) {
        newTriangleShading = clCreateBuffer(
            m_oclcontext,
            CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
            shadingData.triangle_shading.size() * sizeof(fgt_triangle_shading),
            const_cast<fgt_triangle_shading*>(
                shadingData.triangle_shading.data()),
            &err);
        if (err != CL_SUCCESS || !newTriangleShading) {
            throw std::runtime_error(
                "updateSceneShading: failed to create triangle shading buffer, error " +
                std::to_string(err));
        }
    }

    if (!shadingData.materials.empty()) {
        newMaterials = clCreateBuffer(
            m_oclcontext,
            CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
            shadingData.materials.size() * sizeof(fgt_material_data),
            const_cast<fgt_material_data*>(shadingData.materials.data()),
            &err);
        if (err != CL_SUCCESS || !newMaterials) {
            if (newTriangleShading) {
                clReleaseMemObject(newTriangleShading);
            }
            throw std::runtime_error(
                "updateSceneShading: failed to create material buffer, error " +
                std::to_string(err));
        }
    }

    if (!shadingData.emissive_triangles.empty()) {
        newEmissiveTriangles = clCreateBuffer(
            m_oclcontext,
            CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
            shadingData.emissive_triangles.size() * sizeof(fgt_int32),
            const_cast<fgt_int32*>(shadingData.emissive_triangles.data()),
            &err);
        if (err != CL_SUCCESS || !newEmissiveTriangles) {
            if (newMaterials) {
                clReleaseMemObject(newMaterials);
            }
            if (newTriangleShading) {
                clReleaseMemObject(newTriangleShading);
            }
            throw std::runtime_error(
                "updateSceneShading: failed to create emissive triangle buffer, error " +
                std::to_string(err));
        }
    }

    if (m_raySpaceBuffer.emissiveTriangles) {
        clReleaseMemObject(m_raySpaceBuffer.emissiveTriangles);
    }
    if (m_raySpaceBuffer.materials) {
        clReleaseMemObject(m_raySpaceBuffer.materials);
    }
    if (m_raySpaceBuffer.triangleShading) {
        clReleaseMemObject(m_raySpaceBuffer.triangleShading);
    }

    m_raySpaceBuffer.triangleShading = newTriangleShading;
    m_raySpaceBuffer.materials = newMaterials;
    m_raySpaceBuffer.emissiveTriangles = newEmissiveTriangles;
    m_raySpaceBuffer.numMaterials = shadingData.materials.size();
    m_raySpaceBuffer.numEmissiveTriangles =
        shadingData.emissive_triangles.size();
}

void OpenCL_Renderer::releaseRaySpaceBuffer(
    OpenCLRaySpaceBuffer& buffers) noexcept
{
    if (buffers.emissiveTriangles) {
        clReleaseMemObject(buffers.emissiveTriangles);
    }
    if (buffers.lights) {
        clReleaseMemObject(buffers.lights);
    }
    if (buffers.bvhNodes) {
        clReleaseMemObject(buffers.bvhNodes);
    }
    if (buffers.materials) {
        clReleaseMemObject(buffers.materials);
    }
    if (buffers.triangleShading) {
        clReleaseMemObject(buffers.triangleShading);
    }
    if (buffers.triangleGeometry) {
        clReleaseMemObject(buffers.triangleGeometry);
    }

    buffers = {};
}

void OpenCL_Renderer::prepareTextures()
{
    if (!m_textureManager) {
        throw std::runtime_error(
            "OpenCL textures requested before OpenCL initialization.");
    }

    if (m_textureManager->handlesAreDirty() ||
        !m_texturesObj || !m_textureDimensions) {
        setOpenCLTextures(
            m_textureManager->getBindlessHandles(),
            m_textureManager->getTextureDimensions());
        m_textureManager->markHandlesClean();
    }
    if (!m_oclrenderKernel && m_textureManager->getTextureCount() > 0) {
        std::cout << "[prepareTextures] Building kernel AFTER textures loaded" << std::endl;
        buildOCLPrograms();
    }
}

void OpenCL_Renderer::setOpenCLTextures(
    const std::vector<uint64_t>& handles,
    const std::vector<std::array<cl_int, 2>>& dimensions)
{
    static_assert(sizeof(std::array<cl_int, 2>) == sizeof(cl_int) * 2);
    if (handles.size() != dimensions.size()) {
        throw std::invalid_argument(
            "setOpenCLTextures: handle and dimension counts do not match.");
    }
    if (handles.empty()) {
        return;
    }
    if (m_textureDimensions) {
        clReleaseMemObject(m_textureDimensions);
    }
    if (m_texturesObj) {
        clReleaseMemObject(m_texturesObj);
    }
    cl_int err = CL_SUCCESS;
    m_texturesObj = clCreateBuffer(
        m_oclcontext,
        CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
        handles.size() * sizeof(uint64_t),
        const_cast<uint64_t*>(handles.data()),
        &err);
    if (err != CL_SUCCESS || !m_texturesObj) {
        throw std::runtime_error(
            "setOpenCLTextures: failed to create handle buffer, error " +
            std::to_string(err));
    }

    m_textureDimensions = clCreateBuffer(
        m_oclcontext,
        CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
        dimensions.size() * sizeof(std::array<cl_int, 2>),
        const_cast<std::array<cl_int, 2>*>(dimensions.data()),
        &err);
    if (err != CL_SUCCESS || !m_textureDimensions) {
        clReleaseMemObject(m_texturesObj);
        throw std::runtime_error(
            "setOpenCLTextures: failed to create dimension buffer, error " +
            std::to_string(err));
    }
    m_numTextures = handles.size();
}

void OpenCL_Renderer::RenderSceneOpenGLInterop(int width, 
    int height, 
    const std::vector<Triangle>& triangles, 
    const std::vector<BVHNode>& nodes, 
    const std::vector<Light>& lights, 
    const std::vector<int>& emissiveTriIndices, 
    const PBRCamera& camera, int samplesPerPixel, 
    int sampleOffset, 
    GLuint glBufferID,
    bool sceneShadingDirty)
{
    if (width <= 0 || height <= 0) {
        throw std::invalid_argument(
            "OpenCL RenderSceneOpenGLInterop requires positive image dimensions.");
    }
    if (samplesPerPixel <= 0) {
        throw std::invalid_argument(
            "OpenCL RenderSceneOpenGLInterop requires at least one sample.");
    }
    if (!m_oclcontext || !m_oclqueue) {
        throw std::runtime_error(
            "OpenCL RenderSceneOpenGLInterop called before renderer initialization.");
    }
    if (glBufferID == 0) {
        throw std::invalid_argument(
            "OpenCL RenderSceneOpenGLInterop requires a valid OpenGL buffer.");
    }

    prepareTextures();

    if (!m_sceneUploaded) {
        uploadScene(triangles, nodes, emissiveTriIndices);
        clFinish(m_oclqueue);
    }

    // Update material/emission buffers only when the scene changed.
    if (sceneShadingDirty) {
        updateSceneShading(triangles);
        clFinish(m_oclqueue);
    }

    // Update lights when a new accumulation starts.
    if (sampleOffset == 0 || !m_raySpaceBuffer.lights) {
        loadSceneLigths(lights);
        clFinish(m_oclqueue);
    }


    if (!m_oglSharedBuffer ||
        m_oglBufferID != glBufferID ||
        m_oglBufferWidth != width ||
        m_oglBufferHeight != height) {
        if (m_oglSharedBuffer) {
            clReleaseMemObject(m_oglSharedBuffer);
            m_oglSharedBuffer = nullptr;
            m_oglBufferID = 0;
            m_oglBufferWidth = 0;
            m_oglBufferHeight = 0;
        }
        cl_int err = CL_SUCCESS;
        m_oglSharedBuffer = clCreateFromGLBuffer(
            m_oclcontext,
            CL_MEM_WRITE_ONLY,
            glBufferID,
            &err);
        if (err != CL_SUCCESS || !m_oglSharedBuffer) {
            m_oglSharedBuffer = nullptr;
            throw std::runtime_error(
                "Failed to register OpenGL buffer with OpenCL, error " +
                std::to_string(err));
        }
        m_oglBufferID = glBufferID;
        m_oglBufferWidth = width;
        m_oglBufferHeight = height;
    }
    //Get camera data
    const fgt_rayspace_camera cameraData =
        fgt::opencl::translate_rayspace_camera(camera);
    
    const std::size_t imageSize =
        static_cast<std::size_t>(width) * static_cast<std::size_t>(height);

    cl_int err = CL_SUCCESS;
    bool accumulationBufferCreated = false;
    if (!m_oclAccumulationBuffer ||
        m_accumulationWidth != width ||
        m_accumulationHeight != height) {
        if (m_oclAccumulationBuffer) {
            clReleaseMemObject(m_oclAccumulationBuffer);
            m_oclAccumulationBuffer = nullptr;
        }

        m_oclAccumulationBuffer = clCreateBuffer(
            m_oclcontext,
            CL_MEM_READ_WRITE,
            imageSize * sizeof(fgt_vec4),
            nullptr,
            &err);
        if (err != CL_SUCCESS || !m_oclAccumulationBuffer) {
            throw std::runtime_error(
                "OpenCL RenderSceneOpenGLInterop failed to create accumulation buffer, error " +
                std::to_string(err));
        }
        m_accumulationWidth = width;
        m_accumulationHeight = height;
        accumulationBufferCreated = true;
    }

    // Clear the sum when a new accumulation starts.
    if (accumulationBufferCreated || sampleOffset == 0) {
        const fgt_vec4 zeroPixel{0.0f, 0.0f, 0.0f, 0.0f};
        err = clEnqueueFillBuffer(
            m_oclqueue,
            m_oclAccumulationBuffer,
            &zeroPixel,
            sizeof(zeroPixel),
            0,
            imageSize * sizeof(fgt_vec4),
            0,
            nullptr,
            nullptr);
        if (err != CL_SUCCESS) {
            throw std::runtime_error(
                "OpenCL RenderSceneOpenGLInterop failed to clear accumulation buffer, error " +
                std::to_string(err));
        }
    }

    cl_mem cameraBuffer = clCreateBuffer(
        m_oclcontext,
        CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
        sizeof(fgt_rayspace_camera),
        const_cast<fgt_rayspace_camera*>(&cameraData),
        &err);
    if (err != CL_SUCCESS || !cameraBuffer) {
        throw std::runtime_error(
            "OpenCL RenderSceneOpenGLInterop failed to create camera buffer, error " +
            std::to_string(err));
    }

    const cl_int numTriangles = static_cast<cl_int>(m_raySpaceBuffer.numTriangles);
    const cl_int numMaterials = static_cast<cl_int>(m_raySpaceBuffer.numMaterials);
    const cl_int numBVHNodes = static_cast<cl_int>(m_raySpaceBuffer.numBVHNodes);
    const cl_int numLights = static_cast<cl_int>(m_raySpaceBuffer.numLights);
    const cl_int numEmissiveTriangles = static_cast<cl_int>(m_raySpaceBuffer.numEmissiveTriangles);
    const cl_int numTextures = static_cast<cl_int>(m_numTextures);
    cl_uint argument = 0;
    setKernelArg(m_oclrenderKernel, argument, sizeof(cl_mem), &m_oclAccumulationBuffer);
    setKernelArg(m_oclrenderKernel, argument, sizeof(cl_mem), &m_raySpaceBuffer.triangleGeometry);
    setKernelArg(m_oclrenderKernel, argument, sizeof(cl_mem), &m_raySpaceBuffer.triangleShading);
    setKernelArg(m_oclrenderKernel, argument, sizeof(cl_mem), &m_raySpaceBuffer.materials);
    setKernelArg(m_oclrenderKernel, argument, sizeof(cl_mem), &m_raySpaceBuffer.bvhNodes);
    setKernelArg(m_oclrenderKernel, argument, sizeof(cl_mem), &m_raySpaceBuffer.lights);
    setKernelArg(m_oclrenderKernel, argument, sizeof(cl_mem), &m_raySpaceBuffer.emissiveTriangles);
    setKernelArg(m_oclrenderKernel, argument, sizeof(cl_mem), &m_texturesObj);
    setKernelArg(m_oclrenderKernel, argument, sizeof(cl_mem), &m_textureDimensions);
    setKernelArg(m_oclrenderKernel, argument, sizeof(cl_int), &numTriangles);
    setKernelArg(m_oclrenderKernel, argument, sizeof(cl_int), &numMaterials);
    setKernelArg(m_oclrenderKernel, argument, sizeof(cl_int), &numBVHNodes);
    setKernelArg(m_oclrenderKernel, argument, sizeof(cl_int), &numLights);
    setKernelArg(m_oclrenderKernel, argument, sizeof(cl_int), &numEmissiveTriangles);
    setKernelArg(m_oclrenderKernel, argument, sizeof(cl_int), &numTextures);
    setKernelArg(m_oclrenderKernel, argument, sizeof(cl_mem), &cameraBuffer);
    setKernelArg(m_oclrenderKernel, argument, sizeof(cl_int), &width);
    setKernelArg(m_oclrenderKernel, argument, sizeof(cl_int), &height);

    const std::size_t localSize[2] = {16, 16};
    const std::size_t globalSize[2] = {
        (static_cast<std::size_t>(width) + localSize[0] - 1) /
            localSize[0] * localSize[0],
        (static_cast<std::size_t>(height) + localSize[1] - 1) /
            localSize[1] * localSize[1]
    };

    // Add each new sample to the accumulation buffer.
    for (int sample = 0; sample < samplesPerPixel; ++sample) {
        const cl_int sampleIndex = sampleOffset + sample;
        err = clSetKernelArg(
            m_oclrenderKernel,
            argument,
            sizeof(sampleIndex),
            &sampleIndex);
        if (err != CL_SUCCESS) {
            clReleaseMemObject(cameraBuffer);
            throw std::runtime_error(
                "OpenCL RenderSceneOpenGLInterop failed to set sample index, error " +
                std::to_string(err));
        }

        err = clEnqueueNDRangeKernel(
            m_oclqueue,
            m_oclrenderKernel,
            2,
            nullptr,
            globalSize,
            localSize,
            0,
            nullptr,
            nullptr);
        if (err != CL_SUCCESS) {
            clReleaseMemObject(cameraBuffer);
            throw std::runtime_error(
                "OpenCL RenderSceneOpenGLInterop failed to enqueue path tracer, error " +
                std::to_string(err));
        }
    }

    clReleaseMemObject(cameraBuffer);

    // Acquire the shared GL buffer
    glFinish();
    err = clEnqueueAcquireGLObjects(
        m_oclqueue, 1, &m_oglSharedBuffer, 0, nullptr, nullptr);
    if (err != CL_SUCCESS) {
        throw std::runtime_error(
            "OpenCL failed to acquire the OpenGL buffer, error " +
            std::to_string(err));
    }

    // Copy and normalize from accumulation buffer to shared buffer
    const cl_int accumulatedSamples = sampleOffset + samplesPerPixel;
    const size_t pixelCount = static_cast<size_t>(width) * static_cast<size_t>(height);
    cl_uint arg = 0;
    setKernelArg(m_ocl_ogldisplayKernel, arg, sizeof(cl_mem), &m_oclAccumulationBuffer);
    setKernelArg(m_ocl_ogldisplayKernel, arg, sizeof(cl_mem), &m_oglSharedBuffer);
    setKernelArg(m_ocl_ogldisplayKernel, arg, sizeof(cl_int), &accumulatedSamples);

    err = clEnqueueNDRangeKernel(
        m_oclqueue, m_ocl_ogldisplayKernel, 1, nullptr,
        &pixelCount, nullptr, 0, nullptr, nullptr);
    if (err != CL_SUCCESS) {
        clEnqueueReleaseGLObjects(
            m_oclqueue, 1, &m_oglSharedBuffer, 0, nullptr, nullptr);
        clFinish(m_oclqueue);
        throw std::runtime_error(
            "OpenCL failed to run display kernel, error " + std::to_string(err));
    }

    err = clEnqueueReleaseGLObjects(
        m_oclqueue, 1, &m_oglSharedBuffer, 0, nullptr, nullptr);
    if (err != CL_SUCCESS) {
        throw std::runtime_error(
            "OpenCL failed to release the OpenGL buffer, error " +
            std::to_string(err));
    }
    err = clFinish(m_oclqueue);
    if (err != CL_SUCCESS) {
        throw std::runtime_error(
            "OpenCL failed while finishing interop commands, error " +
            std::to_string(err));
    }

}

void OpenCL_Renderer::buildOCLPrograms()
{
    if (!m_oclcontext) {
        throw std::runtime_error(
            "buildOCLPrograms: OpenCL context is not initialized.");
    }
    std::vector<const char*> source_pointers;
    std::vector<size_t> source_lengths;
    std::vector<std::string> file_contents;
    file_contents.reserve(OpenCLRendererSourceFiles.size());
    for (size_t i = 0; i < OpenCLRendererSourceFiles.size(); ++i) {
        const std::string& sourceFile = getAssetPath(OpenCLRendererSourceFiles[i]);
        std::ifstream file(sourceFile);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open OpenCL source file: " + sourceFile);
        }
        std::string buffer{
            std::istreambuf_iterator<char>(file),
            std::istreambuf_iterator<char>() };
        buffer.push_back('\n');
        file_contents.push_back(std::move(buffer));
    }
    for (const auto& content : file_contents) {
        source_pointers.push_back(content.c_str());
        source_lengths.push_back(content.size());
    }
    cl_int err = CL_SUCCESS;
    m_oclprogram = clCreateProgramWithSource(
        m_oclcontext,
        static_cast<cl_uint>(source_pointers.size()),
        source_pointers.data(),
        source_lengths.data(),
        &err);
    if (err != CL_SUCCESS || !m_oclprogram) {
        throw std::runtime_error("Failed to create OpenCL program, error " + std::to_string(err));
    }
    //The flags are set to enable bindless images and advanced bindless mode
    err = clBuildProgram(
        m_oclprogram,
        1,
        &m_ocldevice,
        "-cl-intel-use-bindless-images "
        "-cl-intel-use-bindless-advanced-mode",
        nullptr,
        nullptr);
    if (err != CL_SUCCESS) {
        std::size_t logSize = 0;
        clGetProgramBuildInfo(
            m_oclprogram,
            m_ocldevice,
            CL_PROGRAM_BUILD_LOG,
            0,
            nullptr,
            &logSize);

        std::string buildLog(logSize, '\0');
        if (logSize > 0) {
            clGetProgramBuildInfo(
                m_oclprogram,
                m_ocldevice,
                CL_PROGRAM_BUILD_LOG,
                logSize,
                buildLog.data(),
                nullptr);
        }

        clReleaseProgram(m_oclprogram);
        m_oclprogram = nullptr;
        throw std::runtime_error(
            "Failed to build OpenCL program, error " +
            std::to_string(err) + "\n" + buildLog);
    }

    m_oclrenderKernel = clCreateKernel(m_oclprogram,"path_tracer", &err);
    
    if (err != CL_SUCCESS || !m_oclrenderKernel) {
        m_oclrenderKernel = nullptr;
        clReleaseProgram(m_oclprogram);
        m_oclprogram = nullptr;
        throw std::runtime_error("Failed to create OpenCL path_tracer kernel, error " + std::to_string(err));
    }
    m_oclsampleKernel = clCreateKernel(m_oclprogram, "sample_framebuffer", &err);
    if (err != CL_SUCCESS || !m_oclsampleKernel) {
        m_oclsampleKernel = nullptr;
        clReleaseKernel(m_oclrenderKernel);
        m_oclrenderKernel = nullptr;
        clReleaseProgram(m_oclprogram);
        m_oclprogram = nullptr;
        throw std::runtime_error("Failed to create OpenCL sample_framebuffer, error " + std::to_string(err));
    }

    m_ocl_ogldisplayKernel = clCreateKernel(m_oclprogram, "present_framebuffer", &err);
    if (err != CL_SUCCESS || !m_ocl_ogldisplayKernel) {
        m_ocl_ogldisplayKernel = nullptr;
        clReleaseKernel(m_oclsampleKernel);
        m_oclsampleKernel = nullptr;
        clReleaseKernel(m_oclrenderKernel);
        m_oclrenderKernel = nullptr;
        clReleaseProgram(m_oclprogram);
        m_oclprogram = nullptr;
        throw std::runtime_error("Failed to create OpenCL present_framebuffer, error " + std::to_string(err));
    }

}

void OpenCL_Renderer::setKernelArg(
    cl_kernel kernel, 
    cl_uint& argIndex,
    size_t argSize, 
    const void* argValue)
{
    const cl_int argumentError =
        clSetKernelArg(kernel, argIndex++, argSize, argValue);
    if (argumentError != CL_SUCCESS) {
        throw std::runtime_error(
            "OpenCL RenderScene failed to set kernel argument " +
            std::to_string(argIndex - 1) + ", error " +
            std::to_string(argumentError));
    }
}
