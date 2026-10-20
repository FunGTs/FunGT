#include "../../funGT/fungt.hpp"

const unsigned int SCREEN_WIDTH = 2100;
const unsigned int SCREEN_HEIGHT = 1200;

int main() {
    DisplayGraphics::SetBackend(Backend::OpenGL);

    FunGTScene myScene = FunGT::createScene(SCREEN_WIDTH, SCREEN_HEIGHT);
    myScene->setBackgroundColor();
    myScene->initGL();

    FunGTSceneManager scene_manager = myScene->getSceneManager();

    myScene->set([&]() {
        // No hardcoded models
        // Use File -> Open to load models
    });

    myScene->render([&]() {
        scene_manager->renderScene();
    });

    return 0;
}
