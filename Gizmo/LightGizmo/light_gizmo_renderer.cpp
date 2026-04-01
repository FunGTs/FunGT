#include "light_gizmo_renderer.hpp"

LightGizmoRenderer::LightGizmoRenderer(SceneManager* sceneManager, Camera* camera)
    : m_sceneManager(sceneManager)
    , m_camera(camera)
    , m_vao(0)
    , m_vbo(0)
    , m_ebo(0)
    , m_initialized(false)
{
}

LightGizmoRenderer::~LightGizmoRenderer()
{
    if (m_vao) glDeleteVertexArrays(1, &m_vao);
    if (m_vbo) glDeleteBuffers(1, &m_vbo);
    if (m_ebo) glDeleteBuffers(1, &m_ebo);
}

void LightGizmoRenderer::init()
{
    gizmo_vs = getAssetPath("shaders/gizmo_vs.glsl");
    gizmo_fs = getAssetPath("shaders/gizmo_fs.glsl");
    m_shader.create(gizmo_vs, gizmo_fs);

    float vertices[] = {
        -0.5f,  0.5f, 0.0f,
         0.5f,  0.5f, 0.0f,
        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f
    };

    unsigned int indices[] = {
        0, 2, 1,
        1, 2, 3
    };

    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);
    glGenBuffers(1, &m_ebo);

    glBindVertexArray(m_vao);

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    glBindVertexArray(0);

    m_initialized = true;
}

void LightGizmoRenderer::render(const glm::mat4& projection)
{
    if (!m_initialized) return;

    m_shader.Bind();
    m_shader.setUniformMat4fv("ViewMatrix", m_camera->getViewMatrix());
    m_shader.setUniformMat4fv("ProjectionMatrix", projection);
    m_shader.setUniformVec3f(m_sceneManager->getLightPosition(), "lightWorldPos");
    m_shader.setUniformVec4f(glm::vec4(1.0f, 0.9f, 0.2f, 1.0f), "gizmoColor");
    m_shader.setUniformVec1f(0.4f, "gizmoScale");

    glBindVertexArray(m_vao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    m_shader.unBind();
}