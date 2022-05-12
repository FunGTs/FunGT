#include "material.hpp"

Material::Material(glm::vec3 _ambientLight, glm::vec3 _diffLigth, glm::vec3 _specLight, GLint _diffTexture, GLint _specTexture)
: ambientLight{_ambientLight}, diffLigth{_diffLigth}, specLight{_specLight}, diffTexture{_diffTexture}, specTexture{_specTexture}{
    
}
Material::~Material(){

}
void Material::sendToShader(Shader& program){
    //program.Bind();

    program.setUniformVec3f(ambientLight, "material.ambientLight");
    program.setUniformVec3f(diffLigth, "material.diffLigth");
    program.setUniformVec3f(specLight, "material.specLight");
    program.set1i(diffTexture, "material.diffTexture");
    program.set1i(specTexture, "material.specTexture");
    //program.unBind();
}