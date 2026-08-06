#if !defined(_SCENE_MANAGER_H_)
#define _SCENE_MANAGER_H_
#include<memory>
#include "Shaders/shader.hpp"
#include "Animation/animation.hpp"
#include "CubeMap/cube_map.hpp"
#include "Camera/camera.hpp"
#include "SimpleModel/simple_model.hpp"
#include "Lights/scene_light.hpp"

class SceneManager{

    private:
        std::unique_ptr<Shader> m_shader;
        std::vector<std::shared_ptr<Renderable>> m_VectorOfRenderNodes;
        glm::mat4 m_ViewMatrix = glm::mat4(1.f);
        glm::mat4 m_ProjectionMatrix = glm::mat4(1.f);
        glm::mat4 m_ModelMatrix = glm::mat4(1.f);
        //Lights!
        std::vector<SceneLight> m_lights;
        glm::vec3 m_viewPos = glm::vec3(0.f);
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
        void updateViewPos(const glm::vec3& pos) { m_viewPos = pos; }
        void renderScene();
        void addRenderableObj(std::shared_ptr<Renderable> node);
        void setDeltaTime(float deltaT); 
        float getDetaTime(); 
        glm::mat4 getProjectionMatrix() const {
            return m_ProjectionMatrix;
        }
        // Getters for GUI editing
        glm::vec3& getLightPosition() { return m_lightPosition; }
        glm::vec3& getLightAmbient() { return m_lightAmbient; }
        glm::vec3& getLightDiffuse() { return m_lightDiffuse; }
        glm::vec3& getLightSpecular() { return m_lightSpecular; }

        //Light
        void addLight(const SceneLight& light) { m_lights.push_back(light); }
        const std::vector<SceneLight>& getLights() const { return m_lights; }
        std::vector<SceneLight>& getLights() { return m_lights; }
        size_t getLightCount() const { return m_lights.size(); }

};


#endif // _SCENE_MANAGER_H_
