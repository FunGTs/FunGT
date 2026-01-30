#include "particle_simulation_rtc.hpp"

ParticleRTC::ParticleRTC(sycl::queue& q) : queue_(q) {}

  

bool ParticleRTC::isSupported() {
    return flib::sycl_handler::is_rtc_available();;
}

bool ParticleRTC::compileKernel(const std::string& user_code, std::string& error_msg) {
    try {
        std::string user_header = R"""(
// Minimal Particle definition (no OpenGL dependencies)
namespace flib {
    template<typename T>
    struct Particle {
        T position[3];
        T velocity[3];
        T acceleration[3];
        T color[4];
        T mass;
        T lifespan;
        T age;
    };
}

struct UserUpdate {
    void operator()(flib::Particle<float>& p, float dt) const {
        )""" + user_code + R"""(
    }
};
)""";


        static constexpr auto sycl_source = R"""(
#include <sycl/sycl.hpp>
#include "user_update.h"

extern "C" SYCL_EXT_ONEAPI_FUNCTION_PROPERTY((
    sycl::ext::oneapi::experimental::nd_range_kernel<2>))
void particle_rtc_kernel(flib::Particle<float>* particles, int n, int ydim, float dt) {
    auto item = sycl::ext::oneapi::this_work_item::get_nd_item<2>();
    std::size_t index = item.get_global_id(0) * ydim + item.get_global_id(1);
    
    if (index < n) {
        UserUpdate update;
        update(particles[index], dt);
    }
}
)""";

        syclexp::include_files includes{ "user_update.h", user_header };
        auto source_bundle = syclexp::create_kernel_bundle_from_source(
            queue_.get_context(),
            syclexp::source_language::sycl,
            sycl_source,
            syclexp::properties{ includes }
        );

        syclexp::build_options opts{ "-fsycl" };
        std::string compiler_log;

        syclexp::save_log log{ &compiler_log };

        auto exec_bundle = syclexp::build(source_bundle, syclexp::properties{ opts,log });

        std::cout << "RTC Compiler output:\n" << compiler_log << "\n";

        compiled_kernel_ = exec_bundle.ext_oneapi_get_kernel("particle_rtc_kernel");
        has_kernel_ = true;

        return true;

    }
    catch (sycl::exception& e) {
        error_msg = std::string("SYCL RTC failed: ") + e.what();
        has_kernel_ = false;
        return false;
    }
}

void ParticleRTC::execute(int numParticles, unsigned int vbo, float dt) {
    if (!has_kernel_) {
        throw std::runtime_error("No compiled kernel available");
    }

    std::size_t n = static_cast<std::size_t>(numParticles);
    std::size_t xdim = static_cast<std::size_t>(std::ceil(std::sqrt(n)));
    std::size_t ydim = xdim;
    // Extract kernel from optional BEFORE using it
    sycl::kernel& kernel_ref = compiled_kernel_.value();
    cl_context clcontext = flib::sycl_handler::get_clContext();
    cl_command_queue clqueue = sycl::get_native<sycl::backend::opencl>(queue_);
    cl_mem clbuffer = clCreateFromGLBuffer(clcontext, CL_MEM_READ_WRITE, vbo, NULL);
    if (clbuffer == NULL) {
        throw std::runtime_error("Failed to create OpenCL buffer from GL buffer");
    }
    sycl::context syclCtx = flib::sycl_handler::get_sycl_context();

    glFinish();
    cl_event acquire_event;
    clEnqueueAcquireGLObjects(clqueue, 1, &clbuffer, 0, NULL, &acquire_event);
    clWaitForEvents(1, &acquire_event);

    {
        sycl::buffer<flib::Particle<float>> buf =
            sycl::make_buffer<sycl::backend::opencl, flib::Particle<float>>(clbuffer, syclCtx);

        queue_.submit([&](sycl::handler& cgh) {
            auto acc = buf.template get_access<sycl::access::mode::read_write>(cgh);
            cgh.set_args(acc.get_pointer(), static_cast<int>(n), static_cast<int>(ydim), dt);
            cgh.parallel_for(sycl::range<2>{xdim, ydim}, kernel_ref);
            });
        queue_.wait();
    }
    clFinish(clqueue);

    cl_event release_event;
    clEnqueueReleaseGLObjects(clqueue, 1, &clbuffer, 0, NULL, &release_event);
    clWaitForEvents(1, &release_event);
    clReleaseMemObject(clbuffer);
}