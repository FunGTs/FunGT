#include "triangle_extractor.hpp"
#include <iostream>


// Extract triangles from ALL objects in SceneManager
void extractTriangles(
    SceneManager* sceneManager,
    Space &space)
{
    //std::vector<Triangle> allTriangles;

    if (!sceneManager) {
        std::cerr << "ERROR: SceneManager is null!" << std::endl;
        return;
    }

    const auto& renderables = sceneManager->getRenderable();

    std::cout << "Extracting triangles from " << renderables.size() << " objects..." << std::endl;

    for (const auto& obj : renderables) {
        // Try as SimpleModel
        auto model = std::dynamic_pointer_cast<SimpleModel>(obj);
        if (model) {
            space.LoadModelToRender(*model); // Ensure model is loaded into Space (for texture access)
            continue;
        }

        // Try as SimpleGeometry
        auto geom = std::dynamic_pointer_cast<SimpleGeometry>(obj);
        if (geom) {
            space.LoadGeometryToRender(*geom); // Ensure geometry is loaded into Space (for texture access)
        }
    }


}