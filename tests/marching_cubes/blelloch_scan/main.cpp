#include <iostream>
#include <vector>
#include <cstdint>
#include <cmath>
#include "gpu/mc_gpu.hpp"

bool test_blelloch_scan(int resolution)
{
    std::cout << "Testing resolution " << resolution << "x" << resolution << "x" << resolution << "...\n";

    MarchingCubesGPU mc;
    mc.createQueue("test_queue");
    mc.setResolution(resolution, resolution, resolution);
    mc.sampleScalarFieldInside(fungt::Vec3(-1.0f, -1.0f, -1.0f), fungt::Vec3(1.0f, 1.0f, 1.0f));
    mc.setIsovalue(0.0f);

    mc.createGridBuffer([](float x, float y, float z) {
        return std::sqrt(x*x + y*y + z*z) - 0.5f;
    });

    mc.classifyCells();

    std::vector<uint32_t> triCount = mc.getTriCountCPU();

    std::vector<uint32_t> expectedOffset(triCount.size());
    expectedOffset[0] = 0;
    for (size_t i = 1; i < triCount.size(); i++) {
        expectedOffset[i] = expectedOffset[i - 1] + triCount[i - 1];
    }

    mc.blelloch_scan();

    std::vector<uint32_t> gpuOffset = mc.getTriOffsetCPU();

    bool passed = true;
    int errors = 0;
    for (size_t i = 0; i < gpuOffset.size() && errors < 10; i++) {
        if (gpuOffset[i] != expectedOffset[i]) {
            std::cout << "  FAIL at " << i << ": expected " << expectedOffset[i] << " got " << gpuOffset[i] << "\n";
            passed = false;
            errors++;
        }
    }

    if (passed) {
        uint32_t totalTris = expectedOffset.back() + triCount.back();
        std::cout << "  PASS (cells=" << triCount.size() << ", triangles=" << totalTris << ")\n";
    }

    return passed;
}

int main()
{
    std::cout << "Blelloch Scan Test\n";
    std::cout << "==================\n\n";

    bool all_passed = true;

    all_passed &= test_blelloch_scan(8);
    all_passed &= test_blelloch_scan(16);
    all_passed &= test_blelloch_scan(32);
    all_passed &= test_blelloch_scan(64);

    std::cout << "\n";
    if (all_passed) {
        std::cout << "ALL TESTS PASSED\n";
    } else {
        std::cout << "SOME TESTS FAILED\n";
    }

    return all_passed ? 0 : 1;
}
