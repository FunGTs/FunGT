#include "../include/compute_backends.hpp"


Compute::Backend ComputeRender::s_API = Compute::Backend::CPU; // default backend


void ComputeRender::Init() {
#if defined(FUNGT_USE_CUDA)
    s_API = Compute::Backend::CUDA;
    std::cout << "[ComputeRender] Using CUDA backend\n";
#elif defined(FUNGT_USE_SYCL)
    s_API = Compute::Backend::SYCL;
    std::cout << "[ComputeRender] Using SYCL backend\n";
#else
    s_API = Compute::Backend::CPU;
    std::cout << "[ComputeRender] Using CPU backend\n";
#endif
}
const std::string ComputeRender::GetBackendName() {
    switch (s_API) {
    case Compute::Backend::CUDA: return "CUDA";
    case Compute::Backend::SYCL: return "SYCL";
    case Compute::Backend::CPU:  return "CPU";
    default: return "Unknown";
    }
}
