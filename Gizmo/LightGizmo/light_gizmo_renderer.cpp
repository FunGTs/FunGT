#include "light_gizmo_renderer.hpp"
#include <cmath>

LightGizmoRenderer::LightGizmoRenderer(SceneManager* sceneManager, Camera* camera)
    : m_sceneManager(sceneManager)
    , m_camera(camera)
    , m_initialized(false)
{
}

LightGizmoRenderer::~LightGizmoRenderer()
{
    if (m_sphereMesh.vao) glDeleteVertexArrays(1, &m_sphereMesh.vao);
    if (m_sphereMesh.vbo) glDeleteBuffers(1, &m_sphereMesh.vbo);
    if (m_quadMesh.vao)   glDeleteVertexArrays(1, &m_quadMesh.vao);
    if (m_quadMesh.vbo)   glDeleteBuffers(1, &m_quadMesh.vbo);
}

void LightGizmoRenderer::buildSphere(float radius, int segments, std::vector<float>& out)
{
    // XY plane
    for (int i = 0; i < segments; i++)
    {
        float a1 = (float)i / segments * 2.0f * M_PI;
        float a2 = (float)(i + 1) / segments * 2.0f * M_PI;
        out.push_back(radius * cos(a1)); out.push_back(radius * sin(a1)); out.push_back(0.f);
        out.push_back(radius * cos(a2)); out.push_back(radius * sin(a2)); out.push_back(0.f);
    }
    // XZ plane
    for (int i = 0; i < segments; i++)
    {
        float a1 = (float)i / segments * 2.0f * M_PI;
        float a2 = (float)(i + 1) / segments * 2.0f * M_PI;
        out.push_back(radius * cos(a1)); out.push_back(0.f); out.push_back(radius * sin(a1));
        out.push_back(radius * cos(a2)); out.push_back(0.f); out.push_back(radius * sin(a2));
    }
    // YZ plane
    for (int i = 0; i < segments; i++)
    {
        float a1 = (float)i / segments * 2.0f * M_PI;
        float a2 = (float)(i + 1) / segments * 2.0f * M_PI;
        out.push_back(0.f); out.push_back(radius * cos(a1)); out.push_back(radius * sin(a1));
        out.push_back(0.f); out.push_back(radius * cos(a2)); out.push_back(radius * sin(a2));
    }
}

void LightGizmoRenderer::uploadMesh(GizmoMesh& mesh, const std::vector<float>& vertices, GLenum drawMode)
{
    mesh.drawMode = drawMode;
    mesh.vertexCount = (int)vertices.size() / 3;

    glGenVertexArrays(1, &mesh.vao);
    glGenBuffers(1, &mesh.vbo);

    glBindVertexArray(mesh.vao);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    glBindVertexArray(0);
}

void LightGizmoRenderer::init()
{
    std::string m_vs = getAssetPath("shaders/gizmo_vs.glsl");
    std::string m_fs = getAssetPath("shaders/gizmo_fs.glsl");
    m_shader.create(m_vs, m_fs);

    // Sphere for point light
    std::vector<float> sphereVerts;
    buildSphere(0.5f, 32, sphereVerts);
    uploadMesh(m_sphereMesh, sphereVerts, GL_LINES);

    // Quad billboard for other types
    std::vector<float> quadVerts = {
        -0.5f,  0.5f, 0.0f,
         0.5f,  0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
        -0.5f, -0.5f, 0.0f,
        -0.5f,  0.5f, 0.0f
    };
    uploadMesh(m_quadMesh, quadVerts, GL_LINE_STRIP);

    m_initialized = true;
}

void LightGizmoRenderer::render(const glm::mat4& projection)
{
    if (!m_initialized) return;

    const auto& lights = m_sceneManager->getLights();
    if (lights.empty()) return;

    m_shader.Bind();
    m_shader.setUniformMat4fv("ViewMatrix", m_camera->getViewMatrix());
    m_shader.setUniformMat4fv("ProjectionMatrix", projection);
    m_shader.setUniformVec1f(0.4f, "gizmoScale");

    for (const auto& light : lights)
    {
        glm::vec4 color;
        switch (light.type)
        {
        case SceneLightType::Point: color = glm::vec4(light.color, 1.0f); break;
        case SceneLightType::Sun:   color = glm::vec4(light.color, 1.0f); break;
        case SceneLightType::Spot:  color = glm::vec4(light.color, 1.0f); break;
        case SceneLightType::Area:  color = glm::vec4(light.color, 1.0f);; break;
        }

        m_shader.setUniformVec3f(light.position, "lightWorldPos");
        m_shader.setUniformVec4f(color, "gizmoColor");

        if (light.type == SceneLightType::Point)
        {
            m_shader.set1i(0, "isBillboard");
            glBindVertexArray(m_sphereMesh.vao);
            glLineWidth(2.0f);
            glDrawArrays(m_sphereMesh.drawMode, 0, m_sphereMesh.vertexCount);
            glLineWidth(1.0f);
        }
        else
        {
            m_shader.set1i(1, "isBillboard");
            glBindVertexArray(m_quadMesh.vao);
            glDrawArrays(m_quadMesh.drawMode, 0, m_quadMesh.vertexCount);
        }

        glBindVertexArray(0);
    }

    m_shader.unBind();
}