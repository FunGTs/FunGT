#include "funGT/fungt.hpp"

const unsigned int SCREEN_WIDTH = 2100;
const unsigned int SCREEN_HEIGHT = 1200;

int main()
{

    //Set graphics backend

    DisplayGraphics::SetBackend(Backend::OpenGL);


    FunGTScene myScene = FunGT::createScene(SCREEN_WIDTH, SCREEN_HEIGHT);
    myScene->setBackgroundColor();
    myScene->initGL();

    FunGTSceneManager scene_manager = myScene->getSceneManager();

    FunGTSurface surface = Surface::create();

    surface->setField([](fungt::Vec3 p) -> float {
        return p.length() - 1.5f;
        });

    surface->sampleScalarFieldInside(
        fungt::Vec3(-2.f, -2.f, -2.f),
        fungt::Vec3(2.f, 2.f, 2.f)
    );
    surface->load();
    myScene->set([&]() {
        
        scene_manager->addRenderableObj(surface);
        });

    myScene->render([&]() {

        });

    return 0;
}