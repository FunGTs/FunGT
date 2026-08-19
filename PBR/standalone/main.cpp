#include "PBR/Space/space.hpp"
const int IMAGE_WIDTH = 800;
const int IMAGE_HEIGHT = 400;

int main()
{
    // ComputeRender::SetBackend(Compute::Backend::OPENCL);
    // std::cout << "Initializing standalone PBR backend: "
    //           << ComputeRender::GetBackendName() << std::endl;

    // Space space;
    // space.InitComputeRenderBackend();

    // std::cout << "OpenCL backend initialized successfully." << std::endl;

    // return 0;
    ModelPaths monkey;
    DisplayGraphics::SetBackend(Backend::OpenGL);
    monkey.path = getAssetPath("assets_local/Obj/Woody/woody-head.obj");
    //monkey.vs_path = getAssetPath("resources/luxoball_vs.glsl");
    //monkey.fs_path = getAssetPath("resources/luxoball_fs.glsl");

    ComputeRender::SetBackend(Compute::Backend::CUDA);
    std::cout << "Backend in use: " << ComputeRender::GetBackendName() << std::endl;

    std::shared_ptr<SimpleModel> monkey_model = SimpleModel::create();
    monkey_model->LoadModelData(monkey);

    PBRCamera camera(
        fungt::Vec3(0, 2.5, 30),
        fungt::Vec3(0, 1.8, 0),
        fungt::Vec3(0, 1, 0),
        50.0f,
        float(IMAGE_WIDTH) / float(IMAGE_HEIGHT)
    );

    Space space(camera);
    space.InitComputeRenderBackend();
    space.LoadModelToRender(*monkey_model);
    space.setSamples(100);
    space.BuildBVH();

    auto totalStart = std::chrono::high_resolution_clock::now();
    auto framebuffer = space.Render(IMAGE_WIDTH, IMAGE_HEIGHT);
    auto totalEnd = std::chrono::high_resolution_clock::now();

    Space::SaveFrameBufferAsPNG(framebuffer, IMAGE_WIDTH, IMAGE_HEIGHT);

    auto totalTime = std::chrono::duration_cast<std::chrono::milliseconds>(
        totalEnd - totalStart).count();
    std::cout << "\n========== TIMING RESULTS ==========" << std::endl;
    std::cout << "Total time:       " << totalTime << " ms" << std::endl;
    std::cout << "====================================\n" << std::endl;

    return 0;
}
