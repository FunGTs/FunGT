#include "funGT/fungt.hpp"
#include <cstdlib>
#include <ctime>

const unsigned int SCREEN_WIDTH = 2100;
const unsigned int SCREEN_HEIGHT = 1200;

int main() {
    std::string path = findProjectRoot();
    std::cout << path << std::endl;
    srand(time(0));  // Seed random number generator

    // Model paths
    ModelPaths model_ball, model_lamp;
    model_lamp.path = getAssetPath("Obj/LuxoLamp/Luxo.obj");
    model_ball.path = getAssetPath("Obj/LuxoBall/luxoball.obj");
    DisplayGraphics::SetBackend(Backend::OpenGL);
    FunGTScene myGame = FunGT::createScene(SCREEN_WIDTH, SCREEN_HEIGHT);
    myGame->setBackgroundColor();
    myGame->initGL();

    // Create physics world
    myGame->createPhysicsWorld();
    spCollisionManager myCollision = myGame->getCollisionManager();
    myCollision->showCollidableBodies(false);  // ← ADD THIS TO SHOW COLLIDABLE BODIES IN DEBUG RENDERER
    // Create ground
    auto ground = std::make_shared<RigidBody>(
        std::make_unique<Box>(80.0f, 1.0f, 80.0f),
        0.0f
    );
    ground->m_pos = fungt::Vec3(0, -0.5f, 0.f);
    ground->m_restitution = 0.4f;
    ground->m_friction = 0.4f;
    myCollision->add(ground);
    // Create left wall
    auto leftWall = std::make_shared<RigidBody>(
        std::make_unique<Box>(7.0f, 16.0f, 7.0f),  // thin (1.0), tall (10.0), deep (40.0)
        0.0f
    );
    leftWall->m_pos = fungt::Vec3(0.0f, 8.0f, 0.f);  // Left side, centered vertically
    leftWall->m_restitution = 0.7f;
    leftWall->m_friction = 0.3f;
    myCollision->add(leftWall);
    // Storage for ball
    std::shared_ptr<RigidBody> ball;
    FunGTSModel ballModel;

    // Create single ball rolling from right to left
    ball = std::make_shared<RigidBody>(
        std::make_unique<Sphere>(2.3),
        1.0f
    );
    ball->m_pos = fungt::Vec3(0.0f, 2.3f, 20.0f);  // Start ON the ground (y=0.5 = ground_top + radius)
    ball->m_vel = fungt::Vec3(0.0f, 0.0f, -15.0f);   // Move left (negative X)
    ball->m_angularVel = fungt::Vec3(0, 0, 5.0f);   // Roll in correct direction
    ball->m_restitution = 0.8f;
    ball->m_friction = 0.3f;
    myCollision->add(ball);

    // Create visual model
    ballModel = SimpleModel::create();
    ballModel->load(model_ball);
    ballModel->position(0.0f, 2.3f, 20.f);
    ballModel->scale(2.3f);  // Match physics radius
    ballModel->addCollisionProperty(ball);
    //Loads Pixar Lamp  data
    FunGTSModel lamp = SimpleModel::create();
    lamp->load(model_lamp);
    lamp->position(0.f, 0.f, 0.f);
    lamp->rotation(0.f, 0.f, 0.f);
    FunGTSGeom groundPlane = SimpleGeometry::create(Geometry::Plane);
    groundPlane->load(getAssetPath("img/floor.png"));
    groundPlane->position(0.0, 0.0, 0.0);
    FunGTSceneManager scene_manager = myGame->getSceneManager();
    // ============================================
  // CAPTURE INITIAL SNAPSHOTS FOR RESET
  // ============================================
    FunGTSimController simController = myGame->getSimulationController();
    simController->captureSnapshot(ball);
    //simController->captureSnapshot(lampBox);
    myGame->set([&]() {
        scene_manager->addRenderableObj(groundPlane);
        scene_manager->addRenderableObj(ballModel);
        scene_manager->addRenderableObj(lamp);
        });

    float lastTime = glfwGetTime();
    FunGTPhysicsWorld physics = myGame->getPhysicsWorld();
    myGame->render([&]() {
        float currentTime = glfwGetTime();
        float dt = currentTime - lastTime;
        lastTime = currentTime;
        // ONLY RUN PHYSICS WHEN PLAYING
        if (simController->isPlaying()) {
            float adjustedDt = dt * simController->getPlaybackSpeed();
            physics->runColliders(adjustedDt);
            simController->updateTime(dt);
        }
        scene_manager->renderScene();
    });

    return 0;
}