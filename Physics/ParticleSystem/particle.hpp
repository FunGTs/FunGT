#if !defined(_PARTICLE_H_)
#define _PARTICLE_H_
#include<vector>
#include<cmath>
#include <array>
#include "../../Random/random.hpp"
struct Particle{

    
    std::array<float, 3> pos;
    std::array<float, 3> vel;
    std::array<float, 3> acceleration;
    int lifesPan = 255; 

    Particle();
    void reset(int seed);
    void cpu_reset();
    bool isAlive()const;



};
#endif // _PARTICLE_H_
