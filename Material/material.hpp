#if !defined(_MATERIAL_FUNGT_H_)
#define _MATERIAL_FUNGT_H_
#include "../Shaders/shader.hpp"
class Material
{   private:
        glm::vec3 ambientLight; 
        glm::vec3 diffLigth; 
        glm::vec3 specLight;
        GLint diffTexture; 
        GLint specTexture;

    public:
        Material(glm::vec3 ambientLight, glm::vec3 diffLigth, glm::vec3 specLight, GLint diffTexture, GLint specTexture);
        ~Material();
        void sendToShader(Shader& program);

    /* data */
};


#endif // _MATERIAL_FUNGT_H_
