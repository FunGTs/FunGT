#include "../funGT/fungt.hpp"
#include "../../Physics/PhysicsWorld/physics_world.hpp"
#include <cstdlib>
#include <ctime>

const unsigned int SCREEN_WIDTH = 2100;
const unsigned int SCREEN_HEIGHT = 1200;

int main() {
    std::string path = findProjectRoot();
    std::cout << path << std::endl;
    srand(time(0));  // Seed random number generator

    ModelPaths model_ball,model_lamp;
    model_ball.path = getAssetPath("Obj/LuxoBall/luxoball.obj");
    model_ball.vs_path = getAssetPath("resources/luxoball_vs.glsl");
    model_ball.fs_path = getAssetPath("resources/luxoball_fs.glsl");
    model_lamp.path = getAssetPath("Obj/LuxoLamp/Luxo.obj");
    model_lamp.vs_path = getAssetPath("resources/luxolamp_vs.glsl");
    model_lamp.fs_path = getAssetPath("resources/luxolamp_fs.glsl");
    DisplayGraphics::SetBackend(Backend::OpenGL);
    FunGTScene myGame = FunGT::createScene(SCREEN_WIDTH, SCREEN_HEIGHT);
    myGame->setBackgroundColor();
    myGame->initGL();

    // Create physics world
    std::unique_ptr<PhysicsWorld> physics = std::make_unique<PhysicsWorld>();
    spCollisionManager myCollision = physics->getCollisionManager();

    // Create ground
    auto ground = std::make_shared<RigidBody>(
        std::make_unique<Box>(40.0f, 1.0f, 40.0f),
        0.0f
    );
    ground->m_pos = fungt::Vec3(0, -0.5f, -15.f);
    ground->m_restitution = 0.4f;
    ground->m_friction = 0.4f;
    myCollision->add(ground);
    // Create left wall
    auto leftWall = std::make_shared<RigidBody>(
        std::make_unique<Box>(1.0f, 10.0f, 40.0f),  // thin (1.0), tall (10.0), deep (40.0)
        0.0f
    );
    leftWall->m_pos = fungt::Vec3(-1.0f, 4.5f, -15.f);  // Left side, centered vertically
    leftWall->m_restitution = 0.7f;
    leftWall->m_friction = 0.3f;
    myCollision->add(leftWall);
    // Storage for ball
    std::shared_ptr<RigidBody> ball;
    FunGTSModel ballModel;

    // Create single ball rolling from right to left
    ball = std::make_shared<RigidBody>(
        std::make_unique<Sphere>(1.0f),
        1.0f
    );
    ball->m_pos = fungt::Vec3(10.0f, 1.0f, -15.f);  // Start ON the ground (y=0.5 = ground_top + radius)
    ball->m_vel = fungt::Vec3(-10.0f, 0.0f, 0.0f);   // Move left (negative X)
    ball->m_angularVel = fungt::Vec3(0, 0, 5.0f);   // Roll in correct direction
    ball->m_restitution = 0.8f;
    ball->m_friction = 0.3f;
    myCollision->add(ball);

    // Create visual model
    ballModel = SimpleModel::create();
    ballModel->load(model_ball);
    ballModel->position(10.0f, 0.5f, -15.f);
    ballModel->addCollisionProperty(ball);
    //Loads Pixar Lamp  data
    FunGTSModel lamp = SimpleModel::create();
    lamp->load(model_lamp);
    lamp->position(-4.f, -0.5f, -20.f);
    lamp->rotation(0.f, 90, 0.f);
    lamp->scale(1.f);
    FunGTSceneManager scene_manager = myGame->getSceneManager();
    FunGTInfoWindow infowindow = myGame->getInfoWindow();

    myGame->set([&]() {
        scene_manager->addRenderableObj(ballModel);
        scene_manager->addRenderableObj(lamp);
        });

    float lastTime = glfwGetTime();
    myGame->render([&]() {
        float currentTime = glfwGetTime();
        float dt = currentTime - lastTime;
        lastTime = currentTime;

        physics->runColliders(dt);
        scene_manager->renderScene();
        infowindow->renderGUI();
        });

    return 0;
}