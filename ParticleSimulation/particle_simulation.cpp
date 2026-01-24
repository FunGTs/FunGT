#include "particle_simulation.hpp"

ParticleSimulation::ParticleSimulation(size_t num, std::string vertex_shader, std::string fragment_shader)
: m_NumParticles{num}{
    // INITIALIZE SYCL WITH GL INTEROP (ParticleSimulation owns this responsibility)
    std::cout << "Initializing SYCL for ParticleSimulation..." << std::endl;
    flib::sycl_handler::select_backend_device("OpenCL", "GPU");
    flib::sycl_handler::create_gl_interop_context();
    flib::sycl_handler::get_device_info();
    m_pSet.SetNumParticles(m_NumParticles);
    std::cout<<"Particle system constructor"<<std::endl;
    std::cout<<"Num particles: "<<m_pSet._particles.size()<<std::endl;
   
    loadDemo(4);
    //Print just the position of the firs 2 particles
    std::cout << "Particle positions:" << std::endl;
    for (std::size_t i = 0; i < 2; ++i) {
        std::cout << "Particle "<< i << ": ("
                  << m_pSet._particles[i].position[0] << ", "
                  << m_pSet._particles[i].position[1] << ", "
                  << m_pSet._particles[i].position[2] << ")" << std::endl;
    }
    //m_pSet.print();
    this->init();
    m_shader.create(vertex_shader,fragment_shader);
}

void ParticleSimulation::loadDemo(int demo_index)
{
    if (demo_index < 0 || demo_index >= fgt::demoInits.size()) {
        std::cerr << "Invalid demo index: " << demo_index << std::endl;
        return;
    }

    m_currentDemo = demo_index;
    fgt::demoInits[m_currentDemo](m_pSet._particles);
    // UPLOAD NEW POSITIONS TO GPU VBO!
    m_vbo.bind();
    m_vbo.bufferData(m_pSet._particles.data(),
        m_pSet._particles.size() * sizeof(flib::Particle<float>),
        GL_DYNAMIC_DRAW);
    m_vbo.unbind();
   
    std::cout << "Loaded demo: " << fgt::demoNames[m_currentDemo] << std::endl;
}

void ParticleSimulation::init()
{
    m_vao.genVAO();
    m_vbo.genVB();

    //Bind

    m_vao.bind();

    m_vbo.bind();
    //m_vbo.bufferData(m_pSet._particles.data(),m_pSet._particles.size()*sizeof(flib::Particle<float>),GL_DYNAMIC_DRAW);
    m_vbo.bufferData(m_pSet._particles.data(),m_pSet._particles.size()*sizeof(flib::Particle<float>),GL_DYNAMIC_DRAW); 

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(flib::Particle<float>), (void*)offsetof(flib::Particle<float>, position));
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
    // TODO: insert return statement here
    return m_shader;
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

    switch (m_currentDemo) {
    case 0:
        flib::ParticleSystem<float, decltype(fgt::spiralExplosionUpdate)>::update(
            numParticles, m_vbo.getId(), fgt::spiralExplosionUpdate, 0.005f);
        break;
    case 1:
        flib::ParticleSystem<float, decltype(fgt::blackHoleUpdate)>::update(
            numParticles, m_vbo.getId(), fgt::blackHoleUpdate, 0.005f);
        break;
    case 2:
        flib::ParticleSystem<float, decltype(fgt::vortexUpdate)>::update(
            numParticles, m_vbo.getId(), fgt::vortexUpdate, 0.005f);
        break;
    case 3:
        flib::ParticleSystem<float, decltype(fgt::fireworkUpdate)>::update(
            numParticles, m_vbo.getId(), fgt::fireworkUpdate, 0.005f);
        break;
    case 4:
        flib::ParticleSystem<float, decltype(fgt::waveUpdate)>::update(
            numParticles, m_vbo.getId(), fgt::waveUpdate, 0.005f);
        break;
    case 5:
        flib::ParticleSystem<float, decltype(fgt::smokeUpdate)>::update(
            numParticles, m_vbo.getId(), fgt::smokeUpdate, 0.005f);
        break;
    default:
        flib::ParticleSystem<float, decltype(fgt::spiralExplosionUpdate)>::update(
            numParticles, m_vbo.getId(), fgt::spiralExplosionUpdate, 0.005f);
        break;
    }

}
