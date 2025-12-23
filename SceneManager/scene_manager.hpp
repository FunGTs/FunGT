#if !defined(_SCENE_MANAGER_H_)
#define _SCENE_MANAGER_H_
#include<memory>
#include "../Shaders/shader.hpp"
#include "../Animation/animation.hpp"
#include "../CubeMap/cube_map.hpp"
#include "../Camera/camera.hpp"
#include "../SimpleModel/simple_model.hpp"

class SceneManager{

    private:
        Shader m_shader;
        std::vector<std::shared_ptr<Renderable>> m_VectorOfRenderNodes;
        glm::mat4 m_ViewMatrix = glm::mat4(1.f);
        glm::mat4 m_ProjectionMatrix = glm::mat4(1.f);
        glm::mat4 m_ModelMatrix = glm::mat4(1.f);
        //Lights!
        // Default light properties
        glm::vec3 m_lightPosition = glm::vec3(5.0f, 5.0f, 5.0f);
        glm::vec3 m_lightAmbient = glm::vec3(0.3f, 0.3f, 0.3f);
        glm::vec3 m_lightDiffuse = glm::vec3(1.0f, 1.0f, 1.0f);
        glm::vec3 m_lightSpecular = glm::vec3(1.0f, 1.0f, 1.0f);
        float m_deltaTime; 
    public:
        SceneManager();
        ~SceneManager();
        void loadShaders(std::string &vs_pat, std::string &fs_path);
        Shader& getShader();
        std::vector<std::shared_ptr<Renderable>> getRenderable();
        void updateViewMatrix(const glm::mat4 &viewMatrix);
        void updateProjectionMatrix(const glm::mat4 &projectionMatrix);
        void updateModelMatrix(const glm::mat4 &modelMatrix); 
        void renderScene();
        void addRenderableObj(std::shared_ptr<Renderable> node);
        void setDeltaTime(float deltaT); 
        float getDetaTime(); 
        // Getters for GUI editing
        glm::vec3& getLightPosition() { return m_lightPosition; }
        glm::vec3& getLightAmbient() { return m_lightAmbient; }
        glm::vec3& getLightDiffuse() { return m_lightDiffuse; }
        glm::vec3& getLightSpecular() { return m_lightSpecular; }

};


#endif // _SCENE_MANAGER_H_
