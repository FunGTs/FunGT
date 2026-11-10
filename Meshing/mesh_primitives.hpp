#if !defined(_MESHING_MESH_PRIMITIVES_HPP_)
#define _MESHING_MESH_PRIMITIVES_HPP_

#include <memory>

#include "mesh_geometry.hpp"

class MeshRenderable;

enum class GeomMesh {
    Cube,
    Sphere,
    Plane,
    Torus
};

std::shared_ptr<MeshRenderable> createGeomMesh(GeomMesh geomMesh);

std::shared_ptr<MeshGeometry> createCubeMeshGeometry(float size = 1.0f);
std::shared_ptr<MeshGeometry> createPlaneMeshGeometry(float width = 2.0f, float depth = 2.0f);
std::shared_ptr<MeshGeometry> createSphereMeshGeometry(float radius = 1.0f,
                                                        int sectorCount = 36,
                                                        int stackCount = 18);
std::shared_ptr<MeshGeometry> createTorusMeshGeometry(float majorRadius = 0.8f,
                                                       float minorRadius = 0.3f,
                                                       int majorSegments = 48,
                                                       int minorSegments = 24);

#endif // _MESHING_MESH_PRIMITIVES_HPP_
