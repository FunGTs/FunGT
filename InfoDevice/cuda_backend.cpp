// THIS FILE IS COMPILED WITH g++, NOT SYCL COMPILER!
#include "gpu_device_info.hpp"

#ifdef USE_CUDA
#include <cuda_runtime.h>
#include <stdexcept>

namespace fungt {

    class CudaBackend : public IGPUBackend {
    public:
        CudaBackend() {
            cudaError_t err = cudaGetDeviceCount(&device_count_);
            if (err != cudaSuccess) {
                throw std::runtime_error("CUDA initialization failed");
            }
        }

        std::vector<GPUDeviceInfo> getDevices() override {
            std::vector<GPUDeviceInfo> devices;

            for (int i = 0; i < device_count_; ++i) {
                cudaDeviceProp prop;
                if (cudaGetDeviceProperties(&prop, i) == cudaSuccess) {
                    GPUDeviceInfo info;
                    info.id = i;
                    info.name = prop.name;
                    info.vendor = "NVIDIA";
                    info.memory_bytes = prop.totalGlobalMem;
                    info.compute_units = prop.multiProcessorCount;
                    info.backend = GPUBackend::CUDA;
                    info.isActive = false;
                    devices.push_back(info);
                }
            }

            return devices;
        }

        GPUBackend getBackendType() override {
            return GPUBackend::CUDA;
        }

    private:
        int device_count_ = 0;
    };

    // Factory function
    IGPUBackend* createCudaBackend() {
        return new CudaBackend();
    }

} // namespace fungt

#endif // USE_CUDA