#include "mesh_renderable.hpp"

#include "Path_Manager/path_manager.hpp"

#include <limits>
#include <stdexcept>
#include <utility>

Shader* (*MeshRenderable::s_shaderCreate)() = nullptr;
void (*MeshRenderable::s_shaderFree)(Shader*) = nullptr;

MeshRenderable::MeshRenderable(std::shared_ptr<MeshGeometry> meshgeom)
{
    if (meshgeom) {
        m_meshgeoms.push_back(std::move(meshgeom));
    }
}

MeshRenderable::MeshRenderable(std::vector<std::shared_ptr<MeshGeometry>> meshgeoms)
    : m_meshgeoms(std::move(meshgeoms))
{
}

MeshRenderable::~MeshRenderable()
{
    if (s_shaderFree && m_shader) {
        s_shaderFree(m_shader);
    }
}

void MeshRenderable::load()
{
    if (m_meshgeoms.empty()) {
        throw std::runtime_error("MeshRenderable::load() - No mesh geometry set!");
    }

    if (!m_shader && s_shaderCreate) {
        m_shader = s_shaderCreate();
    }

    if (!m_shader) {
        return;
    }

    m_shader->create(getAssetPath("shaders/fungt_default.vs"),
                     getAssetPath("shaders/fungt_default.fs"));
    for (const auto& meshgeom : m_meshgeoms) {
        meshgeom->InitGraphics();
    }
}

void MeshRenderable::position(float x, float y, float z)
{
    m_hasImportedModelMatrix = false;
    m_position = glm::vec3(x, y, z);
    updateModelMatrix();
}

void MeshRenderable::rotation(float x, float y, float z)
{
    m_hasImportedModelMatrix = false;
    m_rotation = glm::vec3(x, y, z);
    updateModelMatrix();
}

void MeshRenderable::scale(float s)
{
    m_hasImportedModelMatrix = false;
    m_scale = glm::vec3(s);
    updateModelMatrix();
}

void MeshRenderable::scale(float x, float y, float z)
{
    m_hasImportedModelMatrix = false;
    m_scale = glm::vec3(x, y, z);
    updateModelMatrix();
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

bool MeshRenderable::getWorldBounds(glm::vec3& boundsMin,
                                    glm::vec3& boundsMax) const
{
    const float largestValue = std::numeric_limits<float>::max();
    boundsMin = glm::vec3(largestValue);
    boundsMax = glm::vec3(-largestValue);
    bool hasPositions = false;

    for (const auto& meshgeom : m_meshgeoms) {
        if (!meshgeom) {
            continue;
        }

        for (const glm::vec3& position : meshgeom->m_positions) {
            const glm::vec3 worldPosition = glm::vec3(
                m_modelMatrix * glm::vec4(position, 1.0f));
            boundsMin = glm::min(boundsMin, worldPosition);
            boundsMax = glm::max(boundsMax, worldPosition);
            hasPositions = true;
        }
    }

    return hasPositions;
}

std::shared_ptr<MeshGeometry> MeshRenderable::getMeshGeometry() const
{
    if (m_meshgeoms.empty()) {
        return nullptr;
    }

    return m_meshgeoms.front();
}

const std::vector<std::shared_ptr<MeshGeometry>>&
MeshRenderable::getGeomMeshes() const
{
    return m_meshgeoms;
}

void MeshRenderable::draw()
{
    for (const auto& meshgeom : m_meshgeoms) {
        meshgeom->draw(*m_shader);
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
