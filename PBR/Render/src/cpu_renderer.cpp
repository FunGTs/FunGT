#include "PBR/Render/include/cpu_renderer.hpp"
#include "PBR/PBRCamera/pbr_camera.hpp"
#include "cpu_renderer.hpp"

std::vector<fungt::Vec3> CPU_Renderer::RenderScene(int width, int height, const std::vector<Triangle>& triangleList, const std::vector<gpu::TriangleGeometry>& hotTriangles, const std::vector<gpu::TriangleShadingData>& coldTriangles, const std::vector<BVHNode>& nodes, const std::vector<Light>& lightsList, const std::vector<int>& emissiveTriIndices, const PBRCamera& camera, int samplesPerPixel, int sampleOffset)
{
    return std::vector<fungt::Vec3>();
}
