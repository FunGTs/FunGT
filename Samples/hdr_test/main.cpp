#include "funGT/fungt.hpp"

const unsigned int SCREEN_WIDTH = 1600;
const unsigned int SCREEN_HEIGHT = 1000;

int main() {

    DisplayGraphics::SetBackend(Backend::OpenGL);

    FunGTScene myGame = FunGT::createScene(SCREEN_WIDTH, SCREEN_HEIGHT);
    myGame->setBackgroundColor();
    myGame->initGL();

    FunGTSceneManager scene_manager = myGame->getSceneManager();

    FunGTCubeMap skybox = CubeMap::create();
     skybox->setShaders(getAssetPath("resources/equirect_skybox.vs"),
                         getAssetPath("resources/equirect_skybox.fs"));
     skybox->buildHDR(getAssetPath("assets_local/hdri/san_giuseppe_bridge_4k.hdr"));

     scene_manager->loadEnvironment(getAssetPath("assets_local/hdri/san_giuseppe_bridge_4k.hdr"));
     scene_manager->setIBLIntensity(1.3f);

    ModelPaths nightStreet;
    nightStreet.path = getAssetPath("assets_local/Obj/street/source/Wagon/#Ulica.obj");

    FunGTSModel street = SimpleModel::create();
    street->load(nightStreet);
    street->position(0.f, 0.f, -40.f);
    street->rotation(0.f, 0.f, 0.f);
    street->scale(1.0);

    myGame->set([&]() {
         scene_manager->addRenderableObj(skybox);
        scene_manager->addRenderableObj(street);
        });

    myGame->render([&]() {
        });

    return 0;
}
