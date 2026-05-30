#ifndef PROGRESSIVE_PATH_TRACER_HPP
#define PROGRESSIVE_PATH_TRACER_HPP

#include "include/prerequisites.hpp"
#include "Camera/camera.hpp"
#include "SceneManager/scene_manager.hpp"
#include "PBR/PBRCamera/pbr_camera.hpp"
#include "PBR/Render/include/compute_backends.hpp"
#include <vector>
#include <memory>
class Space;
class ProgressivePathTracer {
private:
    std::unique_ptr<Space> m_space;
    int m_width = 0;
    int m_height = 0;
    std::vector<float> m_accumBuffer;  // Accumulation buffer
    bool m_initialized = false;

public:
    ProgressivePathTracer();
    ~ProgressivePathTracer();

    // Initialize with camera and scene
    void initialize(Camera* viewportCam,
        std::shared_ptr<SceneManager> sceneManager,
        int width, int height);

    // Render one sample and accumulate
    void renderSample(int sample, GLuint targetTexture);

    // Reset accumulation
    void reset();

    bool isInitialized() const { return m_initialized; }
};

#endif // PROGRESSIVE_PATH_TRACER_HPP