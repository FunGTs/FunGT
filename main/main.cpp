#include "../funGT/fungt.hpp"

const unsigned int SCREEN_WIDTH = 1600;
const unsigned int SCREEN_HEIGHT = 1000;



int main(){

    //Path to yuor shaders:
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
    
    
    //Creates a FunGT object
    std::unique_ptr<FunGT> coreGame = std::make_unique<FunGT>(SCREEN_WIDTH,SCREEN_HEIGHT);

    //use 255.f for pure white, 
    coreGame->setBackgroundColor(255.f);
    coreGame->initGL();

    std::shared_ptr<SceneManager> scene_manager  = coreGame->getSceneManager();

    //scene_manager->loadShaders(cube.vs_path, cube.fs_path);
    // //scene_manager->LoadAnimModel(path);
    if(scene_manager==nullptr){
        std::cout<<"Invalid pointers "<<std::endl;
        exit(0);
    }else{
        std::cout<<"Good pointer"<<std::endl;
    }
    // //load your animation and the corresponding shader:
    //shader->load(ModelConfig);


    FunGTCubeMap cube_map = CubeMap::create(); 
    
    cube_map->addData(cube_data);

    FunGTAnimation animation  = Animation::create();

    animation->load(model);
    

    coreGame->set([&](){ //Set all the scenes in your game

        
        scene_manager->addRenderableObj(animation);
        scene_manager->addRenderableObj(cube_map);
        
     
    });

  

    
    coreGame->render([&](){

        scene_manager->renderScene();
        //scene_manager->renderNodes(); 
    });

     

    return 0;
}