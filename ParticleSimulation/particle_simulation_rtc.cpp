#include "particle_simulation_rtc.hpp"

ParticleRTC::ParticleRTC() {

}

bool ParticleRTC::isSupported() {
    // Check for SVM support
    sycl::queue queue_ = flib::sycl_handler::get_queue();
    cl_device_id cl_dev = sycl::get_native<sycl::backend::opencl>(queue_.get_device());
    cl_device_svm_capabilities svm_caps;
    clGetDeviceInfo(cl_dev, CL_DEVICE_SVM_CAPABILITIES, sizeof(svm_caps), &svm_caps, NULL);

    bool has_svm = (svm_caps & CL_DEVICE_SVM_COARSE_GRAIN_BUFFER) != 0;
    std::string svm_support = has_svm ? "Yes" : "No";
    std::cout << " SVM support: " << svm_support << "\n";
    return flib::sycl_handler::is_rtc_available();;
}

bool ParticleRTC::compileKernel(const std::string& user_code, std::string& error_msg) {
    try {
        sycl::queue queue_ = flib::sycl_handler::get_queue();
        std::string user_header = R"""(
namespace flib {
    template<typename T>
    struct Particle {
        T position[3];
        T velocity[3];
        T acceleration[3];
        T mass;
        
        Particle() {
            for (int i = 0; i < 3; i++) {
                position[i] = 0;
                velocity[i] = 0;
                acceleration[i] = 0;
            }
            mass = 1.0f;
        }
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
    std::cout << "[DEBUG] execute() called\n";
    sycl::queue queue_ = flib::sycl_handler::get_queue();
    if (!compiled_kernel_.has_value()) {
        throw std::runtime_error("No compiled kernel available");
    }
    //std::cout << "[DEBUG] kernel exists\n";

    std::size_t n = static_cast<std::size_t>(numParticles);
    std::size_t xdim = static_cast<std::size_t>(std::ceil(std::sqrt(n)));
    std::size_t ydim = xdim;
    //std::cout << "[DEBUG] n=" << n << " xdim=" << xdim << " ydim=" << ydim << "\n";

    sycl::kernel& kernel_ref = compiled_kernel_.value();
//    std::cout << "[DEBUG] got kernel ref\n";

    cl_context clcontext = flib::sycl_handler::get_clContext();
  //  std::cout << "[DEBUG] got cl context\n";

    cl_command_queue clqueue = sycl::get_native<sycl::backend::opencl>(queue_);
    //std::cout << "[DEBUG] got cl queue\n";

    cl_mem clbuffer = clCreateFromGLBuffer(clcontext, CL_MEM_READ_WRITE, vbo, NULL);
    //std::cout << "[DEBUG] created cl buffer from GL\n";

    if (clbuffer == NULL) {
        throw std::runtime_error("Failed to create OpenCL buffer from GL buffer");
    }

    sycl::context syclCtx = flib::sycl_handler::get_sycl_context();
   // std::cout << "[DEBUG] got sycl context\n";

    glFinish();
    //std::cout << "[DEBUG] glFinish done\n";

    cl_event acquire_event;
    clEnqueueAcquireGLObjects(clqueue, 1, &clbuffer, 0, NULL, &acquire_event);
   // std::cout << "[DEBUG] acquire enqueued\n";

    clWaitForEvents(1, &acquire_event);
    //std::cout << "[DEBUG] acquire complete\n";

    {
      //  std::cout << "[DEBUG] creating buffer\n";
        // sycl::buffer<flib::Particle<float>> buf =
        //     sycl::make_buffer<sycl::backend::opencl, flib::Particle<float>>(clbuffer, syclCtx);
        //std::cout << "[DEBUG] buffer created\n";

        queue_.submit([&](sycl::handler& cgh) {
          //  std::cout << "[DEBUG] in submit lambda\n";
            //auto acc = buf.template get_access<sycl::access::mode::read_write>(cgh);
            //std::cout << "[DEBUG] got accessor\n";
            //auto multi_ptr = acc.template get_multi_ptr<sycl::access::decorated::no>();
            //auto particles_ptr = multi_ptr.get();
            
            cgh.set_args(clbuffer, static_cast<int>(n), static_cast<int>(ydim), dt);
            //std::cout << "[DEBUG] set args\n";
            cgh.parallel_for(
                sycl::nd_range<2>{
                sycl::range<2>{xdim, ydim},  // global size
                    sycl::range<2>{1, 1}         // local work-group size
            },
                kernel_ref
            );
            //std::cout << "[DEBUG] parallel_for submitted\n";
            });
        //std::cout << "[DEBUG] submit done\n";

        queue_.wait();
        //std::cout << "[DEBUG] queue wait done\n";
    }
    //std::cout << "[DEBUG] buffer destroyed\n";

    clFinish(clqueue);
    //std::cout << "[DEBUG] clFinish done\n";

    cl_event release_event;
    clEnqueueReleaseGLObjects(clqueue, 1, &clbuffer, 0, NULL, &release_event);
    //std::cout << "[DEBUG] release enqueued\n";

    clWaitForEvents(1, &release_event);
   // std::cout << "[DEBUG] release complete\n";

    glMemoryBarrier(GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT | GL_BUFFER_UPDATE_BARRIER_BIT);
   // std::cout << "[DEBUG] memory barrier done\n";

    clReleaseMemObject(clbuffer);
    //std::cout << "[DEBUG] execute complete\n";
}