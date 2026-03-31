#pragma once
#include <funlib/funlib.hpp>
#include <string>
#include <CL/cl_gl.h>
namespace syclexp = sycl::ext::oneapi::experimental;

class ParticleRTC {
public:
    ParticleRTC(sycl::queue& q);

    // Compile user lambda code
    bool compileKernel(const std::string& user_code, std::string& error_msg);

    // Execute compiled kernel on particles
    void execute(int numParticles, unsigned int vbo, float dt);

    // Check if device supports RTC
    static bool isSupported(const sycl::device& dev);

    bool hasKernel() const { return has_kernel_; }

private:
    sycl::queue& queue_;
    sycl::kernel compiled_kernel_;
    bool has_kernel_ = false;
};