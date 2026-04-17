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
    std::shared_ptr<ParticleRTC> pRTC = std::make_shared<ParticleRTC>(10000);
    myGame->set([&]() { // Sets up all the scenes in your game
        // Adds the renderable objects to the SceneManager
                // Adds the renderable objects to the SceneManager
                
        scene_manager->addRenderableObj(pRTC);
        auto rtcWindow = std::make_unique<ParticleRTCWindow>(pRTC);
        myGame->getImGuiLayer().addWindow(std::move(rtcWindow));
    });
    myGame->render([&]() {

    });

    return 0;

}