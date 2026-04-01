#if !defined(_LIGHT_GIZMO_RENDERER_HPP_)
#define _LIGHT_GIZMO_RENDERER_HPP_

#include "Shaders/shader.hpp"
#include "SceneManager/scene_manager.hpp"
#include "Camera/camera.hpp"
#include "include/prerequisites.hpp"
#include "include/glmath.hpp"

class LightGizmoRenderer {

    SceneManager* m_sceneManager;
    Camera* m_camera;
    Shader        m_shader;
    GLuint        m_vao;
    GLuint        m_vbo;
    GLuint        m_ebo;
    bool          m_initialized;
    std::string gizmo_vs;
    std::string gizmo_fs;
public:
    LightGizmoRenderer(SceneManager* sceneManager, Camera* camera);
    ~LightGizmoRenderer();

    void init();
    void render(const glm::mat4& projection);
};

#endif // _LIGHT_GIZMO_RENDERER_HPP_