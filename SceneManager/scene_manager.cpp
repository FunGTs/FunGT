#include "scene_manager.hpp"

SceneManager::SceneManager(){
        std::cout<<"Scene Manager Constructor"<<std::endl;
        
}
SceneManager:: ~SceneManager(){
    std::cout<<"Scene Manager Destructor"<<std::endl;
}

void SceneManager::loadShaders(std::string &vs_pat, std::string &fs_path)
{
    m_shader.create(vs_pat,fs_path);
}
Shader& SceneManager::getShader(){
    //std::cout<<"Returning Shader reference"<<std::endl;
    return m_shader;
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
        node->getShader().setUniformVec3f(m_lightPosition, "light.position");
        node->getShader().setUniformVec3f(m_lightAmbient, "light.ambient");
        node->getShader().setUniformVec3f(m_lightDiffuse, "light.diffuse");
        node->getShader().setUniformVec3f(m_lightSpecular, "light.specular");
        node->getShader().setUniformMat4fv("ViewMatrix",node->getViewMatrix());
        node->getShader().setUniformMat4fv("ProjectionMatrix",m_ProjectionMatrix);
        node->getShader().setUniformMat4fv("ModelMatrix",node->getModelMatrix());
        node->draw();
        node->disableDepthFunc(); //For CubeMap purposes
    }
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
