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
    //Lights from Scene
    m_space->loadLightsFromScene(sceneManager->getLights());
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
    auto framebuffer = m_space->Render(m_width, m_height,sample);
    
    // ACCUMULATE (just add, don't average yet)
    for (int i = 0; i < m_width * m_height; i++) {
        int bufIdx = i * 4;
        m_accumBuffer[bufIdx + 0] += framebuffer[i].x;  // Just sum
        m_accumBuffer[bufIdx + 1] += framebuffer[i].y;
        m_accumBuffer[bufIdx + 2] += framebuffer[i].z;
        m_accumBuffer[bufIdx + 3] = 1.0f;
    }

    // DISPLAY (average and gamma correct)
    float invSamples = 1.0f / (sample + 1);
    std::vector<float> gammaCorrected(m_width * m_height * 4);

    for (int i = 0; i < m_width * m_height; i++) {
        int idx = i * 4;

        // Average first, then gamma correct
        float r = m_accumBuffer[idx + 0] * invSamples;
        float g = m_accumBuffer[idx + 1] * invSamples;
        float b = m_accumBuffer[idx + 2] * invSamples;

        gammaCorrected[idx + 0] = std::pow(std::clamp(r, 0.0f, 1.0f), 1.0f / 2.2f);
        gammaCorrected[idx + 1] = std::pow(std::clamp(g, 0.0f, 1.0f), 1.0f / 2.2f);
        gammaCorrected[idx + 2] = std::pow(std::clamp(b, 0.0f, 1.0f), 1.0f / 2.2f);
        gammaCorrected[idx + 3] = 1.0f;
    }

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