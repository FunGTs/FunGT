#include "progressive_path_tracer_viewport.hpp"
#include "opengl/opengl_progressive_path_tracer.hpp"
// #include "vulkan/vulkan_progressive_path_tracer.hpp"
#include "PBR/Space/space.hpp"

ProgressivePathTracer::~ProgressivePathTracer() = default;

std::unique_ptr<ProgressivePathTracer> ProgressivePathTracer::create()
{
    switch (DisplayGraphics::GetBackend()) {
        case Backend::OpenGL: return std::make_unique<OpenGLProgressivePathTracer>();
        case Backend::Vulkan: throw std::runtime_error("Vulkan path tracer not yet implemented");
        case Backend::Metal:  throw std::runtime_error("Metal path tracer not yet implemented");
    }
    throw std::runtime_error("Unknown backend");
}

void ProgressivePathTracer::initialize(Camera* viewportCam,
    std::shared_ptr<SceneManager> sceneManager,
    int width, int height)
{
    std::cout << "Initializing progressive path tracer: " << width << "x" << height << std::endl;
    ComputeRender::SetBackend(Compute::Backend::SYCL);
    std::cout << "Using backend: " << ComputeRender::GetBackendName() << std::endl;
    m_width = width;
    m_height = height;

    m_accumBuffer.clear();
    m_accumBuffer.resize(width * height * 4, 0.0f);

    glm::vec3 pos   = viewportCam->getPosition();
    glm::vec3 front = viewportCam->getFront();
    glm::vec3 up    = viewportCam->getUp();
    float fov       = viewportCam->getFOV();

    glm::vec3 lookAt = pos + front;
    float aspect = (float)width / height;

    fungt::Vec3 pbrPos(pos.x, pos.y, pos.z);
    fungt::Vec3 pbrLookAt(lookAt.x, lookAt.y, lookAt.z);
    fungt::Vec3 pbrUp(up.x, up.y, up.z);

    PBRCamera pbrCam(pbrPos, pbrLookAt, pbrUp, fov, aspect);
    m_space = std::make_unique<Space>(pbrCam);
    m_space->InitComputeRenderBackend();
    m_space->loadLightsFromScene(sceneManager->getLights());

    const auto& objects = sceneManager->getRenderable();
    int modelCount = 0;
    int geometryCount = 0;

    for (auto& obj : objects) {
        auto simpleModel = std::dynamic_pointer_cast<SimpleModel>(obj);
        if (simpleModel) {
            m_space->LoadModelToRender(*simpleModel);
            modelCount++;
            continue;
        }
        auto simpleGeometry = std::dynamic_pointer_cast<SimpleGeometry>(obj);
        if (simpleGeometry) {
            m_space->LoadGeometryToRender(*simpleGeometry);
            geometryCount++;
        }
    }

    if (modelCount == 0 && geometryCount == 0) {
        std::cerr << "Warning: No models or geometries in scene for path tracing" << std::endl;
        m_initialized = false;
        return;
    }

    std::cout << "Loaded " << modelCount << " models, " << geometryCount << " geometries" << std::endl;
    std::cout << "Building BVH..." << std::endl;
    m_space->BuildBVH();

    m_initialized = true;
    std::cout << "Progressive path tracer initialized" << std::endl;
}

void ProgressivePathTracer::reset()
{
    std::fill(m_accumBuffer.begin(), m_accumBuffer.end(), 0.0f);
    m_initialized = false;
}
