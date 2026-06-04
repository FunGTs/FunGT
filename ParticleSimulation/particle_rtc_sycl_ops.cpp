#include "particle_rtc_sycl_ops.hpp"
#include <GL/glew.h>
#include <sycl/sycl.hpp>
#include <funlib/funlib.hpp>
#include "ParticleSimulation/particle.hpp"
#include "Path_Manager/path_manager.hpp"
#include <optional>
#include <cmath>
#include <iostream>
#include <CL/cl_gl.h>

namespace syclexp = sycl::ext::oneapi::experimental;

static std::optional<sycl::kernel> s_compiledInitKernel;
static std::optional<sycl::kernel> s_compiledKernel;
static bool s_hasKernel = false;

void particleRTC_initSycl() {
    flib::sycl_handler::register_queue("gl_queue", flib::device::GPU, flib::vendor::INTEL, flib::backend::OPENCL);
    flib::sycl_handler::create_gl_interop_context("gl_queue");
    flib::sycl_handler::get_device_info("gl_queue");
}

bool particleRTC_compileInitKernel(const std::string& user_init_code, std::string& error_msg) {
    try {
        sycl::queue queue_ = flib::sycl_handler::get_queue("gl_queue");

        std::string user_header = R"""(
struct UserInit {
    void operator()(fgt::Particle<float>& p, int index) const {
        )""" + user_init_code + R"""(
    }
};
)""";

        static constexpr auto sycl_source = R"""(
#include <sycl/sycl.hpp>
#include "ParticleSimulation/particle.hpp"
#include "Random/fgt_rng.hpp"
#include "user_init.h"

extern "C" SYCL_EXT_ONEAPI_FUNCTION_PROPERTY((
    sycl::ext::oneapi::experimental::nd_range_kernel<2>))
void particle_init_kernel(fgt::Particle<float>* particles, int n, int ydim) {
    auto item = sycl::ext::oneapi::this_work_item::get_nd_item<2>();
    std::size_t index = item.get_global_id(0) * ydim + item.get_global_id(1);
    
    if (index < n) {
        UserInit init;
        init(particles[index], index);
    }
}
)""";

        syclexp::include_files includes{ "user_init.h", user_header };
        std::string include_path = "-I" + getAssetPath("");
        syclexp::build_options opts{ "-fsycl " + include_path };
        std::string compiler_log;
        syclexp::save_log log{ &compiler_log };

        auto source_bundle = syclexp::create_kernel_bundle_from_source(
            queue_.get_context(),
            syclexp::source_language::sycl,
            sycl_source,
            syclexp::properties{ includes }
        );
        auto exec_bundle = syclexp::build(source_bundle, syclexp::properties{ opts, log });
        s_compiledInitKernel = exec_bundle.ext_oneapi_get_kernel("particle_init_kernel");

        return true;

    }
    catch (sycl::exception& e) {
        error_msg = std::string("Init kernel RTC failed: ") + e.what();
        s_compiledInitKernel.reset();
        return false;
    }
}

bool particleRTC_compileKernel(const std::string& user_code, std::string& error_msg) {
    try {
        sycl::queue queue_ = flib::sycl_handler::get_queue("gl_queue");

        std::string user_header = R"""(
        struct UserUpdate {
            void operator()(fgt::Particle<float>& p, float dt) const {
                )""" + user_code + R"""(
            }
        };
        )""";

        static constexpr auto sycl_source = R"""(
        #include <sycl/sycl.hpp>
        #include "ParticleSimulation/particle.hpp"
        #include "Random/fgt_rng.hpp"
        #include "user_update.h"

        extern "C" SYCL_EXT_ONEAPI_FUNCTION_PROPERTY((
            sycl::ext::oneapi::experimental::nd_range_kernel<2>))
        void particle_rtc_kernel(fgt::Particle<float>* particles, int n, int ydim, float dt) {
            auto item = sycl::ext::oneapi::this_work_item::get_nd_item<2>();
            std::size_t index = item.get_global_id(0) * ydim + item.get_global_id(1);
            
            if (index < n) {
                UserUpdate update;
                update(particles[index], dt);
            }
        }
        )""";

        syclexp::include_files includes{ "user_update.h", user_header };
        std::string include_path = "-I" + getAssetPath("");
        syclexp::build_options opts{ "-fsycl " + include_path };
        std::string compiler_log;
        syclexp::save_log log{ &compiler_log };

        auto source_bundle = syclexp::create_kernel_bundle_from_source(
            queue_.get_context(),
            syclexp::source_language::sycl,
            sycl_source,
            syclexp::properties{ includes }
        );
        auto exec_bundle = syclexp::build(source_bundle, syclexp::properties{ opts, log });
        s_compiledKernel = exec_bundle.ext_oneapi_get_kernel("particle_rtc_kernel");
        s_hasKernel = true;

        return true;

    }
    catch (sycl::exception& e) {
        error_msg = std::string("SYCL RTC failed: ") + e.what();
        s_hasKernel = false;
        return false;
    }
}

