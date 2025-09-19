#include "../PhysicsWorld/physics_world.hpp"
#include <iostream>

// Example usage
int main() {
    // Create physics world
    std::unique_ptr<PhysicsWorld> physics = std::make_unique<PhysicsWorld>();
    
    // Create a sphere that can roll
    auto ball = std::make_unique<RigidBody>(
        std::make_unique<Sphere>(1.0f), // radius = 1
        1.0f  // mass = 5
    );
    ball->m_pos = fungl::Vec3(0, 8, 0);    // Start 8 units high
    ball->m_vel = fungl::Vec3(3, 2, 0);    // Initial velocity with angle
    ball->m_angularVel = fungl::Vec3(0, 0, 5); // Initial spin
    ball->m_restitution = 0.8f;          // Very bouncy!
    ball->m_friction = 0.3f;

    // Create ground
    auto ground = std::make_unique<RigidBody>(
        std::make_unique<Box>(20.0f, 1.0f, 20.0f),
        0.0f  // Static
    );
    ground->m_pos = fungl::Vec3(0, -0.5f, 0);
    ground->m_restitution = 0.6f;
    ground->m_friction = 0.4f;

    // Create walls for bouncing
    auto leftWall = std::make_unique<RigidBody>(
        std::make_unique<Box>(1.0f, 10.0f, 20.0f),
        0.0f
    );
    leftWall->m_pos = fungl::Vec3(-10, 5, 0);
    leftWall->m_restitution = 0.7f;

    auto rightWall = std::make_unique<RigidBody>(
        std::make_unique<Box>(1.0f, 10.0f, 20.0f),
        0.0f
    );
    rightWall->m_pos = fungl::Vec3(10, 5, 0);
    rightWall->m_restitution = 0.7f;

    
    spCollisionManager myCollision = physics->getCollisionManager();

    // Add bodies to world
    myCollision->add(std::move(ball));
    myCollision->add(std::move(ground));
    myCollision->add(std::move(leftWall));
    myCollision->add(std::move(rightWall));
    
    // Simulate for a few steps
    std::cout << "Physics simulation started...\n";
    float dt = 1.0f / 60.0f; // 60 FPS
    
    std::cout << "Ball bouncing with your collision dispatcher!\n";
    std::cout << "Time\tPos(x,y,z)\t\tVel(x,y,z)\t\tAngVel(x,y,z)\n";
    std::cout << "----\t----------\t\t----------\t\t-------------\n";

    for (int i = 0; i < 600; i++) { // 10 seconds
        physics->runColliders(dt);

        if (i % 60 == 0) { // Print every second
            auto ballBody = myCollision->getCollideBody(0);
            fungl::Vec3 euler = ballBody->getEulerAngles();
            if (ballBody) {
                float time = i * dt;
                //print position, velocitym angular velocity and orientation
                printf("%.2f\t(%.2f, %.2f, %.2f)\t(%.2f, %.2f, %.2f)\t(%.2f, %.2f, %.2f)\n", 
                       time, 
                       ballBody->m_pos.x, ballBody->m_pos.y, ballBody->m_pos.z,
                       ballBody->m_vel.x, ballBody->m_vel.y, ballBody->m_vel.z,
                       ballBody->m_angularVel.x, ballBody->m_angularVel.y, ballBody->m_angularVel.z);
                std::cout<<"Orientation (Euler angles): ("<<euler.x<<", "<<euler.y<<", "<<euler.z<<")\n"; 
            }
        }
    }

    
    return 0;
}