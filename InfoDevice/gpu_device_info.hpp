#ifndef _GPU_DEVICE_INFO_HPP_
#define _GPU_DEVICE_INFO_HPP_

#include <string>
#include <vector>
#include <memory>

namespace fungt {

    // ════════════════════════════════════════════════════════════════════════════
    // GPU Backend Types
    // ════════════════════════════════════════════════════════════════════════════
    enum class GPUBackend {
        CUDA,
        SYCL,
        OPENGL  // Fallback
    };

    // ════════════════════════════════════════════════════════════════════════════
    // Device Information (Plain C++ - no GPU headers!)
    // ════════════════════════════════════════════════════════════════════════════
    struct GPUDeviceInfo {
        int id;
        std::string name;
        std::string vendor;
        size_t memory_bytes;
        int compute_units;
        GPUBackend backend;
        bool isActive;

        // Helper to get formatted memory string for GUI
        std::string getMemoryString() const {
            double gb = memory_bytes / (1024.0 * 1024.0 * 1024.0);
            char buffer[32];
            snprintf(buffer, sizeof(buffer), "%.1f GB", gb);
            return std::string(buffer);
        }
        // Helper to get backend name as string
        std::string getBackendName() const {
            switch (backend) {
            case GPUBackend::CUDA:   return "CUDA";
            case GPUBackend::SYCL:   return "SYCL";
            case GPUBackend::OPENGL: return "OpenGL";
            default:                 return "Unknown";
            }
        }
    };

    // ════════════════════════════════════════════════════════════════════════════
    // Abstract Backend Interface
    // ════════════════════════════════════════════════════════════════════════════
    class IGPUBackend {
    public:
        virtual ~IGPUBackend() = default;
        virtual std::vector<GPUDeviceInfo> getDevices() = 0;
        virtual GPUBackend getBackendType() = 0;
    };

    // Factory functions (implemented in separate compilation units)
    IGPUBackend* createCudaBackend();
    IGPUBackend* createSyclBackend();

} // namespace fungt

// ════════════════════════════════════════════════════════════════════════════
// Device Manager (Used by GUI)
// ════════════════════════════════════════════════════════════════════════════
class GPUDeviceManager {
public:
    GPUDeviceManager();
    ~GPUDeviceManager();

    // Initialize all available backends
    void initialize();

    // Get all detected devices
    const std::vector<fungt::GPUDeviceInfo>& getDevices() const;

    // Set active device for rendering
    void setActiveDevice(int deviceIndex);

    // Get current active device
    int getActiveDeviceIndex() const;

private:
    std::vector<std::unique_ptr<fungt::IGPUBackend>> backends_;
    std::vector<fungt::GPUDeviceInfo> all_devices_;
    int active_device_index_;
};

#endif // _GPU_DEVICE_INFO_HPP_