#include "particle_sys.hpp"

ParticleSystem::ParticleSystem(size_t num,std::string vs_Path,std::string fs_Path)
:m_NumParticles{num}{
    m_particles.resize(m_NumParticles);

    std::cout<<"Particle system constructor"<<std::endl;
    for(int i = 0; i<m_particles.size(); i++){
     std::array<float, 3> rndMove = {0.0f,0.f,0.0f};
        rndMove[0] = random(-1.f,1.f);
        rndMove[1] = random(-1.f,1.1f);
        rndMove[2] = 0.0f;
        for(int j=0; j<3; j++){
            m_particles[i].pos[j] = m_startPosition[j];
            m_particles[i].vel[j] = rndMove[j];
            m_particles[i].acceleration[j] *= 0.0;
        }
    }
    this->init();
    m_shader.create(vs_Path,fs_Path);
    
}

void ParticleSystem::init()
{
   

    m_vao.genVAO();
    m_vbo.genVB();

    //Bind

    m_vao.bind();

    m_vbo.bind();
    m_vbo.bufferData(m_particles.data(),m_particles.size()*sizeof(Particle),GL_DYNAMIC_DRAW); 

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)offsetof(Particle, pos));
    glEnableVertexAttribArray(0);

    m_vao.unbind();
}
void ParticleSystem::update()
{

    this->updateParticles();

    m_vbo.bind();
    m_vbo.bufferSubData(m_particles.data(),m_particles.size()*sizeof(Particle));
}
void ParticleSystem::updateParticles(){
}

void ParticleSystem::applyForce(int index_particle){
}

void ParticleSystem::resetParticle(int index_particle){
}
void ParticleSystem::moveParticle(int index_particle){
}
void ParticleSystem::draw()
{
    this->update();
    glEnable(GL_PROGRAM_POINT_SIZE);
    m_vao.bind();
    glDrawArrays(GL_POINTS, 0, m_NumParticles);

}

Shader &ParticleSystem::getShader()
{
   return m_shader;
}

void ParticleSystem::updateTime(float deltaTime)
{
    this->m_deltaTime = deltaTime; 
}

void ParticleSystem::setViewMatrix(const glm::mat4 &viewMatrix)
{
    this->m_viewMatrix = viewMatrix;
}

glm::mat4 ParticleSystem::getViewMatrix()
{
    return m_viewMatrix;
}
