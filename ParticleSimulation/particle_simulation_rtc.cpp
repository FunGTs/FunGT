#include "particle_simulation_rtc.hpp"

namespace syclexp = sycl::ext::oneapi::experimental;

ParticleRTC::ParticleRTC(sycl::queue& q) : m_queue(q) {}

bool ParticleRTC::isSupported(const sycl::device& dev) {
    return flib::sycl_handler::is_rtc_available();
}

bool ParticleRTC::compileForceKernel(const std::string& user_code, std::string& error_msg) {
    return false;
}

sycl::kernel* ParticleRTC::getKernel() {
    return m_has_kernel ? &m_compiled_kernel : nullptr;
}