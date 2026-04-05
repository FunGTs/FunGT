#if !defined(_LIGHT_GIZMO_RENDERER_HPP_)
#define _LIGHT_GIZMO_RENDERER_HPP_

#include "Shaders/shader.hpp"
#include "SceneManager/scene_manager.hpp"
#include "Camera/camera.hpp"
#include "include/prerequisites.hpp"
#include "include/glmath.hpp"
#include <vector>

struct GizmoMesh {
    GLuint vao = 0;
    GLuint vbo = 0;
    int    vertexCount = 0;
    GLenum drawMode = GL_LINES;
};

class LightGizmoRenderer {

    SceneManager* m_sceneManager;   
    Camera* m_camera;
    Shader        m_shader;
    GizmoMesh     m_sphereMesh;
    GizmoMesh     m_quadMesh;
    bool          m_initialized;
    std::string m_vs;
    std::string m_fs;
    static void buildSphere(float radius, int segments, std::vector<float>& out);
    static void uploadMesh(GizmoMesh& mesh, const std::vector<float>& vertices, GLenum drawMode);

public:
    LightGizmoRenderer(SceneManager* sceneManager, Camera* camera);
    ~LightGizmoRenderer();

    void init();
    void render(const glm::mat4& projection);
};

#endif // _LIGHT_GIZMO_RENDERER_HPP_