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

};


#endif // _SCENE_MANAGER_H_
