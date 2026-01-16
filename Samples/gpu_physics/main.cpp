#include "funGT/fungt.hpp"
#include "Physics/GPU/include/gpu_physics_world.hpp"
#include "Physics/GPU/include/gpu_renderable_geometry.hpp"
#include <cstdlib>
#include <ctime>

const unsigned int SCREEN_WIDTH = 2100;
const unsigned int SCREEN_HEIGHT = 1200;

int main() {
    std::string path = findProjectRoot();
    std::cout << path << std::endl;

    srand(time(0));

    DisplayGraphics::SetBackend(Backend::OpenGL);
    FunGTScene myGame = FunGT::createScene(SCREEN_WIDTH, SCREEN_HEIGHT);
    myGame->setBackgroundColor();
    myGame->initGL();

    // GPU PHYSICS
    gpu::PhysicsWorld gpuPhysics;
    auto gpuCollision = gpuPhysics.getCollisionManager();

    std::cout << "=== Creating GPU Physics Scene ===" << std::endl;

    // GROUP 0: Ground (1 body)
    std::cout << "Adding ground..." << std::endl;
    gpuCollision->beginGroup();
    gpuCollision->addBox(0, -0.5f, -15.f, 40, 1, 40, 0.0f);
    gpuCollision->endGroup();

    // GROUP 1: 100 Balls (100 bodies)
    std::cout << "Adding 100 balls..." << std::endl;
    gpuCollision->beginGroup();
    for (int i = 0; i < 100; i++) {
        float x = (rand() % 40) - 20.0f;
        float y = 5.0f + i * 2.0f;
        float z = -15.f;
        gpuCollision->addSphere(x, y, z, 1.0f, 1.0f);
    }
    gpuCollision->endGroup();

    std::cout << "Total bodies: " << gpuCollision->getNumBodies() << std::endl;
    std::cout << "Total groups: " << gpuCollision->getNumGroups() << std::endl;

    // GPU VISUALS
    std::cout << "Creating GPU geometry..." << std::endl;

    auto ground = GPUGeometry::create(gpuCollision, GPUGeometryType::Cube);
    ground->load(getAssetPath("textures/ground.png"));

    auto balls = GPUGeometry::create(gpuCollision, GPUGeometryType::Sphere);
    balls->load(getAssetPath("textures/moon.png"));

    std::cout << "GPU geometry created!" << std::endl;

    // SCENE SETUP
    FunGTSceneManager scene_manager = myGame->getSceneManager();
    FunGTInfoWindow infowindow = myGame->getInfoWindow();

    myGame->set([&]() {
        scene_manager->addRenderableObj(ground);
        scene_manager->addRenderableObj(balls);
    });

    // MAIN LOOP
    float lastTime = glfwGetTime();

    myGame->render([&]() {
        float currentTime = glfwGetTime();
        float dt = currentTime - lastTime;
        lastTime = currentTime;

        if (dt > 0.1f) dt = 0.1f;

        gpuPhysics.update(dt);
        scene_manager->renderScene();
        infowindow->renderGUI();
        });

    return 0;
}