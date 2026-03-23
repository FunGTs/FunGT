#include "particle_simulation.hpp"

ParticleSimulation::ParticleSimulation(size_t num, std::string vertex_shader, std::string fragment_shader, bool use_rtc)
: m_NumParticles{num}{
    
    m_pSet.SetNumParticles(m_NumParticles);
    std::cout << "Particle system constructor" << std::endl;
    std::cout << "Num particles: " << m_pSet._particles.size() << std::endl;

    bool m_useRTC  = use_rtc;
    bool allowRTC = false;
    // Try RTC first
    if(m_useRTC){

        if (ParticleRTC::isSupported()) {
            m_rtc = std::make_unique<ParticleRTC>();
            std::cout << "ParticleRTC initialized successfully\n";

            std::string init_code = R"""(
            float theta = index * 0.061803f;
            float phi = index * 0.123f;
            float speed = 2.0f + (index % 100) * 0.01f;
            
            p.position[0] = 0.0f;
            p.position[1] = 0.0f;
            p.position[2] = 0.0f;
            
            p.velocity[0] = speed * sycl::sin(phi) * sycl::cos(theta);
            p.velocity[1] = speed * sycl::sin(phi) * sycl::sin(theta);
            p.velocity[2] = speed * sycl::cos(phi);
        )""";

            std::string update_code = R"""(
            constexpr float gravity = -2.0f;
            constexpr float drag = 0.99f;
            p.velocity[2] += gravity * dt;
            p.velocity[0] *= drag;
            p.velocity[1] *= drag;
            p.velocity[2] *= drag;
            p.position[0] += p.velocity[0] * dt;
            p.position[1] += p.velocity[1] * dt;
            p.position[2] += p.velocity[2] * dt;
        )""";

            std::string error;
            bool init_ok = m_rtc->compileInitKernel(init_code, error);
            bool update_ok = m_rtc->compileKernel(update_code, error);

            if (init_ok && update_ok) {
                std::cout << "Using RTC path\n";
                this->initRTC();
            }
            else {
                std::cerr << "RTC compilation failed: " << error << "\n";
                std::cout << "Falling back to compile-time demos\n";
            }
        }
        else {
            std::cout << "RTC not supported, using compile-time demos\n";
        }
    }
    else{

        loadDemo(3);
        this->init();
    }
    m_shader.create(vertex_shader, fragment_shader);
}

void ParticleSimulation::loadDemo(int demo_index)
{
    if (demo_index < 0 || demo_index >= fgt::demoInits.size()) {
        std::cerr << "Invalid demo index: " << demo_index << std::endl;
        return;
    }

    m_currentDemo = demo_index;
    
    fgt::demoInits[m_currentDemo](m_pSet._particles);
    std::cout << "Loaded demo: " << fgt::demoNames[m_currentDemo] << std::endl;
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
void ParticleSimulation::initRTC()
{
    m_vao.genVAO();
    m_vbo.genVB();

    m_vao.bind();
    m_vbo.bind();

    // Allocate empty VBO (no CPU data needed for RTC)
    m_vbo.bufferData(nullptr,
        m_pSet._particles.size() * sizeof(fgt::Particle<float>),
        GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
        sizeof(fgt::Particle<float>),
        (void*)offsetof(fgt::Particle<float>, position));
    glEnableVertexAttribArray(0);

    m_vao.unbind();

    // GPU kernel fills the VBO
    if (m_rtc && m_rtc->hasInitKernel()) {
        m_rtc->executeInit(m_pSet._particles.size(), m_vbo.getId());
    }
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
glm::mat4 ParticleSimulation::getModelMatrix()
{
    return m_ModelMatrix;
}
void ParticleSimulation::simulation()
{
    int numParticles = m_pSet._particles.size();
    // Use RTC kernel if available
    if (m_rtc && m_rtc->hasKernel()) {
        // Print BEFORE
        
        m_rtc->execute(numParticles, m_vbo.getId(), 0.005f);

        return;
    }

    switch (m_currentDemo) {
    case 0:
        fgt::ParticleSystem<float, decltype(fgt::spiralExplosionUpdate)>::update(
            numParticles, m_vbo.getId(), fgt::spiralExplosionUpdate, 0.005f);
        break;
    case 1:
        fgt::ParticleSystem<float, decltype(fgt::blackHoleUpdate)>::update(
            numParticles, m_vbo.getId(), fgt::blackHoleUpdate, 0.005f);
        break;
    case 2:
        fgt::ParticleSystem<float, decltype(fgt::vortexUpdate)>::update(
            numParticles, m_vbo.getId(), fgt::vortexUpdate, 0.005f);
        break;
    case 3:
        fgt::ParticleSystem<float, decltype(fgt::fireworkUpdate)>::update(
            numParticles, m_vbo.getId(), fgt::fireworkUpdate, 0.005f);
        break;
    case 4:
        fgt::ParticleSystem<float, decltype(fgt::waveUpdate)>::update(
            numParticles, m_vbo.getId(), fgt::waveUpdate, 0.005f);
        break;
    case 5:
        fgt::ParticleSystem<float, decltype(fgt::smokeUpdate)>::update(
            numParticles, m_vbo.getId(), fgt::smokeUpdate, 0.005f);
        break;
    default:
        fgt::ParticleSystem<float, decltype(fgt::spiralExplosionUpdate)>::update(
            numParticles, m_vbo.getId(), fgt::spiralExplosionUpdate, 0.005f);
        break;
    }

}
