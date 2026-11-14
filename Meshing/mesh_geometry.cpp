#include "mesh_geometry.hpp"

void (*MeshGeometry::s_gpuBuild)(MeshGeometry&, MeshGeomGPU*&) = nullptr;
void (*MeshGeometry::s_gpuFree)(MeshGeomGPU*) = nullptr;
void (*MeshGeometry::s_gpuDraw)(MeshGeomGPU&, const MeshGeometry&, Shader&) = nullptr;
