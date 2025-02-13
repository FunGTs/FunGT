#include "noise_particle.hpp"

NoiseParticles::NoiseParticles(size_t num, std::string vs, std::string fs)
: ParticleSystem(num,vs,fs)
{
    std::cout<<"Noise Particle constructor"<<std::endl;

}

void NoiseParticles::updateParticles()
{
    for(int i = 0; i<m_particles.size(); i++)
    {
            
        moveParticle(i);
        //if particle is out of width bounds put it back to the other side
        if(m_particles[i].pos[0] > 1.0 || m_particles[i].pos[0] < -1.0){
            m_particles[i].pos[0] = -m_particles[i].pos[0];
        }
        //if particle is out of height bounds put it back to the other side
        if(m_particles[i].pos[1] > 1.0 || m_particles[i].pos[1] < -1.0){
            m_particles[i].pos[1] = -m_particles[i].pos[1];
        }
     

    }
}

void NoiseParticles::applyForce(int index_particle)
{
   
}

void NoiseParticles::resetParticle(int index_particle)
{
}

void NoiseParticles::moveParticle(int index_particle)
{
    double scale = 0.1;
    double xValue = PerlinNoise::noise3d(xoffset*scale, 1000.0,0.1);
    double yValue = PerlinNoise::noise3d(0.0, yoffset*scale,0.1);

    // Passing to the range [-1,1]
    std::array<float, 3> noiseMove = {0.0f, 0.0f, 0.0f};
    noiseMove[0] = xValue * 2.0 - 1.0;
    noiseMove[1] = yValue * 2.0 - 1.0;
    noiseMove[2] = 0.0f;

    //std::cout << "Noise Move: " << noiseMove[0] << " " << noiseMove[1] << " " << noiseMove[2] << std::endl;

    for (int j = 0; j < 3; j++) {
        m_particles[index_particle].vel[j] = noiseMove[j];
        m_particles[index_particle].pos[j] = noiseMove[j];
        m_particles[index_particle].acceleration[j] *= 0.0;
    }
    xoffset += 0.1;
    yoffset += 0.1;

}
