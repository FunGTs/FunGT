#if !defined(_ANIMATION_EXPORTER_H_)
#define _ANIMATION_EXPORTER_H_

#include "Physics/AnimationCreator/animation_controller.hpp"
#include "SceneManager/scene_manager.hpp"
#include "PBR/Space/space.hpp"
#include "PBR/TriangleExtractor/triangle_extractor.hpp"
#include <string>
#include <memory>
#include <functional>
namespace fungt {

    /**
     * AnimationExporter - Renders animation frames using PBR path tracer
     *
     * Takes recorded keyframes and renders them frame-by-frame to PNG files
     */
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
            std::shared_ptr<SceneManager> sceneManager,Camera* camera)
            : m_animController(animController)
            , m_sceneManager(sceneManager)
            , m_width(1920)
            , m_height(1080)
            , m_samplesPerPixel(32)
            , m_backend(Compute::Backend::CUDA)
            , m_camera(camera)
        {
        }
        void setBackend(Compute::Backend backend) {  // ← ADD THIS!
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
        void exportAnimation(int startFrame, int endFrame, const std::string& outputDir) {
            std::cout << "\n========================================" << std::endl;
            std::cout << "ANIMATION EXPORT STARTED" << std::endl;
            std::cout << "========================================" << std::endl;
            std::cout << "Frames: " << startFrame << " - " << endFrame << std::endl;
            std::cout << "Total: " << (endFrame - startFrame + 1) << " frames" << std::endl;
            std::cout << "Resolution: " << m_width << "x" << m_height << std::endl;
            std::cout << "Samples: " << m_samplesPerPixel << std::endl;
            std::cout << "Output: " << outputDir << std::endl;
            std::cout << "========================================\n" << std::endl;
            glm::vec3 pos = m_camera->getPosition();
            glm::vec3 front = m_camera->getFront();
            glm::vec3 up = m_camera->getUp();
            float fov = m_camera->getFOV();

            // Calculate look-at point
            glm::vec3 lookAt = pos + front;

            // Convert to PBR format
            fungt::Vec3 pbrPos(pos.x, pos.y, pos.z);
            fungt::Vec3 pbrLookAt(lookAt.x, lookAt.y, lookAt.z);
            fungt::Vec3 pbrUp(up.x, up.y, up.z);
            float aspect = (float)m_width / m_height;


            // Create PBR camera
            PBRCamera pbrCam(pbrPos, pbrLookAt, pbrUp, fov, aspect);
            // Create Space (path tracer) ONCE
            ComputeRender::SetBackend(m_backend); // Fucking important to set backend before initializing Space!
            Space space(pbrCam);
            space.setSamples(m_samplesPerPixel);
            space.InitComputeRenderBackend();

    

            // Render each frame
            for (int frame = startFrame; frame <= endFrame; frame++) {
                std::cout << "[" << (frame - startFrame + 1) << "/" << (endFrame - startFrame + 1)
                    << "] Rendering frame " << frame << "..." << std::endl;

                // 1. Update scene to this frame using keyframes
                m_animController->updateFrame(frame);
                space.ClearGeometry(); // Clear previous frame's geometry and lights
                // 2. Extract triangles from scene (with updated transforms)
                extractTriangles(m_sceneManager.get(), space);

                // 4. Build BVH acceleration structure
                space.BuildBVH();

                // 5. Render with path tracer
                std::vector<fungt::Vec3> framebuffer = space.Render(m_width, m_height);

                // 6. Save PNG with frame number
                char filename[512];
                snprintf(filename, sizeof(filename), "%sframe_%04d.png", outputDir.c_str(), frame);
                Space::SaveFrameBufferAsPNG(framebuffer, m_width, m_height, std::string(filename));
                m_progressCallback(frame - startFrame + 1, endFrame - startFrame + 1); // Update progress bar
                std::cout << "    Saved: " << filename << std::endl;
            }

            std::cout << "\n========================================" << std::endl;
            std::cout << "EXPORT COMPLETE!" << std::endl;
            std::cout << "Total frames rendered: " << (endFrame - startFrame + 1) << std::endl;
            std::cout << "Location: " << outputDir << std::endl;
            std::cout << "\nTo create video, run:" << std::endl;
            std::cout << "ffmpeg -framerate 30 -i " << outputDir << "frame_%04d.png -c:v libx264 -pix_fmt yuv420p -crf 18 output.mp4" << std::endl;
            std::cout << "========================================\n" << std::endl;
        }
    };

} // namespace fungt

#endif // _ANIMATION_EXPORTER_H_