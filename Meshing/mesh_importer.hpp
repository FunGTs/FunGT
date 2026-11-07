#if !defined(_MESH_IMPORTER_HPP_)
#define _MESH_IMPORTER_HPP_
#include <memory>
#include <string>
#include <vector>

#include "mesh_renderable.hpp"
std::vector<std::shared_ptr<MeshRenderable>> importStaticMesh(const std::string& path);

#endif // _MESH_IMPORTER_HPP_
