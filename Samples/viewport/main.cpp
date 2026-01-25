#include "funGT/fungt.hpp"
const unsigned int SCREEN_WIDTH = 2100;
const unsigned int SCREEN_HEIGHT = 1200;



int main() {
    //std::string path = findProjectRoot();
    //Path to your shaders and models:
    ModelPaths model_ball, model_lamp;

    model_lamp.path = getAssetPath("Obj/LuxoLamp/Luxo.obj");
    model_ball.path = getAssetPath("Obj/LuxoBall/luxoball.obj");
    //Creates a FunGT Scene to display 
    FunGTScene myGame = FunGT::createScene(SCREEN_WIDTH, SCREEN_HEIGHT);
    //Background color, use 255.f for pure white, 
    myGame->setBackgroundColor();

    //Initializes the Graphics Stuff
    myGame->initGL();
    //Gets an instance of the SceneManager class to render objects
    FunGTSceneManager scene_manager = myGame->getSceneManager();
    // Creates an animation object
    FunGTSModel pixarLamp = SimpleModel::create();

    // Loads Pixar lamp data
    pixarLamp->load(model_lamp);
    pixarLamp->position(0.f, 0.f, 0.f);
    pixarLamp->rotation(0.f, 0.f, 0.f);

    FunGTSModel pixarBall = SimpleModel::create();

    pixarBall->load(model_ball);
    pixarBall->position(0.f, 0.f, 10.f);
    pixarBall->rotation(0.f, 0.f, 0.f);
    pixarBall->scale(2.3);

    //std::string ps_vs = getAssetPath("resources/particle.vs");
    //std::string ps_fs = getAssetPath("resources/particle.fs");
    //std::shared_ptr<ParticleSimulation> pSys = std::make_shared<ParticleSimulation>(10000, ps_vs, ps_fs);
    myGame->set([&]() { // Sets up all the scenes in your game
        // Adds the renderable objects to the SceneManager
                // Adds the renderable objects to the SceneManager
        scene_manager->addRenderableObj(pixarLamp);
        scene_manager->addRenderableObj(pixarBall);
        });
    myGame->render([&]() {

        });

    return 0;

}