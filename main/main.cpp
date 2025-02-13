#include "../funGT/fungt.hpp"

const unsigned int SCREEN_WIDTH = 1600;
const unsigned int SCREEN_HEIGHT = 1000;

int main(){

    //Path to your shaders and models:
    ModelPaths model,cube_data;
    cube_data.vs_path   = "../resources/skybox.vs";
    cube_data.fs_path   = "../resources/skybox.fs";
    cube_data.data_path = {
        "../img/spacebox/bkg1_right.png",
        "../img/spacebox/bkg1_left.png",
        "../img/spacebox/bkg1_top.png",
        "../img/spacebox/bkg1_bot.png",
        "../img/spacebox/bkg1_front.png",
        "../img/spacebox/bkg1_back.png"
    };
    //model.path = "../Animations/SillyDancing/SillyDancing.dae"; 
    model.path = "../Animations/raptoid/scene.gltf"; 
    model.vs_path = "../resources/animation_vs.glsl"; 
    model.fs_path = "../resources/animation_fs.glsl"; 
    std::string vs_Path = "../resources/particle.vs";
    std::string fs_Path = "../resources/particle.fs";
    //Creates a FunGT Scene to display 
    FunGTScene myGame = FunGT::createScene(SCREEN_WIDTH,SCREEN_HEIGHT);

    //Background color, use 255.f for pure white, 
    myGame->setBackgroundColor();

    //Initializes the Graphics Stuff
    myGame->initGL();

    //Gets an instance of the SceneManager class to render objects
    FunGTSceneManager scene_manager  = myGame->getSceneManager();

    // ---- SYCL 
    size_t numParticles = 1000;

    // Creates a cube map object
    FunGTCubeMap cube_map = CubeMap::create(); 

    // Adds data to the cube map
    cube_map->addData(cube_data);

    // Creates an animation object
    FunGTAnimation animation = Animation::create();

    // Loads animation data
    animation->load(model);

    myGame->set([&](){ // Sets up all the scenes in your game

        // Adds the renderable objects Animation and CubeMap to the SceneManager
        scene_manager->addRenderableObj(animation);
        scene_manager->addRenderableObj(cube_map);
        // scene_manager->addRenderableObj(pSys);
    });

    myGame->render([&](){ // Renders the entire scene using data from the SceneManager
        scene_manager->renderScene();
    });



    return 0;
}