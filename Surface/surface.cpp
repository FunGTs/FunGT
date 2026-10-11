#include "surface.hpp"
#include <vector>

Surface::Surface()
    : m_shader(Shader::create())
{
    m_vs = getAssetPath("shaders/fungt_surface.vs");
    m_fs = getAssetPath("shaders/fungt_surface.fs");
}

Surface::~Surface() = default;

void Surface::setField(std::function<float(fungt::Vec3)> field)
{
    m_mc.setField(field);
}

void Surface::sampleScalarFieldInside(fungt::Vec3 min, fungt::Vec3 max)
{
    m_mc.sampleScalarFieldInside(min, max);
}

void Surface::setResolution(int x, int y, int z)
{
    m_mc.setResolution(x, y, z);
}

void Surface::setIsovalue(float iso)
{
    m_mc.setIsovalue(iso);
}

void Surface::load()
{
    m_shader->create(m_vs, m_fs);
    rebuild();
}

void Surface::rebuild()
{
    m_mc.execute();

    const auto& mcVerts = m_mc.getVertices();
    const auto& indices = m_mc.getIndices();

    std::vector<SurfaceVertex> verts(mcVerts.size());
    for (size_t i = 0; i < mcVerts.size(); ++i) {
        verts[i].position = mcVerts[i].position;
        verts[i].normal = mcVerts[i].normal;
        verts[i].color = glm::vec3(0.8f, 0.8f, 0.8f);
    }

    m_indexCount = static_cast<unsigned int>(indices.size());

    m_buffer = GPUBuffer::create();
    m_buffer->genVAO();
    m_buffer->bindVAO();
    m_buffer->create(BufferType::Vertex, verts.data(),
        verts.size() * sizeof(SurfaceVertex));
    m_buffer->create(BufferType::Index, indices.data(),
        indices.size() * sizeof(unsigned int));

    m_buffer->applyFormat(SurfaceVertex::getFormat());

    m_buffer->unbindVAO();
}

void Surface::draw()
{
    if (!m_buffer || m_indexCount == 0)
        return;

    m_shader->Bind();

    m_buffer->bindVAO();
    m_buffer->drawIndexed(m_indexCount, DrawMode::Triangles);
    m_buffer->unbindVAO();
}

Shader& Surface::getShader()
{
    return *m_shader;
}

glm::mat4 Surface::getModelMatrix() const
{
    return m_modelMatrix;
}

glm::mat4 Surface::getViewMatrix()
{
    return m_viewMatrix;
}

void Surface::setViewMatrix(const glm::mat4& v)
{
    m_viewMatrix = v;
}

glm::mat4 Surface::getProjectionMatrix()
{
    return m_projMatrix;
}

void Surface::updateModelMatrix()
{
    m_modelMatrix = glm::mat4(1.f);
    m_modelMatrix = glm::translate(m_modelMatrix, m_position);
    m_modelMatrix = glm::rotate(m_modelMatrix,
        glm::radians(m_rotation.x), glm::vec3(1.f, 0.f, 0.f));
    m_modelMatrix = glm::rotate(m_modelMatrix,
        glm::radians(m_rotation.y), glm::vec3(0.f, 1.f, 0.f));
    m_modelMatrix = glm::rotate(m_modelMatrix,
        glm::radians(m_rotation.z), glm::vec3(0.f, 0.f, 1.f));
    m_modelMatrix = glm::scale(m_modelMatrix, m_scale);
}

void Surface::position(float x, float y, float z)
{
    m_position = glm::vec3(x, y, z);
}

void Surface::rotation(float x, float y, float z)
{
    m_rotation = glm::vec3(x, y, z);
}

void Surface::scale(float s)
{
    m_scale = glm::vec3(s);
}

void Surface::setMaterial(const Material& mat)
{
    m_material = mat;
}

Material& Surface::getMaterial()
{
    return m_material;
}

std::shared_ptr<Surface> Surface::create()
{
    return std::shared_ptr<Surface>(new Surface());
}
