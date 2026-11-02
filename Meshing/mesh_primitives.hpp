#if !defined(_MESHING_MESH_PRIMITIVES_HPP_)
#define _MESHING_MESH_PRIMITIVES_HPP_

#include <memory>

#include "mesh_geometry.hpp"

std::shared_ptr<MeshGeometry> createCubeMeshGeometry(float size = 1.0f);

#endif // _MESHING_MESH_PRIMITIVES_HPP_
