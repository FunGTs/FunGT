#include "scene_manager.hpp"
#include <algorithm>

SceneManager::SceneManager()
    : m_shader(Shader::create())
{
    std::cout<<"Scene Manager Constructor"<<std::endl;
}

void SceneManager::initUBOs()
{
    m_matricesUBO = GPUBuffer::create();
    m_matricesUBO->create(BufferType::Uniform, nullptr, sizeof(glm::mat4) * 2);
    m_matricesUBO->bindBase(0);

    const size_t lightStride = 32;
    const size_t lightsUBOSize = lightStride * MAX_LIGHTS + 16;
    m_lightsUBO = GPUBuffer::create();
    m_lightsUBO->create(BufferType::Uniform, nullptr, lightsUBOSize);
    m_lightsUBO->bindBase(1);
}

void SceneManager::updateMatricesUBO()
{
    if (!m_matricesUBO)
        initUBOs();

    glm::mat4 matrices[2] = { m_ViewMatrix, m_ProjectionMatrix };
    m_matricesUBO->update(matrices, sizeof(matrices));
}

void SceneManager::updateLightsUBO()
{
    struct GPULight {
        glm::vec3 position; float pad0;
        glm::vec3 color;    float power;
    };

    const int count = static_cast<int>(std::min(m_lights.size(), static_cast<size_t>(MAX_LIGHTS)));
    std::vector<GPULight> gpuLights(count);
    for (int i = 0; i < count; ++i) {
        gpuLights[i].position = m_lights[i].position;
        gpuLights[i].color = m_lights[i].color;
        gpuLights[i].power = m_lights[i].power;
    }

    if (count > 0)
        m_lightsUBO->update(gpuLights.data(), gpuLights.size() * sizeof(GPULight));

    const size_t lightStride = 32;
    m_lightsUBO->update(&count, sizeof(int), lightStride * MAX_LIGHTS);
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
    updateMatricesUBO();
    updateLightsUBO();

    for (auto& layerBuckets : m_buckets)
        for (auto& bucket : layerBuckets)
            bucket.nodes.clear();

    for (auto& node : m_VectorOfRenderNodes) {
        auto& layerBuckets = m_buckets[static_cast<int>(node->getRenderLayer())];
        Shader* shader = &node->getShader();
        auto it = std::find_if(layerBuckets.begin(), layerBuckets.end(),
            [shader](const ShaderBucket& b) { return b.shader == shader; });
        if (it == layerBuckets.end()) {
            layerBuckets.push_back(ShaderBucket{ shader, {} });
            layerBuckets.back().nodes.reserve(m_VectorOfRenderNodes.size());
            it = layerBuckets.end() - 1;
        }
        it->nodes.push_back(node.get());
    }

    for (auto& layerBuckets : m_buckets) {
        for (auto& bucket : layerBuckets) {
            if (bucket.nodes.empty())
                continue;
            bucket.shader->Bind();
            for (auto* node : bucket.nodes) {
                node->enableDepthFunc(); //For Cubemap purposes
                node->setViewMatrix(m_ViewMatrix);
                node->updateModelMatrix();
                node->updateTime(m_deltaTime);
                node->getShader().setUniformVec3f(m_viewPos, "viewPos");

                node->getShader().setUniform1i("hasIBL", m_hasIBL ? 1 : 0);
                node->getShader().setUniformVec3f(m_ambientColor, "ambientColor");

                if (m_hasIBL) {
                    glActiveTexture(GL_TEXTURE1);
                    glBindTexture(GL_TEXTURE_CUBE_MAP, m_iblProbe->getIrradianceMapID());
                    node->getShader().set1i(1, "irradianceMap");
                    node->getShader().setUniform1f(m_iblIntensity, "iblIntensity");
                }

                node->getShader().setUniformMat4fv("ModelMatrix",node->getModelMatrix());
                node->draw();
                node->disableDepthFunc(); //For CubeMap purposes
            }
        }
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
