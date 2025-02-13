#include "random_particle.hpp"
RandomParticles::RandomParticles(size_t num, std::string vs, std::string fs)
: ParticleSystem(num,vs,fs)
{
    std::cout<<"Random Particle constructor"<<std::endl;

}
void RandomParticles::updateParticles()
{
    for(int i = 0; i<m_particles.size(); i++)
    {
        
        applyForce(i);
        moveParticle(i);
        m_particles[i].lifesPan -= 2;
        if(m_particles[i].lifesPan <= 0){ // Is the particle dead?
            //std::cout << "Particle is dead" << std::endl;
            resetParticle(i);
        }

    }
}
void RandomParticles::applyForce(int index_particle)
{
    std::array<float, 3> force = {0.f,-0.000001f,0.0f};
    for(int j=0; j<3; j++){
        m_particles[index_particle].acceleration[j] += force[j];
    }
}

void RandomParticles::resetParticle(int index_particle)
{
    m_particles[index_particle].lifesPan = 255;
    std::array<float, 3> rndMove = {0.0f,0.f,0.0f};
    rndMove[0] = random(-0.5f,0.5f);
    rndMove[1] = random(-0.5f,0.0f);
    rndMove[2] = 0.0f;
    for(int j=0; j<3; j++){
        m_particles[index_particle].pos[j] = m_startPosition[j];
        m_particles[index_particle].vel[j] = rndMove[j];
        m_particles[index_particle].acceleration[j] *= 0.0;
    }
}

void RandomParticles::moveParticle(int index_particle)
{
    for(int j=0; j<3; j++){
        //m_particles[index_particle].vel[j] = rndMove[j];
        m_particles[index_particle].vel[j] += m_particles[index_particle].acceleration[j];
        m_particles[index_particle].pos[j] += m_particles[index_particle].vel[j];
        //m_particles[index_particle].acceleration[j] *= 0.0;
    }
}
