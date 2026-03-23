#if !defined(_PARTICLE_SIM_RTC_H_)
#define _PARTICLE_SIM_RTC_H_
#include <GL/glew.h>
#include <funlib/funlib.hpp>
#include <string>
#include <optional>
#include <CL/cl_gl.h>
namespace syclexp = sycl::ext::oneapi::experimental;

class ParticleRTC {
public:
    ParticleRTC();

    bool compileInitKernel(const std::string& user_init_code, std::string& error_msg);
    void executeInit(int numParticles, unsigned int vbo);
    // Compile user lambda code
    bool compileKernel(const std::string& user_code, std::string& error_msg);
    
    // Execute compiled kernel on particles
    void execute(int numParticles, unsigned int vbo, float dt);

    // Check if device supports RTC
    static bool isSupported();

    bool hasKernel() const { return has_kernel_; }
    bool hasInitKernel() const { return compiled_init_kernel_.has_value(); }

private:
    //sycl::queue& queue_;
    std::optional<sycl::kernel> compiled_kernel_;
    std::optional<sycl::kernel> compiled_init_kernel_;
    bool has_kernel_ = false;
};

#endif // _PARTICLE_SIM_RTC_H_
