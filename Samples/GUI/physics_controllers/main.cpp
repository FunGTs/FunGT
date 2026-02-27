#include "funGT/fungt.hpp"
#include <cstdlib>
#include <ctime>

const unsigned int SCREEN_WIDTH = 2100;
const unsigned int SCREEN_HEIGHT = 1200;

int main() {
    std::string path = findProjectRoot();
    std::cout << path << std::endl;
    srand(time(0));

    ModelPaths model_ball, model_lamp;
    model_lamp.path = getAssetPath("Obj/LuxoLamp/Luxo.obj");
    model_ball.path = getAssetPath("Obj/LuxoBall/luxoball.obj");

    DisplayGraphics::SetBackend(Backend::OpenGL);
    FunGTScene myGame = FunGT::createScene(SCREEN_WIDTH, SCREEN_HEIGHT);
    myGame->setBackgroundColor();
    myGame->initGL();

    myGame->createPhysicsWorld();
    spCollisionManager myCollision = myGame->getCollisionManager();
    myCollision->showCollidableBodies(false);

    auto ground = std::make_shared<RigidBody>(
        std::make_unique<Box>(80.0f, 1.0f, 80.0f), 0.0f
    );
    ground->m_pos = fungt::Vec3(0, -0.5f, 0.f);
    ground->m_restitution = 0.4f;
    ground->m_friction = 0.6f;
    myCollision->add(ground);

    auto leftWall = std::make_shared<RigidBody>(
        std::make_unique<Box>(7.0f, 16.0f, 7.0f), 0.0f
    );
    leftWall->m_pos = fungt::Vec3(0.0f, 8.0f, 0.f);
    leftWall->m_restitution = 0.7f;
    leftWall->m_friction = 0.3f;
    myCollision->add(leftWall);

    auto ball = std::make_shared<RigidBody>(
        std::make_unique<Sphere>(2.3), 1.0f
    );
    ball->m_pos = fungt::Vec3(0.0f, 2.3f, 15.0f);
    ball->m_vel = fungt::Vec3(0.0f, 0.0f, -15.0f);
  
    ball->m_angularVel = fungt::Vec3(-1.0, 0, 0);  // ≈ (6.52, 0, 0)

    ball->m_restitution = 0.8f;
    ball->m_friction = 0.3f;
    myCollision->add(ball);

    FunGTSModel ballModel = SimpleModel::create();
    ballModel->load(model_ball);
    ballModel->position(0.0f, 2.3f, 15.f);
    ballModel->scale(2.3f);
    ballModel->setAnimationID("ball");
    ballModel->addCollisionProperty(ball);

    FunGTSModel lamp = SimpleModel::create();
    lamp->load(model_lamp);
    lamp->position(0.f, 0.f, 0.f);
    lamp->setAnimationID("lamp");

    FunGTSGeom groundPlane = SimpleGeometry::create(Geometry::Plane);
    groundPlane->load(getAssetPath("img/floor.png"));
    groundPlane->position(0.0, 0.0, 0.0);

    //Back wall:

    FunGTSGeom backWall = SimpleGeometry::create(Geometry::Plane);
    backWall->load(getAssetPath("img/leftwall.png"));
    backWall->rotation(90.0f, 0.0f, 0.0f);
    backWall->position(0.0f, 0.0f, -40.0f);
    FunGTSGeom left_Wall = SimpleGeometry::create(Geometry::Plane);
    left_Wall->load(getAssetPath("img/leftwall.png"));
    left_Wall->rotation(90.0f, 0.0f, -90.0f);
    left_Wall->position(-40.0f, 0.0f, 0.0f);


    FunGTSceneManager scene_manager = myGame->getSceneManager();
    FunGTSimController simController = myGame->getSimulationController();

    simController->captureSnapshot(ball);

    myGame->set([&]() {
        scene_manager->addRenderableObj(groundPlane);
        scene_manager->addRenderableObj(backWall);
        scene_manager->addRenderableObj(left_Wall);
        scene_manager->addRenderableObj(ballModel);
        scene_manager->addRenderableObj(lamp);
        });

    auto animController = myGame->getAnimationController();

    // Enable baking for ball
    animController->setObjectBakingEnabled("ball", true);

    // Store window pointer (you need to add getAnimationWindow() to FunGT!)
    auto animWindow = myGame->getAnimationWindow();  // ← ADD THIS METHOD TO FUNGT!

    float lastTime = glfwGetTime();
    FunGTPhysicsWorld physics = myGame->getPhysicsWorld();

    myGame->render([&]() {
        float currentTime = glfwGetTime();
        float dt = currentTime - lastTime;
        lastTime = currentTime;

        int currentFrame = static_cast<int>(simController->getCurrentTime() * 30.0f);

        if (simController->isPlaying()) {
            float adjustedDt = dt * simController->getPlaybackSpeed();
  
            if (animWindow && animWindow->isRecordingMode()) {


                for (auto& obj : scene_manager->getRenderable()) {
                    if (auto model = std::dynamic_pointer_cast<SimpleModel>(obj)) {
                        model->setUsePhysicsRigidBody(true); // Ensure physics is enabled for all models (optional, based on your design)
                    }
                }

                physics->runColliders(adjustedDt);
                animController->recordPhysicsFrame(currentFrame);
            }
            else if (animWindow && animWindow->isPlaybackMode()) {
                // Disable physics for all
                for (auto& obj : scene_manager->getRenderable()) {
                    if (auto model = std::dynamic_pointer_cast<SimpleModel>(obj)) {
                        model->setUsePhysicsRigidBody(false);
                    }
                }
                animController->updateFrame(currentFrame);
            }

            simController->updateTime(dt);
        }

        scene_manager->renderScene();
        });

    return 0;
}