#include "material.hpp"
Material::Material(){
    
}
Material::Material(glm::vec3 _ambientLight, glm::vec3 _diffLigth, glm::vec3 _specLight,float inShin,std::string name, float emission)
: m_ambientLight{_ambientLight}, m_diffLigth{_diffLigth}, m_specLight{_specLight},m_shininess{inShin},m_name{name},m_emission{emission}{

}
Material::~Material(){

}

Material::Material(const Material& other)
    : m_ambientLight(other.m_ambientLight)
    , m_diffLigth(other.m_diffLigth)
    , m_specLight(other.m_specLight)
    , m_shininess(other.m_shininess)
    , m_name(other.m_name)
    , m_emission(other.m_emission)
{
}

Material::Material(Material&& other) noexcept
    : m_ambientLight(other.m_ambientLight)
    , m_diffLigth(other.m_diffLigth)
    , m_specLight(other.m_specLight)
    , m_shininess(other.m_shininess)
    , m_name(std::move(other.m_name))
    , m_emission(other.m_emission)
{
}

Material& Material::operator=(const Material& other) {
    if (this == &other) return *this;
    m_ambientLight = other.m_ambientLight;
    m_diffLigth = other.m_diffLigth;
    m_specLight = other.m_specLight;
    m_shininess = other.m_shininess;
    m_name      = other.m_name;
    m_emission  = other.m_emission;
    return *this;
}

Material& Material::operator=(Material&& other) noexcept {
    if (this == &other) return *this;
    m_ambientLight = other.m_ambientLight;
    m_diffLigth = other.m_diffLigth;
    m_specLight = other.m_specLight;
    m_shininess = other.m_shininess;
    m_name = std::move(other.m_name);
    m_emission = other.m_emission;
    return *this;
}
void Material::sendToShader(Shader& program) {

    program.setUniformVec3f(m_ambientLight, "material.ambient");
    program.setUniformVec3f(m_diffLigth, "material.diffuse");
    program.setUniformVec3f(m_specLight, "material.specular");
    program.setUniform1f(m_shininess, "material.shininess");
    program.setUniform1f(m_emission, "material.emission");
}

bool Material::isInvalidMaterial() const
{
    // Check if ambient AND diffuse are essentially zero
    // (specular alone won't make the mesh visible)
    const float epsilon = 0.001f;
    return (glm::length(m_ambientLight) < epsilon &&
        glm::length(m_diffLigth) < epsilon);
    // Note: We don't check specular - it can be non-zero but won't help if base colors are black
}

Material Material::createDefaultMaterial()
{
    // Nice neutral gray;
    glm::vec3 ambient(0.2f, 0.2f, 0.2f);
    glm::vec3 diffuse(0.8f, 0.8f, 0.8f);
    glm::vec3 specular(0.5f, 0.5f, 0.5f);
    float shininess = 32.0f;
    float emission  = 0.f;

    return Material(ambient, diffuse, specular, shininess, "FunGT_Default", emission);
}
