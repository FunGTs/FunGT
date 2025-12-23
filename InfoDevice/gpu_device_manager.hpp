#pragma once
#include "gpu_device_interface.h"
#include <memory>
#include <vector>

namespace fungt {

    class GPUDeviceManager {
    public:
        GPUDeviceManager();
        ~GPUDeviceManager();

        void initialize();
        std::vector<GPUDeviceInfo> getAllDevices();

    private:
        std::vector<std::unique_ptr<GPUBackend>> backends_;
    };

} // namespace fungt