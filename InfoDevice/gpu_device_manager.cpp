#include "gpu_device_manager.h"

namespace fungt {

    GPUDeviceManager::GPUDeviceManager() {}
    GPUDeviceManager::~GPUDeviceManager() {}

    void GPUDeviceManager::initialize() {
        backends_.clear();

#ifdef FUNGT_USE_CUDA
        backends_.emplace_back(createCudaBackend());
#endif

#ifdef FUNGT_USE_SYCL
        backends_.emplace_back(createSyclBackend());
#endif
    }

    std::vector<GPUDeviceInfo> GPUDeviceManager::getAllDevices() {
        std::vector<GPUDeviceInfo> all_devices;

        for (auto& backend : backends_) {
            auto devices = backend->getDevices();
            all_devices.insert(all_devices.end(), devices.begin(), devices.end());
        }

        return all_devices;
    }

} // namespace fungt