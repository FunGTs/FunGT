#ifndef PROGRESSIVE_PATH_TRACER_HPP
#define PROGRESSIVE_PATH_TRACER_HPP

#include "Camera/camera.hpp"
#include "SceneManager/scene_manager.hpp"
#include "PBR/PBRCamera/pbr_camera.hpp"
#include "PBR/Render/include/compute_backends.hpp"
#include "Renders/display_graphics.hpp"
#include <vector>
#include <memory>

class Space;

class ProgressivePathTracer {
protected:
    std::unique_ptr<Space> m_space;
    int m_width = 0;
    int m_height = 0;
    std::vector<float> m_accumBuffer;
    bool m_initialized = false;

public:
    ProgressivePathTracer() = default;
    virtual ~ProgressivePathTracer();

    static std::unique_ptr<ProgressivePathTracer> create();

    void initialize(Camera* viewportCam,
        std::shared_ptr<SceneManager> sceneManager,
        int width, int height, bool ogl_interop = false);
    void updateCamera(Camera* viewportCam, int width, int height);
    void reloadLights(std::shared_ptr<SceneManager> sceneManager);
    void reloadSceneShading(std::shared_ptr<SceneManager> sceneManager);

    virtual void renderSample(int sample, uint32_t targetTexture) = 0;
    virtual void renderSampleInterop(int sample, uint32_t targetPBO) = 0;
    void releaseOpenGLInteropResources();

    void reset();

    bool isInitialized() const { return m_initialized; }
};

#endif // PROGRESSIVE_PATH_TRACER_HPP
