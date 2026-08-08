#include "particle_simulation.hpp"
#include "particle_simulation_sycl_ops.hpp"

ParticleSimulation::ParticleSimulation(size_t num, std::string vertex_shader, std::string fragment_shader)
: m_NumParticles{num}, m_shader(Shader::create()){
    // INITIALIZE SYCL WITH GL INTEROP (ParticleSimulation owns this responsibility)
    particleSim_initSycl();
    m_pSet.SetNumParticles(m_NumParticles);
    std::cout << "Particle system constructor" << std::endl;
    std::cout << "Num particles: " << m_pSet._particles.size() << std::endl;

    loadDemo(3);
    this->init();

    m_shader->create(vertex_shader, fragment_shader);
}

void ParticleSimulation::loadDemo(int demo_index)
{
     if (demo_index < 0 || demo_index >= particleSim_getDemoCount()) {
        std::cerr << "Invalid demo index: " << demo_index << std::endl;
        return;
    }
    m_currentDemo = demo_index;
    particleSim_loadDemoParticles(m_currentDemo, m_pSet._particles);
    m_vbo.bind();
    m_vbo.bufferData(m_pSet._particles.data(),
        m_pSet._particles.size() * sizeof(fgt::Particle<float>),
        GL_DYNAMIC_DRAW);
    m_vbo.unbind();
    std::cout << "Loaded demo: " << particleSim_getDemoName(m_currentDemo) << std::endl;
}

void ParticleSimulation::init()
{
    m_vao.genVAO();
    m_vbo.genVB();

    //Bind

    m_vao.bind();

    m_vbo.bind();
    //m_vbo.bufferData(m_pSet._particles.data(),m_pSet._particles.size()*sizeof(fgt::Particle<float>),GL_DYNAMIC_DRAW);
    m_vbo.bufferData(m_pSet._particles.data(),m_pSet._particles.size()*sizeof(fgt::Particle<float>),GL_DYNAMIC_DRAW); 

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(fgt::Particle<float>), (void*)offsetof(fgt::Particle<float>, position));
    glEnableVertexAttribArray(0);

    m_vao.unbind();
}

void ParticleSimulation::draw()
{
    this->simulation();
    glEnable(GL_PROGRAM_POINT_SIZE);
    m_vao.bind();
    glDrawArrays(GL_POINTS, 0, m_NumParticles);
}

Shader &ParticleSimulation::getShader()
{
    return *m_shader;
}

void ParticleSimulation::updateTime(float deltaTime)
{
    this->m_deltaTime = deltaTime; 
}

void ParticleSimulation::setViewMatrix(const glm::mat4 &viewMatrix)
{
    m_viewMatrix = viewMatrix;
}

glm::mat4 ParticleSimulation::getViewMatrix()
{
    return m_viewMatrix;
}
void ParticleSimulation::updateModelMatrix()
{
    m_ModelMatrix = glm::mat4(1.f);
    m_ModelMatrix = glm::translate(m_ModelMatrix, m_position);
    m_ModelMatrix = glm::rotate(m_ModelMatrix, glm::radians(m_rotation.x), glm::vec3(1.f, 0.f, 0.f));
    m_ModelMatrix = glm::rotate(m_ModelMatrix, glm::radians(m_rotation.y), glm::vec3(0.f, 1.f, 0.f));
    m_ModelMatrix = glm::rotate(m_ModelMatrix, glm::radians(m_rotation.z), glm::vec3(0.f, 0.f, 1.f));
    m_ModelMatrix = glm::scale(m_ModelMatrix, m_scale);
}
glm::mat4 ParticleSimulation::getModelMatrix() const
{
    return m_ModelMatrix;
}
void ParticleSimulation::simulation()
{
    int numParticles = m_pSet._particles.size();
 
   particleSim_runDemo(m_currentDemo, numParticles, m_vbo.getId(), 0.005f);
}
