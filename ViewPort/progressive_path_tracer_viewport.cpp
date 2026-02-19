#include "progressive_path_tracer_viewport.hpp"

void ProgressivePathTracer::initialize(Camera* viewportCam,
    std::shared_ptr<SceneManager> sceneManager,
    int width, int height)
{
    std::cout << "Initializing progressive path tracer: " << width << "x" << height << std::endl;
    ComputeRender::SetBackend(Compute::Backend::SYCL);  // Or CUDA
    std::cout << "Using backend: " << ComputeRender::GetBackendName() << std::endl;
    m_width = width;
    m_height = height;

    // Reset accumulation buffer
    m_accumBuffer.clear();
    m_accumBuffer.resize(width * height * 4, 0.0f);

    // Sync camera from viewport
    glm::vec3 pos = viewportCam->getPosition();
    glm::vec3 front = viewportCam->getFront();
    glm::vec3 up = viewportCam->getUp();
    float fov = viewportCam->getFOV();

    glm::vec3 lookAt = pos + front;
    float aspect = (float)width / height;

    // Convert to PBR format
    fungt::Vec3 pbrPos(pos.x, pos.y, pos.z);
    fungt::Vec3 pbrLookAt(lookAt.x, lookAt.y, lookAt.z);
    fungt::Vec3 pbrUp(up.x, up.y, up.z);

    // Create PBR camera
    PBRCamera pbrCam(pbrPos, pbrLookAt, pbrUp, fov, aspect);

    // Create space
    m_space = std::make_unique<Space>(pbrCam);
    m_space->InitComputeRenderBackend();

    // Load scene objects
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

    // Build BVH
    std::cout << "Building BVH..." << std::endl;
    m_space->BuildBVH();

    m_initialized = true;
    std::cout << "Progressive path tracer initialized" << std::endl;
}

void ProgressivePathTracer::renderSample(int sample, GLuint targetTexture)
{
    if (!m_initialized || !m_space) {
        std::cerr << "Path tracer not initialized!" << std::endl;
        return;
    }

    // Render ONE sample (set samples to 1)
    m_space->setSamples(1);
    auto framebuffer = m_space->Render(m_width, m_height);
    std::cout << "Framebuffer size: " << framebuffer.size() << std::endl;  // DEBUG
    std::cout << "Expected size: " << (m_width * m_height) << std::endl;   // DEBUG
    if (!framebuffer.empty()) {
        std::cout << "First pixel: (" << framebuffer[0].x << ", "
            << framebuffer[0].y << ", " << framebuffer[0].z << ")" << std::endl;  // DEBUG
    }
    // Accumulate using progressive average
// Accumulate using progressive average
    float weight = 1.0f / (sample + 1);

    for (int i = 0; i < m_width * m_height; i++) {
        int bufIdx = i * 4;  // RGBA index in accumBuffer

        // Accumulate RGB separately
        m_accumBuffer[bufIdx + 0] = m_accumBuffer[bufIdx + 0] * (1.0f - weight) + framebuffer[i].x * weight;  // R
        m_accumBuffer[bufIdx + 1] = m_accumBuffer[bufIdx + 1] * (1.0f - weight) + framebuffer[i].y * weight;  // G
        m_accumBuffer[bufIdx + 2] = m_accumBuffer[bufIdx + 2] * (1.0f - weight) + framebuffer[i].z * weight;  // B
        m_accumBuffer[bufIdx + 3] = 1.0f;  // Alpha always 1
    }
    // Check accumulated value
    std::cout << "Accum first pixel: (" << m_accumBuffer[0] << ", "
        << m_accumBuffer[1] << ", " << m_accumBuffer[2] << ")" << std::endl;
    // NEW - Apply gamma correction like SaveFrameBufferAsPNG does:
    std::vector<float> gammaCorrected(m_width * m_height * 4);
    for (int i = 0; i < m_width * m_height; i++) {
        int srcIdx = i * 4;
        int dstIdx = i * 4;

        // Gamma correct (1.0 / 2.2)
        gammaCorrected[dstIdx + 0] = std::pow(std::clamp(m_accumBuffer[srcIdx + 0], 0.0f, 1.0f), 1.0f / 2.2f);
        gammaCorrected[dstIdx + 1] = std::pow(std::clamp(m_accumBuffer[srcIdx + 1], 0.0f, 1.0f), 1.0f / 2.2f);
        gammaCorrected[dstIdx + 2] = std::pow(std::clamp(m_accumBuffer[srcIdx + 2], 0.0f, 1.0f), 1.0f / 2.2f);
        gammaCorrected[dstIdx + 3] = 1.0f;
    }
    // Upload to OpenGL texture
    glBindTexture(GL_TEXTURE_2D, targetTexture);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_width, m_height,
        GL_RGBA, GL_FLOAT, gammaCorrected.data());
    glBindTexture(GL_TEXTURE_2D, 0);
}

void ProgressivePathTracer::reset()
{
    std::fill(m_accumBuffer.begin(), m_accumBuffer.end(), 0.0f);
    m_initialized = false;
}