#if !defined(_GPU_MEM_UTILS_HPP_)
#define _GPU_MEM_UTILS_HPP_
#include<iostream>
#include<funlib/funlib.hpp>
// Add to gpu_physics_kernel.hpp or a new gpu_memory_utils.hpp

namespace gpu {

    struct MemoryRequirements {
        size_t bodiesMemory;      // DeviceData arrays
        size_t gridMemory;        // UniformGrid arrays
        size_t contactsMemory;    // Contact pairs buffer
        size_t matricesMemory;    // Model matrices SSBO
        size_t totalMemory;
    };

    inline MemoryRequirements calculateMemoryRequirements(
        int maxBodies,
        float worldSize,    // e.g., 200 for -100 to +100
        float cellSize)
    {
        MemoryRequirements mem = {};

        // DeviceData: ~21 float arrays + invInertiaTensor (9 floats per body)
        int numFloatArrays = 21;
        mem.bodiesMemory = maxBodies * numFloatArrays * sizeof(float);
        mem.bodiesMemory += maxBodies * 9 * sizeof(float);  // inertia tensor
        mem.bodiesMemory += maxBodies * sizeof(int);         // shapeType

        // UniformGrid
        int gridDim = static_cast<int>(worldSize / cellSize);
        int totalCells = gridDim * gridDim * gridDim;
        mem.gridMemory = maxBodies * 2 * sizeof(int);        // cellIndex + sortedBodyIndex
        mem.gridMemory += totalCells * 2 * sizeof(int);      // cellStart + cellEnd

        // Contact pairs (worst case: every body touches every other)
        int maxPairs = (maxBodies * maxBodies) / 2;
        mem.contactsMemory = maxPairs * sizeof(int) * 2;     // pair indices

        // Model matrices for rendering
        mem.matricesMemory = maxBodies * 16 * sizeof(float);

        mem.totalMemory = mem.bodiesMemory + mem.gridMemory +
            mem.contactsMemory + mem.matricesMemory;

        return mem;
    }

    inline void printMemoryRequirements(const MemoryRequirements& mem) {
        auto toMB = [](size_t bytes) { return bytes / (1024.0 * 1024.0); };
        auto toKB = [](size_t bytes) { return bytes / 1024.0; };

        std::cout << "=== GPU Memory Requirements ===" << std::endl;
        std::cout << "  Bodies data:    " << toKB(mem.bodiesMemory) << " KB" << std::endl;
        std::cout << "  Uniform grid:   " << toKB(mem.gridMemory) << " KB" << std::endl;
        std::cout << "  Contact pairs:  " << toKB(mem.contactsMemory) << " KB" << std::endl;
        std::cout << "  Model matrices: " << toKB(mem.matricesMemory) << " KB" << std::endl;
        std::cout << "  TOTAL:          " << toMB(mem.totalMemory) << " MB" << std::endl;
    }

    inline bool checkGPUMemory(sycl::queue& queue, int maxBodies, float worldSize, float cellSize) {
        sycl::device dev = queue.get_device();
        size_t availableMem = dev.get_info<sycl::info::device::global_mem_size>();

        std::cout << "=== GPU Memory Info ===" << std::endl;
        std::cout << "  Device: " << dev.get_info<sycl::info::device::name>() << std::endl;
        std::cout << "  Available: " << (availableMem / 1024 / 1024) << " MB" << std::endl;

        MemoryRequirements mem = calculateMemoryRequirements(maxBodies, worldSize, cellSize);
        printMemoryRequirements(mem);

        // Use 80% as safe limit (leave room for system/other apps)
        size_t safeLimit = static_cast<size_t>(availableMem * 0.8);

        if (mem.totalMemory > safeLimit) {
            std::cerr << "ERROR: Requested " << (mem.totalMemory / 1024 / 1024)
                << " MB but only " << (safeLimit / 1024 / 1024)
                << " MB available (80% limit)" << std::endl;
            return false;
        }

        std::cout << "  Status: OK (" << (mem.totalMemory * 100 / availableMem) << "% usage)" << std::endl;
        return true;
    }

} // namespace gpu


#endif // _GPU_MEM_UTILS_HPP_
