#include "mesh_renderable.hpp"

#include "Path_Manager/path_manager.hpp"

#include <stdexcept>
#include <utility>

MeshRenderable::MeshRenderable(std::shared_ptr<MeshGeometry> meshgeom)
    : m_meshgeom(std::move(meshgeom))
    , m_shader(Shader::create())
{
}

void MeshRenderable::load()
{
    if (!m_meshgeom) {
        throw std::runtime_error("MeshRenderable::load() - No mesh geometry set!");
    }

    m_shader->create(getAssetPath("shaders/fungt_default.vs"),
                     getAssetPath("shaders/fungt_default.fs"));
    m_meshgeom->InitGraphics();
}

void MeshRenderable::position(float x, float y, float z)
{
    m_hasImportedModelMatrix = false;
    m_position = glm::vec3(x, y, z);
}

void MeshRenderable::rotation(float x, float y, float z)
{
    m_hasImportedModelMatrix = false;
    m_rotation = glm::vec3(x, y, z);
}

void MeshRenderable::scale(float s)
{
    m_hasImportedModelMatrix = false;
    m_scale = glm::vec3(s);
}

void MeshRenderable::scale(float x, float y, float z)
{
    m_hasImportedModelMatrix = false;
    m_scale = glm::vec3(x, y, z);
}

void MeshRenderable::setModelMatrix(const glm::mat4& modelMatrix)
{
    m_importedModelMatrix = modelMatrix;
    m_modelMatrix = modelMatrix;
    m_hasImportedModelMatrix = true;
}

glm::vec3 MeshRenderable::getPosition() const
{
    return m_position;
}

glm::vec3 MeshRenderable::getRotation() const
{
    return m_rotation;
}

glm::vec3 MeshRenderable::getScale() const
{
    return m_scale;
}

std::shared_ptr<MeshGeometry> MeshRenderable::getMeshGeometry() const
{
    return m_meshgeom;
}

void MeshRenderable::draw()
{
    if (m_meshgeom) {
        m_meshgeom->draw(*m_shader);
    }
}

Shader& MeshRenderable::getShader()
{
    return *m_shader;
}

glm::mat4 MeshRenderable::getModelMatrix() const
{
    return m_modelMatrix;
}

void MeshRenderable::setViewMatrix(const glm::mat4& viewMatrix)
{
    m_viewMatrix = viewMatrix;
}

void MeshRenderable::updateModelMatrix()
{
    if (m_hasImportedModelMatrix) {
        m_modelMatrix = m_importedModelMatrix;
        return;
    }

    m_modelMatrix = glm::mat4(1.0f);
    m_modelMatrix = glm::translate(m_modelMatrix, m_position);
    m_modelMatrix = glm::rotate(m_modelMatrix, glm::radians(m_rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    m_modelMatrix = glm::rotate(m_modelMatrix, glm::radians(m_rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    m_modelMatrix = glm::rotate(m_modelMatrix, glm::radians(m_rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
    m_modelMatrix = glm::scale(m_modelMatrix, m_scale);
}
