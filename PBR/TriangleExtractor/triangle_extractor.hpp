#if !defined(_TRIANGLE_EXTRACTOR_H_)
#define _TRIANGLE_EXTRACTOR_H_

#include "Triangle/triangle.hpp"
#include "SimpleModel/simple_model.hpp"
#include "SimpleGeometry/simple_geometry.hpp"
#include "SceneManager/scene_manager.hpp"
#include "PBR/TextureManager/idevice_texture.hpp"
#include <vector>
#include <memory>

// Extract triangles from a SimpleModel (with current transform!)
std::vector<Triangle> extractTriangles(
    const SimpleModel& model,
    IDeviceTexture* textureManager);

// Extract triangles from a SimpleGeometry (with current transform!)
std::vector<Triangle> extractTriangles(
    const SimpleGeometry& geometry,
    IDeviceTexture* textureManager);

// Extract triangles from ALL objects in SceneManager
std::vector<Triangle> extractTriangles(
    SceneManager* sceneManager,
    IDeviceTexture* textureManager);

#endif // _TRIANGLE_EXTRACTOR_H_