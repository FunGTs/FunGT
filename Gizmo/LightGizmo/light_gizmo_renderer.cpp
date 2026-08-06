#include "light_gizmo_renderer.hpp"
#include <cmath>

LightGizmoRenderer::LightGizmoRenderer(SceneManager* sceneManager, Camera* camera)
    : m_sceneManager(sceneManager)
    , m_camera(camera)
    , m_initialized(false)
{
}

void LightGizmoRenderer::buildSphere(float radius, int segments, std::vector<GizmoVertex>& out)
{
    auto push = [&](float x, float y, float z) { out.push_back({ glm::vec3(x, y, z) }); };

    for (int i = 0; i < segments; i++) {
        float a1 = (float)i / segments * 2.0f * M_PI;
        float a2 = (float)(i + 1) / segments * 2.0f * M_PI;
        push(radius * cos(a1), radius * sin(a1), 0.f);
        push(radius * cos(a2), radius * sin(a2), 0.f);
    }
    for (int i = 0; i < segments; i++) {
        float a1 = (float)i / segments * 2.0f * M_PI;
        float a2 = (float)(i + 1) / segments * 2.0f * M_PI;
        push(radius * cos(a1), 0.f, radius * sin(a1));
        push(radius * cos(a2), 0.f, radius * sin(a2));
    }
    for (int i = 0; i < segments; i++) {
        float a1 = (float)i / segments * 2.0f * M_PI;
        float a2 = (float)(i + 1) / segments * 2.0f * M_PI;
        push(0.f, radius * cos(a1), radius * sin(a1));
        push(0.f, radius * cos(a2), radius * sin(a2));
    }
}

void LightGizmoRenderer::uploadMesh(GizmoMesh& mesh, const std::vector<GizmoVertex>& vertices, DrawMode drawMode)
{
    mesh.drawMode    = drawMode;
    mesh.vertexCount = (int)vertices.size();

    mesh.buffer = GPUBuffer::create();
    mesh.buffer->genVAO();
    mesh.buffer->bindVAO();
    mesh.buffer->create(BufferType::Vertex, vertices.data(), vertices.size() * sizeof(GizmoVertex));
    mesh.buffer->applyFormat(GizmoVertex::getFormat());
    mesh.buffer->unbindVAO();
}

void LightGizmoRenderer::init()
{
    std::string m_vs = getAssetPath("shaders/gizmo_vs.glsl");
    std::string m_fs = getAssetPath("shaders/gizmo_fs.glsl");
    m_shader.create(m_vs, m_fs);

    std::vector<GizmoVertex> sphereVerts;
    buildSphere(0.5f, 32, sphereVerts);
    uploadMesh(m_sphereMesh, sphereVerts, DrawMode::Lines);

    std::vector<GizmoVertex> quadVerts = {
        { glm::vec3(-0.5f,  0.5f, 0.0f) },
        { glm::vec3( 0.5f,  0.5f, 0.0f) },
        { glm::vec3( 0.5f, -0.5f, 0.0f) },
        { glm::vec3(-0.5f, -0.5f, 0.0f) },
        { glm::vec3(-0.5f,  0.5f, 0.0f) }
    };
    uploadMesh(m_quadMesh, quadVerts, DrawMode::LineStrip);

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
        case SceneLightType::Area:  color = glm::vec4(light.color, 1.0f); break;
        }

        m_shader.setUniformVec3f(light.position, "lightWorldPos");
        m_shader.setUniformVec4f(color, "gizmoColor");

        if (light.type == SceneLightType::Point)
        {
            m_shader.set1i(0, "isBillboard");
            m_sphereMesh.buffer->bindVAO();
            glLineWidth(2.0f);
            m_sphereMesh.buffer->drawArrays(m_sphereMesh.vertexCount, m_sphereMesh.drawMode);
            glLineWidth(1.0f);
            m_sphereMesh.buffer->unbindVAO();
        }
        else
        {
            m_shader.set1i(1, "isBillboard");
            m_quadMesh.buffer->bindVAO();
            m_quadMesh.buffer->drawArrays(m_quadMesh.vertexCount, m_quadMesh.drawMode);
            m_quadMesh.buffer->unbindVAO();
        }
    }

    m_shader.unBind();
}
