#if !defined(_SCENE_MANAGER_H_)
#define _SCENE_MANAGER_H_
#include<memory>
#include "Shaders/shader.hpp"
#include "Animation/animation.hpp"
#include "CubeMap/cube_map.hpp"
#include "Camera/camera.hpp"
#include "SimpleModel/simple_model.hpp"
#include "Lights/scene_light.hpp"
#include "IBL/ibl_probe.hpp"
#include "GraphicsRenderBackend/gpu_buffer.hpp"
#include <array>

struct ShaderBucket {
    Shader* shader;
    std::vector<Renderable*> nodes;
};

class SceneManager{

    private:
        std::unique_ptr<Shader> m_shader;
        std::vector<std::shared_ptr<Renderable>> m_VectorOfRenderNodes;
        std::array<std::vector<ShaderBucket>, 3> m_buckets;
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

        // IBL: irradiance cubemap sampled for ambient light, built via loadEnvironment()
        std::unique_ptr<IBLProbe> m_iblProbe;
        bool m_hasIBL = false;
        float m_iblIntensity = 0.3f;

        glm::vec3 m_ambientColor = glm::vec3(0.3f, 0.3f, 0.3f);

        static constexpr int MAX_LIGHTS = 32;
        std::unique_ptr<GPUBuffer> m_matricesUBO;
        std::unique_ptr<GPUBuffer> m_lightsUBO;
        void initUBOs();
        void updateMatricesUBO();
        void updateLightsUBO();
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
        void loadEnvironment(const std::string& hdrPath);
        void setIBLIntensity(float intensity) { m_iblIntensity = intensity; }
        float getIBLIntensity() const { return m_iblIntensity; }
        void setAmbientColor(const glm::vec3& color) { m_ambientColor = color; }
        glm::vec3 getAmbientColor() const { return m_ambientColor; }
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
