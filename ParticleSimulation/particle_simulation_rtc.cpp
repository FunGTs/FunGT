#include "particle_simulation_rtc.hpp"

ParticleRTC::ParticleRTC(std::size_t numOfParticles)
: m_NumParticles{numOfParticles}{
    // INITIALIZE SYCL WITH GL INTEROP (ParticleSimulation owns this responsibility)
    std::cout << "Initializing SYCL for RTC ParticleSimulation..." << std::endl;
    flib::sycl_handler::register_queue("gl_queue", flib::device::GPU, flib::vendor::INTEL, flib::backend::OPENCL);
    flib::sycl_handler::create_gl_interop_context("gl_queue"); // replaces that queue's context in-place
    flib::sycl_handler::get_device_info("gl_queue");
    m_pSet.SetNumParticles(m_NumParticles);
    std::cout << "Particle RTC system constructor" << std::endl;
    std::cout << "Num particles: " << m_pSet._particles.size() << std::endl;

    m_fs = getAssetPath("resources/particle.fs");
    m_vs = getAssetPath("resources/particle.vs");

    //this->init();
    m_shader.create(m_vs, m_fs);

}

bool ParticleRTC::compileInitKernel(const std::string& user_init_code, std::string& error_msg)
{
    try {
        sycl::queue queue_ = flib::sycl_handler::get_queue("gl_queue");

        // Only embed the user code as virtual header
        std::string user_header = R"""(
struct UserInit {
    void operator()(fgt::Particle<float>& p, int index) const {
        )""" + user_init_code + R"""(
    }
};
)""";

        static constexpr auto sycl_source = R"""(
#include <sycl/sycl.hpp>
#include "ParticleSimulation/particle.hpp"  // Physical file!
#include "user_init.h"                      // Virtual file

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

        // Add include path for physical headers
        std::string include_path = "-I" + getAssetPath("");  // Points to project root
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

        // std::cout << "Init Kernel Compiled output:\n" << compiler_log << "\n";

        compiled_init_kernel_ = exec_bundle.ext_oneapi_get_kernel("particle_init_kernel");

        return true;

    }
    catch (sycl::exception& e) {
        error_msg = std::string("Init kernel RTC failed: ") + e.what();
        compiled_init_kernel_.reset();
        return false;
    }
}
bool ParticleRTC::isSupported() {
    
    return flib::sycl_handler::is_rtc_available();
}

void ParticleRTC::executeInitKernel()
{
    if (!compiled_init_kernel_.has_value()) {
        throw std::runtime_error("No compiled init kernel available");
    }
   
    std::size_t n = m_NumParticles;
    unsigned int vbo = m_vbo.getId();
    sycl::queue queue_ = flib::sycl_handler::get_queue("gl_queue");

    std::size_t xdim = static_cast<std::size_t>(std::ceil(std::sqrt(n)));
    std::size_t ydim = xdim;

    sycl::kernel& kernel_ref = compiled_init_kernel_.value();

    // GL interop
    cl_context clcontext = flib::sycl_handler::get_clContext();
    cl_command_queue clqueue = sycl::get_native<sycl::backend::opencl>(queue_);
    cl_mem clbuffer = clCreateFromGLBuffer(clcontext, CL_MEM_READ_WRITE, vbo, NULL);

    glFinish();
    cl_event acquire_event;
    clEnqueueAcquireGLObjects(clqueue, 1, &clbuffer, 0, NULL, &acquire_event);
    clWaitForEvents(1, &acquire_event);

    // Execute init kernel
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
    
    // std::cout << "Particles initialized with RTC kernel\n";
}

bool ParticleRTC::compileKernel(const std::string& user_code, std::string& error_msg) {
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
#include "ParticleSimulation/particle.hpp"  // Physical file
#include "user_update.h"                    // Virtual file

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

        // std::cout << "RTC Compiler output:\n" << compiler_log << "\n";

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

void ParticleRTC::executeKernel() {

    sycl::queue queue_ = flib::sycl_handler::get_queue("gl_queue");
    if (!compiled_kernel_.has_value()) {
        throw std::runtime_error("No compiled kernel available");
    }
    // std::cout << "[DEBUG] kernel exists\n";
    float dt = 0.005;
    unsigned int vbo = m_vbo.getId(); 
    std::size_t n = m_NumParticles;
    std::size_t xdim = static_cast<std::size_t>(std::ceil(std::sqrt(n)));
    std::size_t ydim = xdim;
    // std::cout << "[DEBUG] n=" << n << " xdim=" << xdim << " ydim=" << ydim << "\n";

    sycl::kernel& kernel_ref = compiled_kernel_.value();
     // std::cout << "[DEBUG] got kernel ref\n";

    cl_context clcontext = flib::sycl_handler::get_clContext();
   // std::cout << "[DEBUG] got cl context\n";

    cl_command_queue clqueue = sycl::get_native<sycl::backend::opencl>(queue_);
    // std::cout << "[DEBUG] got cl queue\n";

    cl_mem clbuffer = clCreateFromGLBuffer(clcontext, CL_MEM_READ_WRITE, vbo, NULL);
    // std::cout << "[DEBUG] created cl buffer from GL\n";

    if (clbuffer == NULL) {
        throw std::runtime_error("Failed to create OpenCL buffer from GL buffer");
    }

    sycl::context syclCtx = flib::sycl_handler::get_sycl_context();
   // std::cout << "[DEBUG] got sycl context\n";

    glFinish();
    // std::cout << "[DEBUG] glFinish done\n";

    cl_event acquire_event;
    clEnqueueAcquireGLObjects(clqueue, 1, &clbuffer, 0, NULL, &acquire_event);
    // std::cout << "[DEBUG] acquire enqueued\n";

    clWaitForEvents(1, &acquire_event);
    // std::cout << "[DEBUG] acquire complete\n";

    {
      //  std::cout << "[DEBUG] creating buffer\n";
        // sycl::buffer<fgt::Particle<float>> buf =
        //     sycl::make_buffer<sycl::backend::opencl, fgt::Particle<float>>(clbuffer, syclCtx);
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
        // std::cout << "[DEBUG] queue wait done\n";
    }
    // std::cout << "[DEBUG] buffer destroyed\n";

    clFinish(clqueue);
    // std::cout << "[DEBUG] clFinish done\n";

    cl_event release_event;
    clEnqueueReleaseGLObjects(clqueue, 1, &clbuffer, 0, NULL, &release_event);
    //std::cout << "[DEBUG] release enqueued\n";

    clWaitForEvents(1, &release_event);
   // std::cout << "[DEBUG] release complete\n";

    glMemoryBarrier(GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT | GL_BUFFER_UPDATE_BARRIER_BIT);
   // std::cout << "[DEBUG] memory barrier done\n";

    clReleaseMemObject(clbuffer);
    // std::cout << "[DEBUG] execute complete\n";
    // CHECK IF POSITIONS CHANGED
    // static bool first_check = true;
    // if (first_check) {
    //     glBindBuffer(GL_ARRAY_BUFFER, vbo);
    //     fgt::Particle<float>* particles = (fgt::Particle<float>*)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_ONLY);
    //     if (particles) {
    //         std::cout << "[AFTER UPDATE] Particle 0:\n";
    //         std::cout << "  pos(" << particles[0].position[0] << ", " << particles[0].position[1] << ", " << particles[0].position[2] << ")\n";
    //         std::cout << "  vel(" << particles[0].velocity[0] << ", " << particles[0].velocity[1] << ", " << particles[0].velocity[2] << ")\n";
    //         glUnmapBuffer(GL_ARRAY_BUFFER);
    //     }
    //     glBindBuffer(GL_ARRAY_BUFFER, 0);
    //     first_check = false;
    // }
}

void ParticleRTC::init()
{
    // std::cout<<" INIT RTC "<<std::endl;
    m_vao.genVAO();
    m_vbo.genVB();

    m_vao.bind();
    m_vbo.bind();

    // Allocate empty VBO (no CPU data needed for RTC)
    m_vbo.bufferData(nullptr,
        m_pSet._particles.size() * sizeof(fgt::Particle<float>),
        GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
        sizeof(fgt::Particle<float>),
        (void*)offsetof(fgt::Particle<float>, position));
    glEnableVertexAttribArray(0);

    m_vao.unbind();

    
    // GPU kernel fills the VBO
    if (hasInitKernel()) {
        executeInitKernel();
    }
    m_isReady = true;
}

void ParticleRTC::simulation()
{
    
    if (m_isReady && hasKernel()) {  // CHECK READY FLAG
        // std::cout << "Running Simulation\n";
        executeKernel();
    }
}

void ParticleRTC::draw()
{
    this->simulation();
    glEnable(GL_PROGRAM_POINT_SIZE);
    m_vao.bind();
    glDrawArrays(GL_POINTS, 0, m_NumParticles);
}

Shader& ParticleRTC::getShader()
{
    // TODO: insert return statement here
    return m_shader;
}

void ParticleRTC::updateTime(float deltaTime)
{
    this->m_deltaTime = deltaTime;
}

void ParticleRTC::setViewMatrix(const glm::mat4& viewMatrix)
{
    m_viewMatrix = viewMatrix;
}

glm::mat4 ParticleRTC::getViewMatrix()
{
    return m_viewMatrix;
}

void ParticleRTC::updateModelMatrix()
{
    m_ModelMatrix = glm::mat4(1.f);
    m_ModelMatrix = glm::translate(m_ModelMatrix, m_position);
    m_ModelMatrix = glm::rotate(m_ModelMatrix, glm::radians(m_rotation.x), glm::vec3(1.f, 0.f, 0.f));
    m_ModelMatrix = glm::rotate(m_ModelMatrix, glm::radians(m_rotation.y), glm::vec3(0.f, 1.f, 0.f));
    m_ModelMatrix = glm::rotate(m_ModelMatrix, glm::radians(m_rotation.z), glm::vec3(0.f, 0.f, 1.f));
    m_ModelMatrix = glm::scale(m_ModelMatrix, m_scale);
}

glm::mat4 ParticleRTC::getModelMatrix() const
{
    return m_ModelMatrix;
}
