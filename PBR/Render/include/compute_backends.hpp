#if !defined(_COMPUTE_BACKENDS)
#define _COMPUTE_BACKENDS
#include <iostream>
#include <string>
namespace Compute{
    enum class Backend {
        CPU,
        CUDA,
        SYCL,
        SYCL_CUDA, // SYCL targeting CUDA devices
        OPENCL,
    };
}
class ComputeRender {
public:
    static const std::string GetBackendName();
    static void SetBackend(Compute::Backend api) { s_API = api; }
    static Compute::Backend GetBackend() { return s_API; }

private:
    static Compute::Backend s_API;
};



#endif // _COMPUTE_BACKENDS
