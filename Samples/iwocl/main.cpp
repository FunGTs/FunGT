#include "funGT/fungt.hpp"
const unsigned int SCREEN_WIDTH = 2100;
const unsigned int SCREEN_HEIGHT = 1200;



int main() {
    //std::string path = findProjectRoot();
    //Path to your shaders and models:
    ModelPaths model_ball, model_lamp, model_monkey, iwocl;

    model_lamp.path = getAssetPath("Obj/LuxoLamp/Luxo.obj");
    // model_ball.path = getAssetPath("Obj/LuxoBall/luxoball.obj");
    model_monkey.path = getAssetPath("Obj/monkey/monkey.obj");
    iwocl.path = getAssetPath("Obj/iwocl/iwocl.obj");
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
    FunGTSModel pixarLamp = SimpleModel::create();

    // // Loads Pixar lamp data
    pixarLamp->load(model_lamp);
    pixarLamp->position(0.f, 0.f, -10.f);
    pixarLamp->rotation(0.f, 0.f, 0.f);

    // FunGTSModel pixarBall = SimpleModel::create();

    // pixarBall->load(model_ball);
    // pixarBall->position(0.f, 2.3f, 10.f);
    // pixarBall->rotation(0.f, 0.f, 0.f);
    // pixarBall->scale(2.3);
    FunGTSModel monkey = SimpleModel::create();
    monkey->load(model_monkey);
    monkey->position(-10.f, 1.f, 5.f);
    monkey->rotation(-40.f, 20.f, 0.f);
    monkey->scale(2.f);
    FunGTSGeom ground = SimpleGeometry::create(Geometry::Plane);
    ground->load(getAssetPath("img/floor.png"));
    ground->position(0.0,0.0,0.0);


    FunGTSGeom backWall = SimpleGeometry::create(Geometry::Plane);
    backWall->load(getAssetPath("img/wall.png"));
    backWall->rotation(90.0f, 0.0f, 0.0f);
    backWall->position(0.0f, 0.0f, -40.0f);
    FunGTSGeom left_Wall = SimpleGeometry::create(Geometry::Plane);
    left_Wall->load(getAssetPath("img/wall.png"));
    left_Wall->rotation(90.0f, 0.0f, -90.0f);
    left_Wall->position(-40.0f, 0.0f, 0.0f);
    // FunGTSGeom ball = SimpleGeometry::create(Geometry::Sphere);
    // ball->load(getAssetPath("img/ball.jpg"));
    // ball->position(0.f, 2.3f, 10.f);
    // ball->scale(2.3);
    myGame->set([&]() { // Sets up all the scenes in your game
        // Adds the renderable objects to the SceneManager
                // Adds the renderable objects to the SceneManager
        scene_manager->addRenderableObj(ground);
        scene_manager->addRenderableObj(backWall);
        scene_manager->addRenderableObj(left_Wall);
        scene_manager->addRenderableObj(iwocl_model);
        scene_manager->addRenderableObj(pixarLamp);
        //scene_manager->addRenderableObj(ball);
        scene_manager->addRenderableObj(monkey);
        });
    myGame->render([&]() {

        });

    return 0;

}