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

    flib::sycl_handler::select_backend_device("OpenCL", "GPU");
    flib::sycl_handler::create_gl_interop_context();
    flib::sycl_handler::get_device_info();

    gpu::PhysicsWorld gpuPhysics;
    auto gpuCollision = gpuPhysics.getCollisionManager();

    // Ground
    gpuCollision->beginGroup();
    //gpuCollision->addBox(0, -0.5f, -15.f, 40, 1, 40, 0.0f);
    //gpuCollision->addBox(0, 2.0f, -15.f, 40, 1, 40, 0.0f);  // Y = 2 instead of -0.5
   // gpuCollision->addBox(0, -2.5f, -15.f, 40, 5, 40, 0.0f);  // adjust Y position
    gpuCollision->addBox(0, -3.0f, -15.f, 20, 1, 20, 0.0f);
    gpuCollision->endGroup();
    
    auto groundBox = std::make_shared<geometry::Box>(20.0f, 1.0f, 20.0f);
    auto ground = GPUGeometry::create(gpuCollision, GPUGeometryType::Box, groundBox);
    ground->load(getAssetPath("img/box.jpg"));
    std::cout << "GROUND: startIndex=" << ground->getStartIndex()
        << ", instanceCount=" << ground->getInstanceCount() << std::endl;
    // Balls
    gpuCollision->beginGroup();
    for (int i = 0; i < 20; i++) {
        float x = (rand() % 40) - 20.0f;
        float y = 5.0f + i * 2.0f;
        float z = -15.f;
        gpuCollision->addSphere(x, y, z, 1.0f, 1.0f);
    }
    gpuCollision->endGroup();
   
    // === DEBUG START ===
    std::cout << "\n=== DEBUG INFO ===" << std::endl;
    std::cout << "Total bodies: " << gpuCollision->getNumBodies() << std::endl;
    std::cout << "Total groups: " << gpuCollision->getNumGroups() << std::endl;

    std::cout << "\nGroups:" << std::endl;
    for (int g = 0; g < gpuCollision->getNumGroups(); g++) {
        auto grp = gpuCollision->getGroup(g);
        std::cout << "  Group " << g << ": startIndex=" << grp.startIndex << ", count=" << grp.count << std::endl;
    }
   

    auto balls = GPUGeometry::create(gpuCollision, GPUGeometryType::Sphere);
    balls->load(getAssetPath("img/moon.jpg"));
    std::cout << "BALLS: startIndex=" << balls->getStartIndex()
        << ", instanceCount=" << balls->getInstanceCount() << std::endl;
    FunGTSceneManager scene_manager = myGame->getSceneManager();
    FunGTInfoWindow infowindow = myGame->getInfoWindow();

    myGame->set([&]() {
        scene_manager->addRenderableObj(ground);
        scene_manager->addRenderableObj(balls);
        });

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