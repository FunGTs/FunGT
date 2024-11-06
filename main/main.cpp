#include "../funGT/fungt.hpp"

const unsigned int SCREEN_WIDTH = 1600;
const unsigned int SCREEN_HEIGHT = 1000;



int main(){

    //Path to your shaders and models:
    ModelPaths model,cube_data;
    cube_data.vs_path   = "../resources/skybox.vs";
    cube_data.fs_path   = "../resources/skybox.fs";
    cube_data.data_path = {
        "../img/sky/right.jpg",
        "../img/sky/left.jpg",
        "../img/sky/top.jpg",
        "../img/sky/bottom.jpg",
        "../img/sky/front.jpg",
        "../img/sky/back.jpg"
    };
    model.path = "../Animations/SillyDancing/SillyDancing.dae"; 
    model.vs_path = "../resources/animation_material_vs.glsl"; 
    model.fs_path = "../resources/animation_material_fs.glsl"; 
    
    
    //Creates a FunGT Scene to display 
    FunGTScene myGame = FunGT::createScene(SCREEN_WIDTH,SCREEN_HEIGHT);

    //use 255.f for pure white, 
    myGame->setBackgroundColor(255.f);
    myGame->initGL();

    FunGTSceneManager scene_manager  = myGame->getSceneManager();

    FunGTCubeMap cube_map = CubeMap::create(); 
    
    cube_map->addData(cube_data);

    FunGTAnimation animation  = Animation::create();

    animation->load(model);
    

    myGame->set([&](){ //Set all the scenes in your game

        
        scene_manager->addRenderableObj(animation);
        scene_manager->addRenderableObj(cube_map);
        
     
    });
    myGame->render([&](){

        scene_manager->renderScene();
        //scene_manager->renderNodes(); 
    });

     

    return 0;
}