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

    std::shared_ptr<MeshGeometry> meshGeometry = createCubeMeshGeometry();
    std::shared_ptr<MeshRenderable> cube = std::make_shared<MeshRenderable>(meshGeometry);
    cube->load();

    scene->set([&]() {
        sceneManager->addRenderableObj(cube);
    });

    scene->render([&]() {
        sceneManager->renderScene();
    });

    return 0;
}
