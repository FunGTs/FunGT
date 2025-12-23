#include "funGT/fungt.hpp"
const unsigned int SCREEN_WIDTH = 2100;
const unsigned int SCREEN_HEIGHT = 1200;



int main (){
    //std::string path = findProjectRoot();
    //Path to your shaders and models:
    ModelPaths model_ball;
    //model.path   = getAssetPath("Animations/monster_dancing/monster_dancing.dae");
    model_ball.path = getAssetPath("Obj/Woody/woody-head.obj");

    //Creates a FunGT Scene to display 
    FunGTScene myGame = FunGT::createScene(SCREEN_WIDTH, SCREEN_HEIGHT);
    //Background color, use 255.f for pure white, 
    myGame->setBackgroundColor();
    // TEMP: Position camera to see grid
    //myGame->getCamera().m_vPos = glm::vec3(8.0f, 6.0f, 5.0f);
    // myGame->getCamera().m_pitch = -25.0f;
    // myGame->getCamera().m_yaw = -135.0f;
    // myGame->getCamera().updateVectors();
    //Initializes the Graphics Stuff
    myGame->initGL();
    //Gets an instance of the SceneManager class to render objects
    FunGTSceneManager scene_manager = myGame->getSceneManager();
    // Creates an animation object
    FunGTSModel pixarBall = SimpleModel::create();

    // Loads Pixar ball data
    pixarBall->load(model_ball);
    pixarBall->position(0.f, 0.f, 0.f);
    pixarBall->rotation(0.f, 0.f, 0.f);
    myGame->set([&]() { // Sets up all the scenes in your game
        // Adds the renderable objects to the SceneManager
                // Adds the renderable objects to the SceneManager
        scene_manager->addRenderableObj(pixarBall);
    });
    myGame->render([&](){

    });

    return 0; 
    
}