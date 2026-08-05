#include "animation_exporter.hpp"
#include "PBR/Space/space.hpp"
#include "PBR/PBRCamera/pbr_camera.hpp"
#include "PBR/TriangleExtractor/triangle_extractor.hpp"
#include "Vector/vector3.hpp"

namespace fungt {

    void AnimationExporter::exportAnimation(int startFrame, int endFrame, const std::string& outputDir) {
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

        glm::vec3 lookAt = pos + front;

        fungt::Vec3 pbrPos(pos.x, pos.y, pos.z);
        fungt::Vec3 pbrLookAt(lookAt.x, lookAt.y, lookAt.z);
        fungt::Vec3 pbrUp(up.x, up.y, up.z);
        float aspect = (float)m_width / m_height;

        PBRCamera pbrCam(pbrPos, pbrLookAt, pbrUp, fov, aspect);

        ComputeRender::SetBackend(m_backend);
        Space space(pbrCam);
        space.setSamples(m_samplesPerPixel);
        space.InitComputeRenderBackend();

        for (int frame = startFrame; frame <= endFrame; frame++) {
            std::cout << "[" << (frame - startFrame + 1) << "/" << (endFrame - startFrame + 1)
                << "] Rendering frame " << frame << "..." << std::endl;

            m_animController->updateFrame(frame);
            space.ClearGeometry();

            extractTriangles(m_sceneManager.get(), space);

            space.BuildBVH();

            std::vector<fungt::Vec3> framebuffer = space.Render(m_width, m_height);

            char filename[512];
            snprintf(filename, sizeof(filename), "%sframe_%04d.png", outputDir.c_str(), frame);
            Space::SaveFrameBufferAsPNG(framebuffer, m_width, m_height, std::string(filename));
            m_progressCallback(frame - startFrame + 1, endFrame - startFrame + 1);
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

} // namespace fungt