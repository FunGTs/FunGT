#include "../../funGT/fungt.hpp"

const unsigned int SCREEN_WIDTH = 2100;
const unsigned int SCREEN_HEIGHT = 1200;

int main() {
    std::string path = findProjectRoot();
    std::cout << "Project root: " << path << std::endl;

    std::string texture_path = getAssetPath("img/box.jpg");

    DisplayGraphics::SetBackend(Backend::OpenGL);

    FunGTScene myGame = FunGT::createScene(SCREEN_WIDTH, SCREEN_HEIGHT);
    myGame->setBackgroundColor(0.1f, 0.1f, 0.1f, 1.0f);
    myGame->initGL();

    // Get scene manager and info window
    FunGTSceneManager scene_manager = myGame->getSceneManager();
    FunGTInfoWindow infowindow = myGame->getInfoWindow();

    // Create a cube geometry using SimpleGeometry
    FunGTSGeom cubeGeom = SimpleGeometry::create(Geometry::Cube);
    cubeGeom->load(texture_path);

    // Position the cube in the middle of the screen
    cubeGeom->position(0.f, 0.f, -5.f);
    cubeGeom->scale(1.5f);

    // Add cube to scene manager
    myGame->set([&]() {
        scene_manager->addRenderableObj(cubeGeom);
    });

    myGame->render([&]() {
        // renderScene() handles everything: shader binding, matrix updates, drawing
        scene_manager->renderScene();
        infowindow->renderGUI();
    });

    return 0;
}