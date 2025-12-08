// THIS FILE IS COMPILED WITH SYCL COMPILER
#include "gpu_device_info.hpp"

#ifdef USE_SYCL
#include <sycl/sycl.hpp>

namespace fungt {

    class SyclBackend : public IGPUBackend {
    public:
        SyclBackend() {
            try {
                auto platforms = sycl::platform::get_platforms();
                for (const auto& platform : platforms) {
                    auto devices = platform.get_devices(sycl::info::device_type::gpu);
                    for (const auto& device : devices) {
                        sycl_devices_.push_back(device);
                    }
                }
            }
            catch (const sycl::exception&) {
                // No SYCL devices available
            }
        }

        std::vector<GPUDeviceInfo> getDevices() override {
            std::vector<GPUDeviceInfo> devices;

            for (size_t i = 0; i < sycl_devices_.size(); ++i) {
                const auto& device = sycl_devices_[i];

                GPUDeviceInfo info;
                info.id = static_cast<int>(i);
                info.name = device.get_info<sycl::info::device::name>();
                info.vendor = device.get_info<sycl::info::device::vendor>();
                info.memory_bytes = device.get_info<sycl::info::device::global_mem_size>();
                info.compute_units = device.get_info<sycl::info::device::max_compute_units>();
                info.backend = GPUBackend::SYCL;
                info.isActive = false;
                devices.push_back(info);
            }

            return devices;
        }

        GPUBackend getBackendType() override {
            return GPUBackend::SYCL;
        }

    private:
        std::vector<sycl::device> sycl_devices_;
    };

    // Factory function
    IGPUBackend* createSyclBackend() {
        return new SyclBackend();
    }

} // namespace fungt

#endif // USE_SYCL