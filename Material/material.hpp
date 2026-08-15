#if !defined(_MATERIAL_FUNGT_H_)
#define _MATERIAL_FUNGT_H_
#include "Shaders/shader.hpp"
class Material
{   public:
        glm::vec3 m_baseColor = glm::vec3(0.8f);
        float m_metallic = 0.0f;
        float m_roughness = 0.5f;
        float m_reflectance = 0.04f;
        float m_emission = 0.0f;

    public:
        std::string m_name;
        Material() = default;
        Material(glm::vec3 baseColor, float metallic, float roughness,
                 float reflectance, std::string name, float emission = 0.0f);
        ~Material() = default;
        void sendToShader(Shader& program) const;
        bool isInvalidMaterial() const;
        static Material createDefaultMaterial();

    /* data */
};


#endif // _MATERIAL_FUNGT_H_
