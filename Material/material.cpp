#include "material.hpp"
Material::Material(){
    
}
Material::Material(glm::vec3 _ambientLight, glm::vec3 _diffLigth, glm::vec3 _specLight,float inShin,std::string name)
: m_ambientLight{_ambientLight}, m_diffLigth{_diffLigth}, m_specLight{_specLight},m_shininess{inShin},m_name{name}{
    
}
Material::~Material(){

}

Material::Material(const Material& other)
    : m_ambientLight(other.m_ambientLight)
    , m_diffLigth(other.m_diffLigth)
    , m_specLight(other.m_specLight)
    , m_shininess(other.m_shininess)
    , m_name(other.m_name)
{
}

Material::Material(Material&& other) noexcept
    : m_ambientLight(other.m_ambientLight)
    , m_diffLigth(other.m_diffLigth)
    , m_specLight(other.m_specLight)
    , m_shininess(other.m_shininess)
    , m_name(std::move(other.m_name))
{
}

Material& Material::operator=(const Material& other) {
    if (this == &other) return *this;
    m_ambientLight = other.m_ambientLight;
    m_diffLigth = other.m_diffLigth;
    m_specLight = other.m_specLight;
    m_shininess = other.m_shininess;
    m_name = other.m_name;
    return *this;
}

Material& Material::operator=(Material&& other) noexcept {
    if (this == &other) return *this;
    m_ambientLight = other.m_ambientLight;
    m_diffLigth = other.m_diffLigth;
    m_specLight = other.m_specLight;
    m_shininess = other.m_shininess;
    m_name = std::move(other.m_name);
    return *this;
}
void Material::sendToShader(Shader& program) {

    program.setUniformVec3f(m_ambientLight, "material.ambient");
    program.setUniformVec3f(m_diffLigth, "material.diffuse");
    program.setUniformVec3f(m_specLight, "material.specular");
    program.setUniform1f(m_shininess, "material.shininess");

}