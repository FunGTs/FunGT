#if !defined(_MATERIAL_FUNGT_H_)
#define _MATERIAL_FUNGT_H_
#include "../Shaders/shader.hpp"
class Material
{   private:
        glm::vec3 m_ambientLight; 
        glm::vec3 m_diffLigth; 
        glm::vec3 m_specLight;
        float m_shininess; 

    public:
        std::string m_name; 
        Material(glm::vec3 ambientLight, glm::vec3 diffLigth, glm::vec3 specLight,float inShin,std::string name);
        ~Material();
        void sendToShader(Shader& program);

    /* data */
};


#endif // _MATERIAL_FUNGT_H_
