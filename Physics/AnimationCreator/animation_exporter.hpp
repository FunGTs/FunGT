#if !defined(_ANIMATION_EXPORTER_H_)
#define _ANIMATION_EXPORTER_H_

#include "Physics/AnimationCreator/animation_controller.hpp"
#include "SceneManager/scene_manager.hpp"
#include "PBR/Render/include/compute_backends.hpp"
#include "Camera/camera.hpp"
#include <string>
#include <memory>
#include <functional>

namespace fungt {

    class AnimationExporter {
    private:
        std::shared_ptr<AnimationController> m_animController;
        std::shared_ptr<SceneManager> m_sceneManager;
        std::function<void(int, int)> m_progressCallback;
        int m_width;
        int m_height;
        int m_samplesPerPixel;
        Compute::Backend m_backend;
        Camera* m_camera;

    public:
        AnimationExporter(std::shared_ptr<AnimationController> animController,
            std::shared_ptr<SceneManager> sceneManager, Camera* camera)
            : m_animController(animController)
            , m_sceneManager(sceneManager)
            , m_width(1920)
            , m_height(1080)
            , m_samplesPerPixel(32)
            , m_backend(Compute::Backend::CUDA)
            , m_camera(camera)
        {
        }

        void setBackend(Compute::Backend backend) {
            m_backend = backend;
        }

        void setResolution(int width, int height) {
            m_width = width;
            m_height = height;
        }

        void setSamples(int samples) {
            m_samplesPerPixel = samples;
        }

        void setProgressCallback(std::function<void(int, int)> cb) {
            m_progressCallback = cb;
        }

        void exportAnimation(int startFrame, int endFrame, const std::string& outputDir);
    };

} // namespace fungt

#endif // _ANIMATION_EXPORTER_H_