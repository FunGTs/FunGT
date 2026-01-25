#include "funGT/fungt.hpp"
const unsigned int SCREEN_WIDTH = 2100;
const unsigned int SCREEN_HEIGHT = 1200;



int main (){
    //std::string path = findProjectRoot();
    //Path to your shaders and models:
    ModelPaths model_ball;
    //model.path   = getAssetPath("Animations/monster_dancing/monster_dancing.dae");
    model_ball.path = getAssetPath("Obj/LuxoLamp/Luxo.obj");

    //Creates a FunGT Scene to display 
    FunGTScene myGame = FunGT::createScene(SCREEN_WIDTH, SCREEN_HEIGHT);
    //Background color, use 255.f for pure white, 
    myGame->setBackgroundColor();

    myGame->initGL();
    //Gets an instance of the SceneManager class to render objects
    FunGTSceneManager scene_manager = myGame->getSceneManager();
    // Creates an animation object
    FunGTSGeom ground = SimpleGeometry::create(Geometry::Sphere);
    ground->load(getAssetPath("img/moon.jpg"));
    

    myGame->set([&]() { // Sets up all the scenes in your game
        // Adds the renderable objects to the SceneManager
                // Adds the renderable objects to the SceneManager
        scene_manager->addRenderableObj(ground);
    });
    myGame->render([&](){

    });

    return 0; 
    
}