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

    ModelPaths model_ball;
    model_ball.path = getAssetPath("Obj/LuxoBall/luxoball.obj");
    model_ball.vs_path = getAssetPath("resources/luxoball_vs.glsl");
    model_ball.fs_path = getAssetPath("resources/luxoball_fs.glsl");

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

    // Storage for balls
    std::vector<std::shared_ptr<RigidBody>> balls;
    std::vector<FunGTSModel> ballModels;

    // Create 5 balls at different positions
    
    float positions[10] = { -10.0f, -8.0f, -6.0f, -4.0f, -2.0f, 2.0f, 6.0f, 10.0f, 14.0f, 18.0f };
    for (int i = 0; i < 10; ++i) {
        // Create physics body
        auto ball = std::make_shared<RigidBody>(
            std::make_unique<Sphere>(1.0f),
            1.0f
        );
        ball->m_pos = fungt::Vec3(positions[i], 5.0f + i * 2.0f, -15.f);

        // Random horizontal velocity
        float randomVelX = ((rand() % 100) / 20.0f) - 2.5f;
        ball->m_vel = fungt::Vec3(randomVelX, 0.0f, 0.0f);

        // Angular velocity to match rolling motion
        ball->m_angularVel = fungt::Vec3(0, 0, -randomVelX);

        ball->m_restitution = 0.8f;
        ball->m_friction = 0.3f;

        balls.push_back(ball);
        myCollision->add(ball);

        // Create visual model
        FunGTSModel pixarBall = SimpleModel::create();
        pixarBall->load(model_ball);
        pixarBall->position(positions[i], 5.0f + i * 2.0f, -15.f);
        pixarBall->addCollisionProperty(ball);

        ballModels.push_back(pixarBall);
    }

    FunGTSceneManager scene_manager = myGame->getSceneManager();
    FunGTInfoWindow infowindow = myGame->getInfoWindow();

    myGame->set([&]() {
        for (auto& ballModel : ballModels) {
            scene_manager->addRenderableObj(ballModel);
        }
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