#include "particle_simulation_rtc.hpp"
#include "particle_rtc_sycl_ops.hpp"

ParticleRTC::ParticleRTC(std::size_t numOfParticles)
: m_NumParticles{numOfParticles}, m_shader(Shader::create()){
    std::cout << "Initializing SYCL for RTC ParticleSimulation..." << std::endl;
    particleRTC_initSycl();
    m_pSet.SetNumParticles(m_NumParticles);
    std::cout << "Particle RTC system constructor" << std::endl;
    std::cout << "Num particles: " << m_pSet._particles.size() << std::endl;

    m_fs = getAssetPath("resources/particle.fs");
    m_vs = getAssetPath("resources/particle.vs");

    m_shader->create(m_vs, m_fs);
}

bool ParticleRTC::compileInitKernel(const std::string& user_init_code, std::string& error_msg)
{
    return particleRTC_compileInitKernel(user_init_code, error_msg);
}

bool ParticleRTC::isSupported() {
    return particleRTC_isSupported();
}

void ParticleRTC::executeInitKernel()
{
    particleRTC_executeInitKernel(m_vbo.getId(), m_NumParticles);
}

bool ParticleRTC::compileKernel(const std::string& user_code, std::string& error_msg) {
    return particleRTC_compileKernel(user_code, error_msg);
}

void ParticleRTC::executeKernel() {
    particleRTC_executeKernel(m_vbo.getId(), m_NumParticles, 0.005f);
}

void ParticleRTC::init()
{
    m_vao.genVAO();
    m_vbo.genVB();

    m_vao.bind();
    m_vbo.bind();

    m_vbo.bufferData(nullptr,
        m_pSet._particles.size() * sizeof(fgt::Particle<float>),
        GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
        sizeof(fgt::Particle<float>),
        (void*)offsetof(fgt::Particle<float>, position));
    glEnableVertexAttribArray(0);

    m_vao.unbind();

    if (particleRTC_hasInitKernel()) {
        executeInitKernel();
    }
    m_isReady = true;
}

void ParticleRTC::simulation()
{
    if (m_isReady && particleRTC_hasKernel()) {
        executeKernel();
    }
}

void ParticleRTC::draw()
{
    this->simulation();
    glEnable(GL_PROGRAM_POINT_SIZE);
    m_vao.bind();
    glDrawArrays(GL_POINTS, 0, m_NumParticles);
}

Shader& ParticleRTC::getShader()
{
    return *m_shader;
}

void ParticleRTC::updateTime(float deltaTime)
{
    this->m_deltaTime = deltaTime;
}

void ParticleRTC::setViewMatrix(const glm::mat4& viewMatrix)
{
    m_viewMatrix = viewMatrix;
}

glm::mat4 ParticleRTC::getViewMatrix()
{
    return m_viewMatrix;
}

void ParticleRTC::updateModelMatrix()
{
    m_ModelMatrix = glm::mat4(1.f);
    m_ModelMatrix = glm::translate(m_ModelMatrix, m_position);
    m_ModelMatrix = glm::rotate(m_ModelMatrix, glm::radians(m_rotation.x), glm::vec3(1.f, 0.f, 0.f));
    m_ModelMatrix = glm::rotate(m_ModelMatrix, glm::radians(m_rotation.y), glm::vec3(0.f, 1.f, 0.f));
    m_ModelMatrix = glm::rotate(m_ModelMatrix, glm::radians(m_rotation.z), glm::vec3(0.f, 0.f, 1.f));
    m_ModelMatrix = glm::scale(m_ModelMatrix, m_scale);
}

glm::mat4 ParticleRTC::getModelMatrix() const
{
    return m_ModelMatrix;
}