void particleRTC_executeInitKernel(unsigned int vboId, std::size_t numParticles) {
    if (!s_compiledInitKernel.has_value()) {
        throw std::runtime_error("No compiled init kernel available");
    }

    sycl::queue queue_ = flib::sycl_handler::get_queue("gl_queue");
    std::size_t n = numParticles;
    std::size_t xdim = static_cast<std::size_t>(std::ceil(std::sqrt(static_cast<double>(n))));
    std::size_t ydim = xdim;

    sycl::kernel& kernel_ref = s_compiledInitKernel.value();

    cl_context clcontext = flib::sycl_handler::get_clContext();
    cl_command_queue clqueue = sycl::get_native<sycl::backend::opencl>(queue_);
    cl_mem clbuffer = clCreateFromGLBuffer(clcontext, CL_MEM_READ_WRITE, vboId, NULL);

    glFinish();
    cl_event acquire_event;
    clEnqueueAcquireGLObjects(clqueue, 1, &clbuffer, 0, NULL, &acquire_event);
    clWaitForEvents(1, &acquire_event);

    queue_.submit([&](sycl::handler& cgh) {
        cgh.set_args(clbuffer, static_cast<int>(n), static_cast<int>(ydim));
        cgh.parallel_for(sycl::nd_range<2>{
            sycl::range<2>{xdim, ydim},
            sycl::range<2>{1, 1}
        }, kernel_ref);
    });
    queue_.wait();

    clFinish(clqueue);
    cl_event release_event;
    clEnqueueReleaseGLObjects(clqueue, 1, &clbuffer, 0, NULL, &release_event);
    clWaitForEvents(1, &release_event);
    glMemoryBarrier(GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT | GL_BUFFER_UPDATE_BARRIER_BIT);
    clReleaseMemObject(clbuffer);
}

void particleRTC_executeKernel(unsigned int vboId, std::size_t numParticles, float dt) {
    if (!s_compiledKernel.has_value()) {
        throw std::runtime_error("No compiled kernel available");
    }

    sycl::queue queue_ = flib::sycl_handler::get_queue("gl_queue");
    std::size_t n = numParticles;
    std::size_t xdim = static_cast<std::size_t>(std::ceil(std::sqrt(static_cast<double>(n))));
    std::size_t ydim = xdim;

    sycl::kernel& kernel_ref = s_compiledKernel.value();

    cl_context clcontext = flib::sycl_handler::get_clContext();
    cl_command_queue clqueue = sycl::get_native<sycl::backend::opencl>(queue_);
    cl_mem clbuffer = clCreateFromGLBuffer(clcontext, CL_MEM_READ_WRITE, vboId, NULL);

    if (clbuffer == NULL) {
        throw std::runtime_error("Failed to create OpenCL buffer from GL buffer");
    }

    glFinish();
    cl_event acquire_event;
    clEnqueueAcquireGLObjects(clqueue, 1, &clbuffer, 0, NULL, &acquire_event);
    clWaitForEvents(1, &acquire_event);

    queue_.submit([&](sycl::handler& cgh) {
        cgh.set_args(clbuffer, static_cast<int>(n), static_cast<int>(ydim), dt);
        cgh.parallel_for(sycl::nd_range<2>{
            sycl::range<2>{xdim, ydim},
            sycl::range<2>{1, 1}
        }, kernel_ref);
    });
    queue_.wait();

    clFinish(clqueue);
    cl_event release_event;
    clEnqueueReleaseGLObjects(clqueue, 1, &clbuffer, 0, NULL, &release_event);
    clWaitForEvents(1, &release_event);
    glMemoryBarrier(GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT | GL_BUFFER_UPDATE_BARRIER_BIT);
    clReleaseMemObject(clbuffer);
}

bool particleRTC_hasKernel() {
    return s_hasKernel && s_compiledKernel.has_value();
}

bool particleRTC_hasInitKernel() {
    return s_compiledInitKernel.has_value();
}

bool particleRTC_isSupported() {
    return flib::sycl_handler::is_rtc_available();
}