#include "../funGT/fungt.hpp"
#include "../../Physics/PhysicsWorld/physics_world.hpp"
#include "../../Meshing/mesh_primitives.hpp"

const unsigned int SCREEN_WIDTH = 2100;
const unsigned int SCREEN_HEIGHT = 1200;

int main()
{
    DisplayGraphics::SetBackend(Backend::OpenGL);

    FunGTScene game =
        FunGT::createScene(SCREEN_WIDTH, SCREEN_HEIGHT);

    game->setBackgroundColor();
    game->initGL();
    game->createPhysicsWorld();

    auto sceneManager = game->getSceneManager();
    auto collisionManager = game->getCollisionManager();
    auto physicsWorld = game->getPhysicsWorld();

    /*
     * Visible procedural plane MeshRenderable.
     *
     * createPlaneMeshGeometry() creates a 2 x 2 plane.
     * Scale 50 makes it 100 x 100.
     */
    auto plane = createGeomMesh(GeomMesh::Plane);
    plane->scale(50.0f);
    plane->position(0.0f, -0.5f, 0.0f);
    plane->load();

    auto planeBody = std::make_shared<RigidBody>(
        std::make_unique<Box>(100.0f, 1.0f, 100.0f),
        0.0f
    );

    planeBody->m_pos = fungt::Vec3(0.0f, -0.5f, 0.0f);
    planeBody->m_restitution = 0.5f;
    planeBody->m_friction = 0.7f;

    collisionManager->add(planeBody);
    plane->addCollisionProperty(planeBody);

    /*
     * Visible procedural sphere MeshRenderable.
     *
     * createSphereMeshGeometry() creates a radius-1 sphere.
     * Scale 3 makes the visible sphere radius 3.
     */
    auto sphere = createGeomMesh(GeomMesh::Sphere);
    sphere->scale(3.0f);
    sphere->position(0.0f, 10.0f, 0.0f);

    auto sphereMaterial = sphere->getMeshGeometry()->getMaterials().front();
    sphereMaterial.m_baseColor = glm::vec3(0.05f, 0.2f, 1.0f);
    sphere->getMeshGeometry()->getMaterials().front() = sphereMaterial;

    sphere->load();

    auto sphereBody = std::make_shared<RigidBody>(
        std::make_unique<Sphere>(3.0f),
        5.0f
    );

    sphereBody->m_pos = fungt::Vec3(0.0f, 10.0f, 0.0f);
    sphereBody->m_restitution = 0.65f;
    sphereBody->m_friction = 0.4f;

    collisionManager->add(sphereBody);
    sphere->addCollisionProperty(sphereBody);

    game->set([&]() {
        sceneManager->addRenderableObj(plane);
        sceneManager->addRenderableObj(sphere);
    });

    float lastTime = glfwGetTime();

    game->render([&]() {
        const float currentTime = glfwGetTime();
        float deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        deltaTime = glm::min(deltaTime, 0.05f);

        physicsWorld->runColliders(deltaTime);

        sceneManager->renderScene();
    });

    return 0;
}
