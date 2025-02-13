#if !defined(_NOISE_PARTICLE_H_)
#define _NOISE_PARTICLE_H_
#include "../../PerlinNoise/perlin_noise.hpp"
#include "particle_sys.hpp"
class NoiseParticles : public ParticleSystem {

    double xoffset = 0.0;
    double yoffset = 10000.0;
    double zoffset = 0.0;

    public:
        NoiseParticles(size_t num, std::string vs, std::string fs);
        void updateParticles() override;
        void applyForce(int index_particle) override;
        void resetParticle(int index_particle) override;
        void moveParticle(int index_particle) override;
};

#endif // _NOISE_PARTICLE_H_
