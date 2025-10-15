#include "../funGT/fungt.hpp"
#include "../../Physics/PhysicsWorld/physics_world.hpp"
const unsigned int SCREEN_WIDTH = 2100;
const unsigned int SCREEN_HEIGHT = 1200;

int main(){
    std::string path = findProjectRoot();
    std::cout<<path<<std::endl;
    //Path to your shaders and models:
    ModelPaths model_ball, model_lamp, model_ground;
    
    //model.path   = getAssetPath("Animations/monster_dancing/monster_dancing.dae");
    model_ball.path   = getAssetPath("Obj/LuxoBall/luxoball.obj");
    model_ball.vs_path = getAssetPath("resources/luxoball_vs.glsl");
    model_ball.fs_path = getAssetPath("resources/luxoball_fs.glsl");

    model_lamp.path   = getAssetPath("Obj/LuxoLamp/Luxo.obj");
    model_lamp.vs_path = getAssetPath("resources/luxolamp_vs.glsl");
    model_lamp.fs_path = getAssetPath("resources/luxolamp_fs.glsl");


    
    //Creates a FunGT Scene to display 
    FunGTScene myGame = FunGT::createScene(SCREEN_WIDTH,SCREEN_HEIGHT);

    //Background color, use 255.f for pure white, 
    myGame->setBackgroundColor();

    //Initializes the Graphics Stuff
    myGame->initGL();

    //-------
    //Gets an instance of the SceneManager class to render objects
    FunGTSceneManager scene_manager  = myGame->getSceneManager();

    //Shows infowindow:

    FunGTInfoWindow infowindow = myGame->getInfoWindow();

    // Creates an animation object
    FunGTSModel pixarBall = SimpleModel::create();
   
    // Loads Pixar ball data
    pixarBall->load(model_ball);
    pixarBall->position(10.f, 1.f, -10.f);
    pixarBall->rotation(-30.f, 0.f, 0.f);
  
    
    myGame->set([&](){ // Sets up all the scenes in your game
        
        // Adds the renderable objects to the SceneManager
        scene_manager->addRenderableObj(pixarBall);

        
    });
    float lastTime = glfwGetTime();
    myGame->render([&](){ // Renders the entire scene using data from the SceneManager

        scene_manager->renderScene();
        infowindow->renderGUI();
    });

    //End of the game

    return 0;
}