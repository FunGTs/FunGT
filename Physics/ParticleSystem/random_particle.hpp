#if !defined(_RANDOM_PARTICLE_H_)
#define _RANDOM_PARTICLE_H_
#include "particle_sys.hpp"

class RandomParticles : public ParticleSystem {
    public:
        RandomParticles(size_t num, std::string vs, std::string fs);
        void updateParticles() override;
        void applyForce(int index_particle) override;
        void resetParticle(int index_particle) override;
        void moveParticle(int index_particle) override;
};


#endif // _RANDOM_PARTICLE_H_
