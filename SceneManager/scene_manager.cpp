#include "scene_manager.hpp"

SceneManager::SceneManager()
    : m_shader(Shader::create())
{
    std::cout<<"Scene Manager Constructor"<<std::endl;
}
SceneManager:: ~SceneManager(){
    std::cout<<"Scene Manager Destructor"<<std::endl;
}

void SceneManager::loadShaders(std::string &vs_pat, std::string &fs_path)
{
    m_shader->create(vs_pat,fs_path);
}
Shader& SceneManager::getShader(){
    return *m_shader;
}
std::vector<std::shared_ptr<Renderable>> SceneManager::getRenderable()
{
    return m_VectorOfRenderNodes;
}
void SceneManager::updateViewMatrix(const glm::mat4 &viewMatrix)
{
    m_ViewMatrix = viewMatrix; 
}
void SceneManager::updateProjectionMatrix(const glm::mat4 &projectionMatrix)
{
    m_ProjectionMatrix = projectionMatrix; 
}
void SceneManager::updateModelMatrix(const glm::mat4 &modelMatrix)
{
    m_ModelMatrix = modelMatrix;
}
void SceneManager::renderScene()
{
    for(auto & node : m_VectorOfRenderNodes){
        node->getShader().Bind();
        node->enableDepthFunc(); //For Cubemap purposes
        node->setViewMatrix(m_ViewMatrix);
        node->updateModelMatrix();
        node->updateTime(m_deltaTime);
        node->getShader().setUniform1i("numLights", (int)m_lights.size());
        node->getShader().setUniformVec3f(m_viewPos, "viewPos");

        node->getShader().setUniform1i("hasIBL", m_hasIBL ? 1 : 0);
        node->getShader().setUniformVec3f(m_ambientColor, "ambientColor");

        if (m_hasIBL) {
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_CUBE_MAP, m_iblProbe->getIrradianceMapID());
            node->getShader().set1i(1, "irradianceMap");
            node->getShader().setUniform1f(m_iblIntensity, "iblIntensity");
        }

        for (size_t i = 0; i < m_lights.size(); ++i)
        {
            const SceneLight& light = m_lights[i];
            std::string idx = std::to_string(i);
            node->getShader().setUniformVec3f(light.position, "light[" + idx + "].position");
            node->getShader().setUniformVec3f(light.color, "light[" + idx + "].color");
            node->getShader().setUniformVec1f(light.power, "light[" + idx + "].power");
        }
        node->getShader().setUniformMat4fv("ViewMatrix",node->getViewMatrix());
        node->getShader().setUniformMat4fv("ProjectionMatrix",m_ProjectionMatrix);
        node->getShader().setUniformMat4fv("ModelMatrix",node->getModelMatrix());
        node->draw();
        node->disableDepthFunc(); //For CubeMap purposes
    }
}
void SceneManager::loadEnvironment(const std::string& hdrPath)
{
    m_iblProbe = std::make_unique<IBLProbe>();
    m_iblProbe->build(hdrPath);
    m_hasIBL = true;
}

void SceneManager::addRenderableObj(std::shared_ptr<Renderable> node)
{
    m_VectorOfRenderNodes.push_back(node);
}

void SceneManager::setDeltaTime(float deltaT)
{
    m_deltaTime = deltaT;
}

float SceneManager::getDetaTime()
{
    return m_deltaTime;
}
