#if !defined(_PHYSICS_DEBUG_LAYER_H_)
#define _PHYSICS_DEBUG_LAYER_H_

#include "Layer/layer.hpp"
#include "debug_rigidbody_renderer.hpp"
#include "SceneManager/scene_manager.hpp"
#include "Camera/camera.hpp"
#include <memory>

class PhysicsDebugLayer : public Layer {
private:
    std::unique_ptr<DebugRenderer> m_debugRenderer;
    std::shared_ptr<SceneManager> m_sceneManager;
    Camera* m_camera;

    bool m_showCollisionBoxes;
    std::weak_ptr<RigidBody> m_selectedBody;  // For highlighting selected object

public:
    PhysicsDebugLayer(std::shared_ptr<SceneManager> sceneManager, Camera* camera)
        : Layer("Physics Debug Layer")
        , m_sceneManager(sceneManager)
        , m_camera(camera)
        , m_showCollisionBoxes(false)
    {
    }

    ~PhysicsDebugLayer() override = default;

    void onAttach() override {
        std::cout << "PhysicsDebugLayer::onAttach" << std::endl;

        // Create debug renderer
        m_debugRenderer = std::make_unique<DebugRenderer>();
        m_debugRenderer->init();
    }

    void onDetach() override {
        std::cout << "PhysicsDebugLayer::onDetach" << std::endl;
    }

    void onUpdate() override {
        // Nothing to update per frame
    }

    void begin() override {
        // Nothing needed
    }

    void end() override {
        // Render wireframes AFTER scene is rendered
        if (m_showCollisionBoxes && m_debugRenderer) {
            renderCollisionBoxes();
        }
    }

    void onImGuiRender() override {
        // This layer doesn't render ImGui - that's for CollisionDebugWindow
    }

    // Toggle collision box visibility
    void setShowCollisionBoxes(bool show) {
        m_showCollisionBoxes = show;
    }

    bool isShowingCollisionBoxes() const {
        return m_showCollisionBoxes;
    }

    // Set selected rigid body (for highlighting)
    void setSelectedBody(std::weak_ptr<RigidBody> body) {
        m_selectedBody = body;
    }

    std::weak_ptr<RigidBody> getSelectedBody() const {
        return m_selectedBody;
    }

private:
    void renderCollisionBoxes() {
        if (!m_sceneManager || !m_camera) return;

        m_debugRenderer->clear();

        // Get all renderable objects
        const auto& renderables = m_sceneManager->getRenderable();

        // Lock the selected body once
        auto selectedBodyLocked = m_selectedBody.lock();
        int bodyCount = 0;
        for (const auto& obj : renderables) {
            // Check if this object has a RigidBody attached
            auto weakBody = obj->getRigidBody();
            auto rigidBody = weakBody.lock();  // Convert weak_ptr to shared_ptr

            if (!rigidBody) continue;  // No physics body or expired

            // Determine color (yellow if selected, red otherwise)
            fungt::Vec3 color = (rigidBody == selectedBodyLocked)
                ? fungt::Vec3(1.0f, 1.0f, 0.0f)  // Yellow for selected
                : fungt::Vec3(1.0f, 0.0f, 0.0f); // Red for normal
            std::cout << "Drawing wireframe for body at: " << rigidBody->m_pos.x << ","
                << rigidBody->m_pos.y << "," << rigidBody->m_pos.z << std::endl;
            // Get shape and draw appropriate wireframe
            renderRigidBodyWireframe(rigidBody, color);
        }

        // Render all wireframes
        glm::mat4 view = m_camera->getViewMatrix();
        glm::mat4 proj = m_sceneManager->getProjectionMatrix();
        m_debugRenderer->render(view, proj);
    }

    void renderRigidBodyWireframe(std::shared_ptr<RigidBody> body, const fungt::Vec3& color) {
        if (!body || !body->m_shape) return;

        // Get shape type
        ShapeType shapeType = body->m_shape->GetType();
        switch (shapeType) {
        case ShapeType::BOX: {
            // Get box dimensions from shape
            Box* box = static_cast<Box*>(body->m_shape.get());
            fungt::Vec3 size(box->m_width, box->m_height, box->m_depth);

            m_debugRenderer->drawWireframeBox(body->m_pos, size, color);
            break;
        }

        case ShapeType::SPHERE: {
            // Get sphere radius from shape
            Sphere* sphere = static_cast<Sphere*>(body->m_shape.get());
            float radius = sphere->m_radius;

            // Draw sphere as wireframe (for now, draw a box approximation)
            // TODO: Implement proper sphere wireframe
            fungt::Vec3 size(radius * 2.0f, radius * 2.0f, radius * 2.0f);
            m_debugRenderer->drawWireframeBox(body->m_pos, size, color);
            break;
        }

        default:
            // Unsupported shape type
            break;
        }
    }
};

#endif // _PHYSICS_DEBUG_LAYER_H_