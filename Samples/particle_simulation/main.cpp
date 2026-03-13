#include "funGT/fungt.hpp"
const unsigned int SCREEN_WIDTH = 2100;
const unsigned int SCREEN_HEIGHT = 1200;



int main() {

     //Creates a FunGT Scene to display 
    FunGTScene myGame = FunGT::createScene(SCREEN_WIDTH, SCREEN_HEIGHT);
    //Background color, use 255.f for pure white, 
    myGame->setBackgroundColor();
    // TEMP: Position camera to see grid

    //Initializes the Graphics Stuff
    myGame->initGL();
    //Gets an instance of the SceneManager class to render objects
    FunGTSceneManager scene_manager = myGame->getSceneManager();

    std::string ps_vs = getAssetPath("resources/particle.vs");
    std::string ps_fs = getAssetPath("resources/particle.fs");
    std::shared_ptr<ParticleSimulation> pSys = std::make_shared<ParticleSimulation>(10000, ps_vs, ps_fs);
    myGame->set([&]() { // Sets up all the scenes in your game
        // Adds the renderable objects to the SceneManager
                // Adds the renderable objects to the SceneManager
        scene_manager->addRenderableObj(pSys);
        });
    myGame->render([&]() {

        });

    return 0;

}