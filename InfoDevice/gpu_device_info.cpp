#include "gpu_device_info.hpp"
#include <iostream>
#ifdef FUNGT_USE_OPENCL
#include <CL/cl.h>
#endif

// ════════════════════════════════════════════════════════════════════════════
// GPU Device Manager Implementation
// ════════════════════════════════════════════════════════════════════════════

GPUDeviceManager::GPUDeviceManager()
    : active_device_index_(-1)
{
}

GPUDeviceManager::~GPUDeviceManager() {
}

void GPUDeviceManager::initialize() {
    backends_.clear();
    all_devices_.clear();

    std::cout << "Initializing GPU backends..." << std::endl;

    // Try to create CUDA backend
#ifdef FUNGT_USE_CUDA
    try {
        auto cuda_backend = std::unique_ptr<fungt::IGPUBackend>(fungt::createCudaBackend());
        if (cuda_backend) {
            auto devices = cuda_backend->getDevices();
            if (!devices.empty()) {
                std::cout << "CUDA backend initialized: " << devices.size() << " device(s)" << std::endl;
                all_devices_.insert(all_devices_.end(), devices.begin(), devices.end());
                backends_.push_back(std::move(cuda_backend));
            }
        }
    }
    catch (const std::exception& e) {
        std::cout << "CUDA backend failed: " << e.what() << std::endl;
    }
#else
    std::cout << "  CUDA backend disabled (compile with -DFUNGT_USE_CUDA=ON)" << std::endl;
#endif

    // Try to create SYCL backend
#ifdef FUNGT_USE_SYCL
    try {
        auto sycl_backend = std::unique_ptr<fungt::IGPUBackend>(fungt::createSyclBackend());
        if (sycl_backend) {
            auto devices = sycl_backend->getDevices();
            if (!devices.empty()) {
                std::cout << " SYCL backend initialized: " << devices.size() << " device(s)" << std::endl;
                all_devices_.insert(all_devices_.end(), devices.begin(), devices.end());
                backends_.push_back(std::move(sycl_backend));
            }
        }
    }
    catch (const std::exception& e) {
        std::cout << "SYCL backend failed: " << e.what() << std::endl;
    }
#else
    std::cout << "  SYCL backend disabled (compile with -DFUNGT_USE_SYCL=ON)" << std::endl;
#endif

    // Find OpenCL GPU devices.
#ifdef FUNGT_USE_OPENCL
    cl_uint platformCount = 0;
    if (clGetPlatformIDs(0, nullptr, &platformCount) == CL_SUCCESS) {
        std::vector<cl_platform_id> platforms(platformCount);
        clGetPlatformIDs(platformCount, platforms.data(), nullptr);

        for (cl_platform_id platform : platforms) {
            cl_uint deviceCount = 0;
            if (clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 0, nullptr, &deviceCount) != CL_SUCCESS) {
                continue;
            }

            std::vector<cl_device_id> devices(deviceCount);
            clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, deviceCount, devices.data(), nullptr);
            for (cl_uint i = 0; i < deviceCount; ++i) {
                char name[256] = {};
                char vendor[256] = {};
                cl_ulong memory = 0;
                cl_uint computeUnits = 0;
                clGetDeviceInfo(devices[i], CL_DEVICE_NAME, sizeof(name), name, nullptr);
                clGetDeviceInfo(devices[i], CL_DEVICE_VENDOR, sizeof(vendor), vendor, nullptr);
                clGetDeviceInfo(devices[i], CL_DEVICE_GLOBAL_MEM_SIZE, sizeof(memory), &memory, nullptr);
                clGetDeviceInfo(devices[i], CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(computeUnits), &computeUnits, nullptr);

                fungt::GPUDeviceInfo info;
                info.id = static_cast<int>(i);
                info.name = name;
                info.vendor = vendor;
                info.memory_bytes = static_cast<size_t>(memory);
                info.compute_units = static_cast<int>(computeUnits);
                info.backend = fungt::GPUBackend::OPENCL;
                info.isActive = false;
                all_devices_.push_back(std::move(info));
            }
        }
    }
#endif

    // Add OpenGL fallback device (always available)
    fungt::GPUDeviceInfo opengl_device;
    opengl_device.id = static_cast<int>(all_devices_.size());
    opengl_device.name = "OpenGL Rasterizer (CPU Fallback)";
    opengl_device.vendor = "System";
    opengl_device.memory_bytes = 0;
    opengl_device.compute_units = 0;
    opengl_device.backend = fungt::GPUBackend::OPENGL;
    opengl_device.isActive = false;
    all_devices_.push_back(opengl_device);

    std::cout << "OpenGL fallback available" << std::endl;

    // Set first device as active by default
    if (!all_devices_.empty()) {
        all_devices_[0].isActive = true;
        active_device_index_ = 0;
        std::cout << "\nActive device: " << all_devices_[0].name << std::endl;
    }

    std::cout << "\nTotal devices available: " << all_devices_.size() << std::endl;
}

const std::vector<fungt::GPUDeviceInfo>& GPUDeviceManager::getDevices() const {
    return all_devices_;
}

void GPUDeviceManager::setActiveDevice(int deviceIndex) {
    if (deviceIndex < 0 || deviceIndex >= static_cast<int>(all_devices_.size())) {
        std::cerr << "Invalid device index: " << deviceIndex << std::endl;
        return;
    }

    // Deactivate all devices
    for (auto& device : all_devices_) {
        device.isActive = false;
    }

    // Activate selected device
    all_devices_[deviceIndex].isActive = true;
    active_device_index_ = deviceIndex;

    std::cout << "Switched to device: " << all_devices_[deviceIndex].name << std::endl;
}

int GPUDeviceManager::getActiveDeviceIndex() const {
    return active_device_index_;
}
