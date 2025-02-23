#include "../funGT/fungt.hpp"

const unsigned int SCREEN_WIDTH = 1600;
const unsigned int SCREEN_HEIGHT = 1000;

int main(){
    std::string path = findProjectRoot();
    std::cout<<path<<std::endl;
    //Path to your shaders and models:
    ModelPaths model, cube_data;
    cube_data.vs_path   = getAssetPath("resources/skybox.vs");
    cube_data.fs_path   = getAssetPath("resources/skybox.fs");
    cube_data.data_path = {
        getAssetPath("img/spaceboxred/bkg2_right.png"),
        getAssetPath("img/spaceboxred/bkg2_left.png"),
        getAssetPath("img/spaceboxred/bkg2_top.png"),
        getAssetPath("img/spaceboxred/bkg2_bot.png"),
        getAssetPath("img/spaceboxred/bkg2_front.png"),
        getAssetPath("img/spaceboxred/bkg2_back.png")
    };
    
    model.path   = getAssetPath("Animations/raptoid/scene.gltf");
    model.vs_path = getAssetPath("resources/animation_vs.glsl");
    model.fs_path = getAssetPath("resources/animation_fs.glsl");
    //Creates a FunGT Scene to display 
    FunGTScene myGame = FunGT::createScene(SCREEN_WIDTH,SCREEN_HEIGHT);

    //Background color, use 255.f for pure white, 
    myGame->setBackgroundColor();

    //Initializes the Graphics Stuff
    myGame->initGL();

    //Gets an instance of the SceneManager class to render objects
    FunGTSceneManager scene_manager  = myGame->getSceneManager();

    //Shows infowindow:

    FunGTInfoWindow infowindow = myGame->getInfoWindow();

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
        infowindow->renderGUI();
    });



    return 0;
}