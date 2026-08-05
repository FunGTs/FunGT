#if !defined(_PARTICLE_RTC_SYCL_OPS_H_)
#define _PARTICLE_RTC_SYCL_OPS_H_
#include <string>
#include <cstddef>
 
void particleRTC_initSycl();
bool particleRTC_compileInitKernel(const std::string& user_init_code, std::string& error_msg);
bool particleRTC_compileKernel(const std::string& user_code, std::string& error_msg);
void particleRTC_executeInitKernel(unsigned int vboId, std::size_t numParticles);
void particleRTC_executeKernel(unsigned int vboId, std::size_t numParticles, float dt);
bool particleRTC_hasKernel();
bool particleRTC_hasInitKernel();
bool particleRTC_isSupported();
 
#endif // _PARTICLE_RTC_SYCL_OPS_H_
 
