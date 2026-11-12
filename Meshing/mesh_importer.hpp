#if !defined(_MESH_IMPORTER_HPP_)
#define _MESH_IMPORTER_HPP_
#include <memory>
#include <string>

#include "mesh_renderable.hpp"
std::shared_ptr<MeshRenderable> importStaticMesh(const std::string& path);

#endif // _MESH_IMPORTER_HPP_
