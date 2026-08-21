#include "PBR/Space/space.hpp"
#include "PBR/Render/shared/opencl/fgt_opencl_data.h"
#include <algorithm>
#include <cmath>
#include <type_traits>

const int IMAGE_WIDTH = 800;
const int IMAGE_HEIGHT = 400;

PBRCamera frameScene(
    const SimpleModel& model,
    float aspectRatio,
    float verticalFov = 50.0f,
    float padding = 1.2f)
{
    glm::vec3 boundsMin;
    glm::vec3 boundsMax;
    if (!model.getWorldBounds(boundsMin, boundsMax)) {
        throw std::runtime_error("Cannot frame a model with no vertices.");
    }

    const glm::vec3 center = (boundsMin + boundsMax) * 0.5f;
    const float radius = std::max(
        glm::length(boundsMax - boundsMin) * 0.5f,
        0.001f);
    const float verticalFovRadians = glm::radians(verticalFov);
    const float horizontalFovRadians = 2.0f * std::atan(
        std::tan(verticalFovRadians * 0.5f) * aspectRatio);
    const float limitingFov = std::min(
        verticalFovRadians,
        horizontalFovRadians);
    const float distance =
        padding * radius / std::sin(limitingFov * 0.5f);

    return PBRCamera(
        fungt::Vec3(center.x, center.y, center.z + distance),
        fungt::Vec3(center.x, center.y, center.z),
        fungt::Vec3(0.0f, 1.0f, 0.0f),
        verticalFov,
        aspectRatio);
}

SceneLight frameSceneLight(const SimpleModel& model)
{
    glm::vec3 boundsMin;
    glm::vec3 boundsMax;
    if (!model.getWorldBounds(boundsMin, boundsMax)) {
        throw std::runtime_error("Cannot light a model with no vertices.");
    }

    const glm::vec3 center = (boundsMin + boundsMax) * 0.5f;
    const float radius = std::max(
        glm::length(boundsMax - boundsMin) * 0.5f,
        0.001f);

    SceneLight light;
    light.type = SceneLightType::Point;
    light.name = "Framed model light";
    light.position = center + glm::vec3(-0.8f, 1.2f, 0.8f) * radius;
    light.color = glm::vec3(1.0f);
    light.power = 10.0f * radius * radius;
    return light;
}

int main()
{
    std::cout << std::boolalpha
              << "sizeof(Vec3) == sizeof(fgt_vec3): "
              << (sizeof(fungt::Vec3) == sizeof(fgt_vec3)) << '\n'
              << "alignof(Vec3) == alignof(fgt_vec3): "
              << (alignof(fungt::Vec3) == alignof(fgt_vec3)) << '\n'
              << "Vec3 is trivially copyable: "
              << std::is_trivially_copyable_v<fungt::Vec3> << std::endl;

    // ComputeRender::SetBackend(Compute::Backend::OPENCL);
    // std::cout << "Initializing standalone PBR backend: "
    //           << ComputeRender::GetBackendName() << std::endl;

    // Space space;
    // space.InitComputeRenderBackend();

    // std::cout << "OpenCL backend initialized successfully." << std::endl;

    // return 0;
    ModelPaths monkey;
    DisplayGraphics::SetBackend(Backend::OpenGL);
    monkey.path = getAssetPath("assets_local/opencl_logo2/scene.gltf");
    //monkey.vs_path = getAssetPath("resources/luxoball_vs.glsl");
    //monkey.fs_path = getAssetPath("resources/luxoball_fs.glsl");

    ComputeRender::SetBackend(Compute::Backend::OPENCL);
    std::cout << "Backend in use: " << ComputeRender::GetBackendName() << std::endl;

    std::shared_ptr<SimpleModel> monkey_model = SimpleModel::create();
    monkey_model->LoadModelData(monkey);
    monkey_model->position(0.0f, -5.0f, 2.0f);
    monkey_model->rotation(0.0f, 25.0f, 0.0f);

    const float aspectRatio =
        float(IMAGE_WIDTH) / float(IMAGE_HEIGHT);
    PBRCamera camera = frameScene(*monkey_model, aspectRatio);

    Space space(camera);
    space.InitComputeRenderBackend();
    space.loadLightsFromScene({frameSceneLight(*monkey_model)});
    space.LoadModelToRender(*monkey_model);
    space.setSamples(100);
    space.BuildBVH();

    auto totalStart = std::chrono::high_resolution_clock::now();
    auto framebuffer = space.Render(IMAGE_WIDTH, IMAGE_HEIGHT);
    auto totalEnd = std::chrono::high_resolution_clock::now();

    const std::size_t expectedPixels =
        static_cast<std::size_t>(IMAGE_WIDTH) * IMAGE_HEIGHT;
    if (framebuffer.size() != expectedPixels) {
        std::cerr << "OpenCL framebuffer size mismatch: expected "
                  << expectedPixels << ", received "
                  << framebuffer.size() << std::endl;
        return 1;
    }

    std::cout << "OpenCL skeleton returned " << framebuffer.size()
              << " pixels." << std::endl;
    std::cout << "First pixel: ("
              << framebuffer.front().x << ", "
              << framebuffer.front().y << ", "
              << framebuffer.front().z << ")" << std::endl;

    Space::SaveFrameBufferAsPNG(framebuffer, IMAGE_WIDTH, IMAGE_HEIGHT);

    auto totalTime = std::chrono::duration_cast<std::chrono::milliseconds>(
        totalEnd - totalStart).count();
    std::cout << "\n========== TIMING RESULTS ==========" << std::endl;
    std::cout << "Total time:       " << totalTime << " ms" << std::endl;
    std::cout << "====================================\n" << std::endl;

    return 0;
}
