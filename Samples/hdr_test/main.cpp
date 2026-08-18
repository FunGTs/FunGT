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
     skybox->buildHDR(getAssetPath("assets_local/hdri/small_empty_room_3_4k.hdr"));

     scene_manager->loadEnvironment(getAssetPath("assets_local/hdri/small_empty_room_3_4k.hdr"));
     scene_manager->setIBLIntensity(1.3f);

    ModelPaths nightStreet;
    nightStreet.path = getAssetPath("assets_local/scene_final/scene.gltf");

    FunGTSModel street = SimpleModel::create();
    street->load(nightStreet);

    myGame->set([&]() {
         
        scene_manager->addRenderableObj(skybox);
        scene_manager->addRenderableObj(street);
        });

    myGame->render([&]() {
        });

    return 0;
}
