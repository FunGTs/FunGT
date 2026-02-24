#if !defined(_PHYSICS_DEBUG_RENDERER_H_)
#define _PHYSICS_DEBUG_RENDERER_H_

#include "debug_rigidbody_renderer.hpp"
#include "Physics/CollisionManager/collision_manager.hpp"
#include "Camera/camera.hpp"
#include "SceneManager/scene_manager.hpp"
#include <memory>

// NOT a Layer anymore! Just a rendering utility
class PhysicsDebugRenderer {
private:
    std::unique_ptr<DebugRenderer> m_debugRenderer;
    std::shared_ptr<CollisionManager> m_collisionManager;
    Camera* m_camera;   
    std::shared_ptr<SceneManager> m_sceneManager;

    bool m_showCollisionBoxes;
    std::weak_ptr<RigidBody> m_selectedBody;

public:
    PhysicsDebugRenderer(std::shared_ptr<CollisionManager> collisionManager,std::shared_ptr<SceneManager> sceneManager ,Camera* camera)
        : m_collisionManager(collisionManager)
        , m_camera(camera)
        , m_sceneManager(sceneManager)  
        , m_showCollisionBoxes(false)
    {
        // Initialize immediately
        m_debugRenderer = std::make_unique<DebugRenderer>();
        m_debugRenderer->init();
        std::cout << "PhysicsDebugRenderer created and initialized" << std::endl;
    }

    ~PhysicsDebugRenderer() = default;

    // Call this from ViewPortLayer's render function!
    void render() {
        if (!m_showCollisionBoxes || !m_debugRenderer) return;
        if (!m_collisionManager || !m_camera) return;

        m_debugRenderer->clear();

        // Get ALL rigid bodies
        const auto& rigidBodies = m_collisionManager->getCollidable();
        auto selectedBodyLocked = m_selectedBody.lock();

        for (const auto& body : rigidBodies) {
            if (!body) continue;

            // Yellow if selected, red otherwise
            fungt::Vec3 color = (body == selectedBodyLocked)
                ? fungt::Vec3(1.0f, 1.0f, 0.0f)
                : fungt::Vec3(1.0f, 0.0f, 0.0f);

            renderRigidBodyWireframe(body, color);
        }

        // Render all wireframes
        glm::mat4 view = m_camera->getViewMatrix();
        glm::mat4 proj = m_sceneManager->getProjectionMatrix();

        m_debugRenderer->render(view, proj);
    }

    void setShowCollisionBoxes(bool show) {
        m_showCollisionBoxes = show;
    }

    bool isShowingCollisionBoxes() const {
        return m_showCollisionBoxes;
    }

    void setSelectedBody(std::weak_ptr<RigidBody> body) {
        m_selectedBody = body;
    }

    std::weak_ptr<RigidBody> getSelectedBody() const {
        return m_selectedBody;
    }

private:
    void renderRigidBodyWireframe(std::shared_ptr<RigidBody> body, const fungt::Vec3& color) {
        if (!body || !body->m_shape) return;

        ShapeType shapeType = body->m_shape->GetType();

        switch (shapeType) {
        case ShapeType::BOX: {
            Box* box = static_cast<Box*>(body->m_shape.get());
            fungt::Vec3 size(box->m_width, box->m_height, box->m_depth);
            m_debugRenderer->drawWireframeBox(body->m_pos, size, color);
            break;
        }

        case ShapeType::SPHERE: {
            Sphere* sphere = static_cast<Sphere*>(body->m_shape.get());
            float radius = sphere->m_radius;
            // Draw proper sphere wireframe with 3 circles!
            m_debugRenderer->drawWireframeSphere(body->m_pos, radius, color, 20);
            break;
        }

        default:
            break;
        }
    }
};

#endif // _PHYSICS_DEBUG_RENDERER_H_