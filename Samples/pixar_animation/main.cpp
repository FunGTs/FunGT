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
    //Physcis:

     // Create physics world
    std::unique_ptr<PhysicsWorld> physics = std::make_unique<PhysicsWorld>();
    
    // Create a sphere that can roll
    auto rigidball = std::make_unique<RigidBody>(
        std::make_unique<Sphere>(1.0f), // radius = 1
        1.0f 
    );
    rigidball->m_pos = fungt::Vec3(10.f, 1.f, -15.f);    
    rigidball->m_vel = fungt::Vec3(-10.0f, 0.0f, 0.0f);   
    rigidball->m_angularVel = fungt::Vec3(0,0 , -rigidball->m_vel.x); 
    rigidball->m_restitution = 0.9f;         
    rigidball->m_friction = 0.2f;

    // Create ground
    auto ground = std::make_unique<RigidBody>(
        std::make_unique<Box>(40.0f, 1.0f, 40.0f),
        0.0f  // Static
    );
    ground->m_pos = fungt::Vec3(0, -0.5f, 0);
    ground->m_restitution = 0.6f;
    ground->m_friction = 0.4f;

    // Create walls for bouncing
    auto leftWall = std::make_unique<RigidBody>(
        std::make_unique<Box>(1.0f, 10.0f, 20.0f),
        0.0f
    );
    leftWall->m_pos = fungt::Vec3(-6.0, 0.0, -20);
    leftWall->m_restitution = 0.7f;

    spCollisionManager myCollision = physics->getCollisionManager();
    myCollision->add(std::move(rigidball));
    myCollision->add(std::move(ground));
    myCollision->add(std::move(leftWall));

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
    //Loads Pixar Lamp  data
    FunGTSModel lamp = SimpleModel::create();
    lamp->load(model_lamp);
    lamp->position(-10.f, -0.5f, -20.f);
    lamp->rotation(0.f, 90, 0.f);
    lamp->scale(1.f);
    

    pixarBall->addCollisionProperty(myCollision); // Adds collision properties to the ball

    
    myGame->set([&](){ // Sets up all the scenes in your game
        
        // Adds the renderable objects to the SceneManager
        scene_manager->addRenderableObj(pixarBall);
        scene_manager->addRenderableObj(lamp);
        
    });
    float lastTime = glfwGetTime();
    myGame->render([&](){ // Renders the entire scene using data from the SceneManager
        float currentTime = glfwGetTime();
        float dt = currentTime - lastTime;
        lastTime = currentTime;

        // Clamp dt to avoid huge jumps
        //if (dt > 0.05f) dt = 0.05f; // Max step ~20 FPS
        physics->runColliders(dt);
        
        scene_manager->renderScene();
        infowindow->renderGUI();
    });

    //End of the game

    return 0;
}