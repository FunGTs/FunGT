#include "material.hpp"
Material::Material(glm::vec3 baseColor, float metallic, float roughness,
                   float reflectance, std::string name, float emission)
    : m_baseColor(baseColor)
    , m_metallic(glm::clamp(metallic, 0.0f, 1.0f))
    , m_roughness(glm::clamp(roughness, 0.05f, 1.0f))
    , m_reflectance(glm::clamp(reflectance, 0.0f, 1.0f))
    , m_emission(emission)
    , m_name(std::move(name))
{
}

void Material::sendToShader(Shader& program) const {
    program.setUniformVec3f(m_baseColor, "material.baseColor");
    program.setUniform1f(m_metallic, "material.metallic");
    program.setUniform1f(m_roughness, "material.roughness");
    program.setUniform1f(m_reflectance, "material.reflectance");
    program.setUniform1f(m_emission, "material.emission");

    // Compatibility for custom shaders that still use the legacy Phong fields.
    const glm::vec3 specular = glm::mix(glm::vec3(m_reflectance), m_baseColor, m_metallic);
    const float shininess = glm::max(1.0f, 2.0f / (m_roughness * m_roughness) - 2.0f);
    program.setUniformVec3f(m_baseColor, "material.ambient");
    program.setUniformVec3f(m_baseColor, "material.diffuse");
    program.setUniformVec3f(specular, "material.specular");
    program.setUniform1f(shininess, "material.shininess");
}

bool Material::isInvalidMaterial() const
{
    const float epsilon = 0.001f;
    return glm::length(m_baseColor) < epsilon;
}

Material Material::createDefaultMaterial()
{
    return Material(glm::vec3(0.8f), 0.0f, 0.5f, 0.04f, "FunGT_Default");
}
