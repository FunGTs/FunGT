#include "funGT/fungt.hpp"
const unsigned int SCREEN_WIDTH = 2100;
const unsigned int SCREEN_HEIGHT = 1200;



int main() {
    //std::string path = findProjectRoot();
    //Path to your models:
    ModelPaths iwocl;

    iwocl.path = getAssetPath("demo_assets/iwocl/iwocl.obj");
    //Creates a FunGT Scene to display 
    FunGTScene myGame = FunGT::createScene(SCREEN_WIDTH, SCREEN_HEIGHT);
    //Background color, use 255.f for pure white, 
    myGame->setBackgroundColor();

    //Initializes the Graphics Stuff
    myGame->initGL();
    //Gets an instance of the SceneManager class to render objects
    FunGTSceneManager scene_manager = myGame->getSceneManager();
    // Creates an animation object
    FunGTSModel iwocl_model = SimpleModel::create();
    iwocl_model->load(iwocl);
    iwocl_model->position(0.f, 0.f, 0.f);
    iwocl_model->rotation(0.f, 0.f, 0.f);
    iwocl_model->scale(5.0);
    
    FunGTSGeom ground = SimpleGeometry::create(Geometry::Plane);
    ground->load(getAssetPath("demo_assets/img/iwocl_floor.jpg"));
    ground->position(0.0,0.0,0.0);


    FunGTSGeom backWall = SimpleGeometry::create(Geometry::Plane);
    backWall->load(getAssetPath("demo_assets/img/iwocl_wall.jpg"));
    backWall->rotation(90.0f, 0.0f, 0.0f);
    backWall->position(0.0f, 0.0f, -40.0f);
    FunGTSGeom left_Wall = SimpleGeometry::create(Geometry::Plane);
    left_Wall->load(getAssetPath("demo_assets/img/iwocl_wall.jpg"));
    left_Wall->rotation(90.0f, 0.0f, -90.0f);
    left_Wall->position(-40.0f, 0.0f, 0.0f);
    FunGTSGeom right_Wall = SimpleGeometry::create(Geometry::Plane);
    right_Wall->load(getAssetPath("demo_assets/img/iwocl_wall.jpg"));
    right_Wall->rotation(-90.0f, 0.0f, 90.0f);
    right_Wall->position(40.0f, 0.0f, 0.0f);

    myGame->set([&]() { // Sets up all the scenes in your game
        // Adds the renderable objects to the SceneManager
                // Adds the renderable objects to the SceneManager
        scene_manager->addRenderableObj(ground);
        scene_manager->addRenderableObj(backWall);
        scene_manager->addRenderableObj(left_Wall);
        scene_manager->addRenderableObj(right_Wall);
        scene_manager->addRenderableObj(iwocl_model);
       
        });
    myGame->render([&]() {

        });

    return 0;

}