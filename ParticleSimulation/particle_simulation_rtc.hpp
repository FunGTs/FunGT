#pragma once
#include <funlib/funlib.hpp>
#include <string>
#include <unordered_map>

class ParticleRTC {
public:
    ParticleRTC(sycl::queue& q);

    // Compile user force code, returns true on success
    bool compileForceKernel(const std::string& user_code, std::string& error_msg);

    // Get the compiled kernel (nullptr if not compiled)
    sycl::kernel* getKernel();

    static bool isSupported(const sycl::device& dev);

private:
    sycl::queue& m_queue;
    sycl::kernel m_compiled_kernel;
    bool m_has_kernel = false;
};