#if !defined(_TRIANGLE_EXTRACTOR_H_)
#define _TRIANGLE_EXTRACTOR_H_

#include "Triangle/triangle.hpp"
#include "SimpleModel/simple_model.hpp"
#include "SimpleGeometry/simple_geometry.hpp"
#include "SceneManager/scene_manager.hpp"
#include "PBR/Space/space.hpp"
#include <vector>
#include <memory>


// Extract triangles from ALL objects in SceneManager
void extractTriangles(
    SceneManager* sceneManager,
    Space &space);

#endif // _TRIANGLE_EXTRACTOR_H_