#if !defined(_PARTICLE_SET_H_)
#define _PARTICLE_SET_H_
#include <vector>
namespace flib
{   
    template <typename T>
    struct Particle
    {
        T position[3];
        T velocity[3];
        T acceleration[3];
        T mass;

        Particle()
        {
            for (int i = 0; i < 3; i++)
            {
                position[i] = 0;
                velocity[i] = 0;
                acceleration[i] = 0;
            }
            mass = 1.0f; // Default mass
        }
    };
    template <typename T>
    class ParticleSet
    {
        public:
            std::vector<Particle<T>> _particles;
        public:
            ParticleSet(){};
            ParticleSet(std::size_t n){
                _particles.resize(n);
            }
            void SetNumParticles(std::size_t n)
            {
                _particles.resize(n);
            }
        void print()
        {
            for (std::size_t i = 0; i < _particles.size(); i++)
            {
                //std::cout << "Particle " << i << ": ";
                std::cout << "Position: (" << _particles[i].position[0] << ", " << _particles[i].position[1] << ", " << _particles[i].position[2] << "), ";
                std::cout << "Velocity: (" << _particles[i].velocity[0] << ", " << _particles[i].velocity[1] << ", " << _particles[i].velocity[2] << "), ";
                //std::cout << "Acceleration: (" << _particles[i].acceleration[0] << ", " << _particles[i].acceleration[1] << ", " << _particles[i].acceleration[2] << "), ";
                //std::cout << "Mass: " << _particles[i].mass;
                std::cout << std::endl;
            }
        }
    
    };
} // namespace flib


#endif // _PARTICLE_SET_H_
