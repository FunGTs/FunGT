#include "../include/compute_backends.hpp"


Compute::Backend ComputeRender::s_API = Compute::Backend::CPU; // default backend

const std::string ComputeRender::GetBackendName() {
    switch (s_API) {
    case Compute::Backend::CUDA: return "CUDA";
    case Compute::Backend::SYCL: return "SYCL";
    case Compute::Backend::SYCL_CUDA: return "SYCL_CUDA";
    case Compute::Backend::CPU:  return "CPU";
    default: return "Unknown";
    }
}
