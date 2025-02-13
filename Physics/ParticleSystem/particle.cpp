#include "particle.hpp"

Particle::Particle(){
    for(int j=0; j<3; j++){
        pos[j] = 0.0f;
        vel[j] = 0.0f;
        acceleration[j] = 0.0f;
    }
}

    void Particle::reset(int seed) {
       //lifesPan = 5.0f;
       for(int j=0; j<3; j++){
            pos[j] = randomFloat(seed+j)*2.0f-1.0f;
            vel[j] = randomFloat(seed+j)*2.0f-1.0f;
       }
    }
    void Particle::cpu_reset(){
        lifesPan = 5.0f;
        //for(int j=0; j<3; j++){
            pos[0] = random(-1.0f,1.0f);
            pos[1] = random(-1.0f,1.0f);
            pos[2] = 0.0f;
            vel[0] = random(-1.0f,1.0f);
            vel[1] = random(-1.0f,1.0f);
            vel[2] = 0.0f;
        //}
    }
    bool Particle::isAlive() const { 
        return lifesPan > 0.0f;
    }
 

