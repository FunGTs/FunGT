#if !defined(_PARTICLE_SIM_SYCL_OPS_H_)
#define _PARTICLE_SIM_SYCL_OPS_H_
#include "particle.hpp"
#include <vector>
#include <string>

void particleSim_initSycl();
void particleSim_runDemo(int demo, int numParticles, unsigned int vboId, float dt);
void particleSim_loadDemoParticles(int demo, std::vector<fgt::Particle<float>>& particles);
int particleSim_getDemoCount();
std::string particleSim_getDemoName(int demo);
void particleSim_initSycl();
void particleSim_runDemo(int demo, int numParticles, unsigned int vboId, float dt);

#endif // _PARTICLE_SIM_SYCL_OPS_H_