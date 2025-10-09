#include "../../funGT/fungt.hpp"
const unsigned int SCREEN_WIDTH = 2100;
const unsigned int SCREEN_HEIGHT = 1200;



int main (){
    std::string path = findProjectRoot();
    //Path to your shaders and models:
    ModelPaths model_ball;
    //model.path   = getAssetPath("Animations/monster_dancing/monster_dancing.dae");
    model_ball.path = getAssetPath("Obj/LuxoBall/luxoball.obj");
    model_ball.vs_path = getAssetPath("resources/luxoball_vs.glsl");
    model_ball.fs_path = getAssetPath("resources/luxoball_fs.glsl");

    //Creates a FunGT Scene to display 
    FunGTScene myGame = FunGT::createScene(SCREEN_WIDTH, SCREEN_HEIGHT);
    //Background color, use 255.f for pure white, 
    myGame->setBackgroundColor();
    //Initializes the Graphics Stuff
    myGame->initGL();

    //Gets an instance of the SceneManager class to render objects
    FunGTSceneManager scene_manager = myGame->getSceneManager();
    // Creates an animation object
    FunGTSModel pixarBall = SimpleModel::create();

    // Loads Pixar ball data
    pixarBall->load(model_ball);
    pixarBall->position(0.f, 1.f, -5.f);
    pixarBall->rotation(-30.f, 0.f, 0.f);
    myGame->set([&]() { // Sets up all the scenes in your game
        // Adds the renderable objects to the SceneManager
                // Adds the renderable objects to the SceneManager
        scene_manager->addRenderableObj(pixarBall);
    });
    myGame->render([&](){

        
    });

    return 0; 
    
}