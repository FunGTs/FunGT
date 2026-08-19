#include "opencl_renderer.hpp"

void OpenCL_Renderer::initialize()
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

    for (cl_platform_id platform : platforms) {
        if (!clGetExtensionFunctionAddressForPlatform(
                platform, "clCreateImageWithPropertiesINTEL")) {
            continue;
        }

        cl_device_id device = nullptr;
        err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, nullptr);
        if (err == CL_SUCCESS && device) {
            m_oclplatform = platform;
            m_ocldevice = device;
            break;
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

    m_oclcontext = clCreateContext(
        nullptr, 1, &m_ocldevice, nullptr, nullptr, &err);
    if (err != CL_SUCCESS || !m_oclcontext) {
        m_oclcontext = nullptr;
        throw std::runtime_error(
            "OpenCL initialization failed while creating the context, error " +
            std::to_string(err));
    }

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

    std::cout << "OpenCL context and command queue initialized." << std::endl;
}

OpenCL_Renderer::~OpenCL_Renderer()
{
    if (m_oclqueue) {
        clFinish(m_oclqueue);
    }

    m_textureManager.reset();

    if (m_texturesObj) {
        clReleaseMemObject(m_texturesObj);
        m_texturesObj = nullptr;
    }
    m_numTextures = 0;

    if (m_oclrenderKernel) {
        clReleaseKernel(m_oclrenderKernel);
        m_oclrenderKernel = nullptr;
    }

    if (m_oclprogram) {
        clReleaseProgram(m_oclprogram);
        m_oclprogram = nullptr;
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

std::vector<fungt::Vec3> OpenCL_Renderer::RenderScene(int width, int height, const std::vector<Triangle>& triangles, const std::vector<BVHNode>& nodes, const std::vector<Light>& lights, const std::vector<int>& emissiveTriIndices, const PBRCamera& camera, int samplesPerPixel, int sampleOffset)
{
    prepareTextures();
    return std::vector<fungt::Vec3>();
}

void OpenCL_Renderer::prepareTextures()
{
    if (!m_textureManager) {
        throw std::runtime_error(
            "OpenCL textures requested before OpenCL initialization.");
    }

    if (m_textureManager->handlesAreDirty()) {
        setOpenCLTextures(m_textureManager->getBindlessHandles());
        m_textureManager->markHandlesClean();
    }
}

void OpenCL_Renderer::setOpenCLTextures(const std::vector<uint64_t>& handles)
{
    if (!m_oclcontext) {
        throw std::runtime_error(
            "setOpenCLTextures: OpenCL context is not initialized.");
    }

    std::cout << "*** SETTING OPENCL TEXTURE OBJECTS*** " << std::endl;
    if (m_texturesObj) {
        clReleaseMemObject(m_texturesObj);
        m_texturesObj = nullptr;
    }

    m_numTextures = handles.size();
    std::cout << "*** NUM OPENCL TEXTURE OBJECTS *** " << m_numTextures << std::endl;

    if (m_numTextures == 0) {
        return;
    }

    cl_int err = CL_SUCCESS;
    m_texturesObj = clCreateBuffer(
        m_oclcontext,
        CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
        m_numTextures * sizeof(uint64_t),
        const_cast<uint64_t*>(handles.data()),
        &err);
    if (err != CL_SUCCESS || !m_texturesObj) {
        throw std::runtime_error("setOpenCLTextures: clCreateBuffer failed for bindless handles, error " + std::to_string(err));
    }

    std::cout << "  Uploaded " << m_numTextures << " bindless handles to GPU" << std::endl;
}
