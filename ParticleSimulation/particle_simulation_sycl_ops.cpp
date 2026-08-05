#include "ParticleSimulation/particle_system.hpp"
#include "ParticleSimulation/particle_demos.hpp"

void particleSim_initSycl() {
    flib::sycl_handler::register_queue("gl_queue", flib::device::GPU, flib::vendor::INTEL, flib::backend::OPENCL);
    flib::sycl_handler::create_gl_interop_context("gl_queue");
    flib::sycl_handler::get_device_info("gl_queue");
}

void particleSim_runDemo(int demo, int numParticles, unsigned int vboId, float dt) {
    switch (demo) {
    case 0: fgt::ParticleSystem<float, decltype(fgt::spiralExplosionUpdate)>::update(numParticles, vboId, fgt::spiralExplosionUpdate, dt); break;
    case 1: fgt::ParticleSystem<float, decltype(fgt::blackHoleUpdate)>::update(numParticles, vboId, fgt::blackHoleUpdate, dt); break;
    case 2: fgt::ParticleSystem<float, decltype(fgt::vortexUpdate)>::update(numParticles, vboId, fgt::vortexUpdate, dt); break;
    case 3: fgt::ParticleSystem<float, decltype(fgt::fireworkUpdate)>::update(numParticles, vboId, fgt::fireworkUpdate, dt); break;
    case 4: fgt::ParticleSystem<float, decltype(fgt::waveUpdate)>::update(numParticles, vboId, fgt::waveUpdate, dt); break;
    case 5: fgt::ParticleSystem<float, decltype(fgt::smokeUpdate)>::update(numParticles, vboId, fgt::smokeUpdate, dt); break;
    default: fgt::ParticleSystem<float, decltype(fgt::spiralExplosionUpdate)>::update(numParticles, vboId, fgt::spiralExplosionUpdate, dt); break;
    }
}
void particleSim_loadDemoParticles(int demo, std::vector<fgt::Particle<float>>& particles) {
    fgt::demoInits[demo](particles);
}

int particleSim_getDemoCount() {
    return fgt::demoInits.size();
}

std::string particleSim_getDemoName(int demo) {
    return fgt::demoNames[demo];
}