#include "material.hpp"
Material::Material(){
    
}
Material::Material(glm::vec3 _ambientLight, glm::vec3 _diffLigth, glm::vec3 _specLight,float inShin,std::string name)
: m_ambientLight{_ambientLight}, m_diffLigth{_diffLigth}, m_specLight{_specLight},m_shininess{inShin},m_name{name}{
    
}
Material::~Material(){

}
void Material::sendToShader(Shader& program){

    program.setUniformVec3f(m_ambientLight, "material.ambient");
    program.setUniformVec3f(m_diffLigth, "material.diffuse");
    program.setUniformVec3f(m_specLight, "material.specular");
    program.setUniform1f(m_shininess,"material.shininess");
   
}