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

    std::shared_ptr<MeshRenderable> cube =
        std::make_shared<MeshRenderable>(createCubeMeshGeometry());
    std::shared_ptr<MeshRenderable> sphere =
        std::make_shared<MeshRenderable>(createSphereMeshGeometry());
    std::shared_ptr<MeshRenderable> plane =
        std::make_shared<MeshRenderable>(createPlaneMeshGeometry(12.0f, 12.0f));

    cube->position(-2.0f, -0.5f, 0.0f);
    sphere->position(2.0f, 0.0f, 0.0f);
    plane->position(0.0f, -1.0f, 0.0f);

    cube->load();
    sphere->load();
    plane->load();

    scene->set([&]() {
        sceneManager->addRenderableObj(cube);
        sceneManager->addRenderableObj(sphere);
        sceneManager->addRenderableObj(plane);
    });

    scene->render([&]() {
        sceneManager->renderScene();
    });

    return 0;
}
