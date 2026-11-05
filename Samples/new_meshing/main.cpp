#include "funGT/fungt.hpp"

const unsigned int SCREEN_WIDTH = 2100;
const unsigned int SCREEN_HEIGHT = 1200;

int main()
{
    DisplayGraphics::SetBackend(Backend::OpenGL);

    FunGTScene scene = FunGT::createScene(SCREEN_WIDTH, SCREEN_HEIGHT);
    scene->setBackgroundColor();
    scene->initGL();

    FunGTSceneManager sceneManager = scene->getSceneManager();

    scene->set([]() {
    });

    scene->render([&]() {
        sceneManager->renderScene();
    });

    return 0;
}
