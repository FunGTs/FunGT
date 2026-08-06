#if !defined(_LIGHT_GIZMO_RENDERER_HPP_)
#define _LIGHT_GIZMO_RENDERER_HPP_

#include "Shaders/shader.hpp"
#include "SceneManager/scene_manager.hpp"
#include "Camera/camera.hpp"
#include "GraphicsRenderBackend/gpu_buffer.hpp"
#include "GraphicsRenderBackend/vertex_format.hpp"
#include <memory>
#include <vector>

struct GizmoVertex {
    glm::vec3 position;
    FUNGT_VERTEX_FORMAT(GizmoVertex, position)
};

struct GizmoMesh {
    std::unique_ptr<GPUBuffer> buffer;
    int       vertexCount = 0;
    DrawMode  drawMode    = DrawMode::Lines;
};

class LightGizmoRenderer {

    SceneManager* m_sceneManager;
    Camera*       m_camera;
    Shader        m_shader;
    GizmoMesh     m_sphereMesh;
    GizmoMesh     m_quadMesh;
    bool          m_initialized;
    std::string m_vs;
    std::string m_fs;
    static void buildSphere(float radius, int segments, std::vector<GizmoVertex>& out);
    static void uploadMesh(GizmoMesh& mesh, const std::vector<GizmoVertex>& vertices, DrawMode drawMode);

public:
    LightGizmoRenderer(SceneManager* sceneManager, Camera* camera);
    ~LightGizmoRenderer() = default;

    void init();
    void render(const glm::mat4& projection);
};

#endif // _LIGHT_GIZMO_RENDERER_HPP_
